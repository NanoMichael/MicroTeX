#include "atom/atom_misc.h"

#include "atom/atom_basic.h"
#include "atom/atom_delim.h"
#include "env/units.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

using namespace std;
using namespace microtex;

sptr<Box> BigSymbolAtom::createBox(Env& env) {
  auto b = microtex::createVDelim(_delim, env, _size);
  const auto axis = env.mathConsts().axisHeight() * env.scale();
  b->_shift = -(b->vlen() / 2 - b->_height) - axis;
  return sptrOf<HBox>(b);
}

LapedAtom::LapedAtom(const sptr<Atom>& a, char type) : _type(type) {
	_at = a == nullptr ? sptrOf<EmptyAtom>() : a;
}

sptr<Box> LapedAtom::createBox(Env& env) {
  auto b = _at->createBox(env);
  auto* vb = new VBox();
  vb->add(b);
  vb->_width = 0;
  switch (_type) {
    case 'l': b->_shift = -b->_width; break;
    case 'r': b->_shift = 0; break;
    default: b->_shift = -b->_width / 2; break;
  }

  return sptr<Box>(vb);
}

sptr<Box> RaiseAtom::createBox(Env& env) {
  auto base = _base->createBox(env);
  if (_raise.isValid()) {
    base->_shift = Units::fsize(_raise, env);
  }
  auto hb = sptrOf<HBox>(base);
  if (_height.isValid()) {
    hb->_height = Units::fsize(_height, env);
  }
  if (_depth.isValid()) {
    hb->_depth = Units::fsize(_depth, env);
  }
  return hb;
}

sptr<Box> ResizeAtom::createBox(Env& env) {
  auto box = _base->createBox(env);
  if (!_width.isValid() && !_height.isValid()) return box;
  auto sx = 1.f, sy = 1.f;
  if (_width.isValid() && _height.isValid()) {
    sx = Units::fsize(_width, env) / box->_width;
    sy = Units::fsize(_height, env) / box->vlen();
    if (_keepAspectRatio) {
      sx = std::min(sx, sy);
      sy = sx;
    }
  } else if (_width.isValid() && !_height.isValid()) {
    sx = Units::fsize(_width, env) / box->_width;
    sy = sx;
  } else {
    sy = Units::fsize(_height, env) / box->vlen();
    sx = sy;
  }
  return sptrOf<ScaleBox>(box, sx, sy);
}

RotateAtom::RotateAtom(const sptr<Atom>& base, float angle, const string& option)
    : _angle(0), _option(Rotation::bl) {
  _base = base == nullptr ? sptrOf<EmptyAtom>() : base;
  _type = _base->_type;
  _angle = angle;
  const auto& opt = parseOption(option);
  auto it = opt.find("origin");
  if (it != opt.end()) {
    _option = RotateBox::getOrigin(it->second);
    return;
  }
  it = opt.find("x");
  if (it != opt.end()) {
    _x = Units::getDimen(it->second);
  } else {
    _x = 0._em;
  }
  it = opt.find("y");
  if (it != opt.end()) {
    _y = Units::getDimen(it->second);
  } else {
    _y = 0._em;
  }
}

RotateAtom::RotateAtom(const sptr<Atom>& base, const string& angle, const string& option)
    : _angle(0), _option(Rotation::none), _x(0._em), _y(0._em) {
  _type = base->_type;
  _base = base;
  valueOf(angle, _angle);
  _option = RotateBox::getOrigin(option);
}

sptr<Box> RotateAtom::createBox(Env& env) {
  if (_option != Rotation::none) {
    return sptrOf<RotateBox>(_base->createBox(env), _angle, _option);
  }

  const auto x = Units::fsize(_x, env);
  const auto y = Units::fsize(_y, env);
  return sptrOf<RotateBox>(_base->createBox(env), _angle, x, y);
}

sptr<Box> RuleAtom::createBox(Env& env) {
  float w = Units::fsize(_w, env);
  float h = Units::fsize(_h, env);
  float r = Units::fsize(_r, env);
  return sptrOf<RuleBox>(h, w, r);
}

sptr<Box> StrikeThroughAtom::createBox(Env& env) {
  const auto t = env.mathConsts().overbarRuleThickness() * env.scale();
  const auto h = env.mathConsts().axisHeight() * env.scale();
  auto b = _at->createBox(env);
  auto r = sptrOf<RuleBox>(t, b->_width, -h + t);
  auto hb = sptrOf<HBox>(b);
  hb->add(StrutBox::create(-b->_width));
  hb->add(r);
  return hb;
}

sptr<Box> VCenterAtom::createBox(Env& env) {
  auto b = _base->createBox(env);
  auto a = env.mathConsts().axisHeight() * env.scale();
  auto hb = sptrOf<HBox>(b);
  hb->_height = b->vlen() / 2 + a;
  hb->_depth = b->vlen() - hb->_height;
  return hb;
}

LongDivAtom::LongDivAtom(long divisor, long dividend) : _divisor(divisor), _dividend(dividend) {
  _halign = Alignment::right;
  setAlignTop(true);
  vector<string> results;
  calculate(results);

  auto rule = sptrOf<RuleAtom>(0._ex, 2.5_ex, 0.3_ex);

  const int s = results.size();
  for (int i = 0; i < s; i++) {
    auto num = Formula(results[i])._root;
    if (i == 1) {
      string divisor = toString(_divisor);
      auto rparen = SymbolAtom::get("rparen");
      auto big = sptrOf<BigSymbolAtom>(rparen, 1);
      auto raise = sptrOf<RaiseAtom>(big, -1._tt, 0._tt, 0._tt);
      auto row = sptrOf<RowAtom>(raise);
      row->add(num);
      auto o = sptrOf<OverUnderBar>(row, true);
      auto r = sptrOf<RowAtom>(Formula(divisor, false)._root);
      r->add(sptrOf<SpaceAtom>(SpaceType::thinMuSkip));
      r->add(o);
      append(r);
      continue;
    }
    if (i % 2 == 0) {
      auto row = sptrOf<RowAtom>(num);
      row->add(rule);
      if (i == 0)
        append(row);
      else
        append(sptrOf<OverUnderBar>(row, false));
    } else {
      auto row = sptrOf<RowAtom>(num);
      row->add(rule);
      append(row);
    }
  }
}

void LongDivAtom::calculate(vector<string>& results) const {
  long quotient = _dividend / _divisor;
  results.push_back(toString(quotient));
  string x = toString(quotient);
  size_t len = x.length();
  long remaining = _dividend;
  results.push_back(toString(remaining));
  for (size_t i = 0; i < len; i++) {
    long b = (x[i] - '0') * pow(10, len - i - 1);
    long product = b * _divisor;
    remaining = remaining - product;
    results.push_back(toString(product));
    results.push_back(toString(remaining));
  }
}

sptr<Box> CancelAtom::createBox(Env& env) {
  auto box = _base->createBox(env);
  vector<float> lines;
  if (_cancelType == SLASH) {
    lines = {0, 0, box->_width, box->_height + box->_depth};
  } else if (_cancelType == BACKSLASH) {
    lines = {box->_width, 0, 0, box->_height + box->_depth};
  } else if (_cancelType == CROSS) {
    lines = {
      0,
      0,
      box->_width,
      box->_height + box->_depth,
      box->_width,
      0,
      0,
      box->_height + box->_depth,
    };
  } else {
    return box;
  }

  const float rt = env.mathConsts().fractionRuleThickness() * env.scale();
  auto overlap = sptrOf<LineBox>(lines, rt);
  overlap->_width = box->_width;
  overlap->_height = box->_height;
  overlap->_depth = box->_depth;
  auto hbox = new HBox(box);
  hbox->add(sptr<Box>(new StrutBox(-box->_width, 0, 0, 0)));
  hbox->add(overlap);
  return sptr<Box>(hbox);
}
