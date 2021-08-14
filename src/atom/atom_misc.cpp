#include "atom/atom_misc.h"
#include "atom/atom_delim.h"
#include "atom/atom_basic.h"
#include "utils/utf.h"
#include "utils/string_utils.h"
#include "env/units.h"

using namespace std;
using namespace tex;

sptr<Box> BigSymbolAtom::createBox(Env& env) {
  auto b = tex::createVDelim(_delim, env, _size);
  const auto axis = env.mathConsts().axisHeight() * env.scale();
  b->_shift = -(b->vlen() / 2 - b->_height) - axis;
  return sptrOf<HBox>(b);
}

sptr<Box> LapedAtom::createBox(Env& env) {
  auto b = _at->createBox(env);
  auto* vb = new VBox();
  vb->add(b);
  vb->_width = 0;
  switch (_type) {
    case 'l':
      b->_shift = -b->_width;
      break;
    case 'r':
      b->_shift = 0;
      break;
    default:
      b->_shift = -b->_width / 2;
      break;
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

RotateAtom::RotateAtom(const sptr<Atom>& base, float angle, const wstring& option)
  : _angle(0), _option(Rotation::bl) {
  _type = base->_type;
  _base = base;
  _angle = angle;
  const string x = wide2utf8(option);
  const auto& opt = parseOption(x);
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

RotateAtom::RotateAtom(const sptr<Atom>& base, const wstring& angle, const wstring& option)
  : _angle(0), _option(Rotation::none), _x(0._em), _y(0._em) {
  _type = base->_type;
  _base = base;
  valueof(angle, _angle);
  const string x = wide2utf8(option);
  _option = RotateBox::getOrigin(x);
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

sptr<Box> VCenteredAtom::createBox(Env& env) {
  auto b = _base->createBox(env);
  auto a = env.mathConsts().axisHeight() * env.scale();
  auto hb = sptrOf<HBox>(b);
  hb->_height = b->vlen() / 2 + a;
  hb->_depth = b->vlen() - hb->_height;
  return hb;
}

LongDivAtom::LongDivAtom(long divisor, long dividend)
  : _divisor(divisor), _dividend(dividend) {}

void LongDivAtom::calculate(vector<wstring>& results) const {
  long quotient = _dividend / _divisor;
  results.push_back(towstring(quotient));
  string x = tostring(quotient);
  size_t len = x.length();
  long remaining = _dividend;
  results.push_back(towstring(remaining));
  for (size_t i = 0; i < len; i++) {
    long b = (x[i] - '0') * pow(10, len - i - 1);
    long product = b * _divisor;
    remaining = remaining - product;
    results.push_back(towstring(product));
    results.push_back(towstring(remaining));
  }
}

sptr<Box> LongDivAtom::createBox(Env& env) {
  VRowAtom vrow;
  vrow._halign = Alignment::right;
  vrow.setAlignTop(true);

  vector<wstring> results;
  calculate(results);

  auto kern = sptrOf<SpaceAtom>(UnitType::ex, 0.f, 2.f, 0.4f);

  const int s = results.size();
  for (int i = 1; i < s; i++) {
    auto num = Formula(results[i])._root;
    auto row = sptrOf<RowAtom>(num);
    if (i == 1) {
      row->add(sptrOf<SpaceAtom>(UnitType::ex, 0.f, 0.f, 0.4f));
      vrow.append(row);
      continue;
    }
    row->add(kern);
    if (i % 2 == 0) {
      vrow.append(sptrOf<OverUnderBar>(row, false));
    } else {
      vrow.append(row);
    }
  }

  const auto scale = 1.2f;

  auto hb = sptrOf<HBox>();
  auto b = vrow.createBox(env);
  const wstring& n = towstring(_divisor);
  hb->add(Formula(n, false)._root->createBox(env));
  hb->add(SpaceAtom(SpaceType::thinMuSkip).createBox(env));
  auto x = ScaleAtom(SymbolAtom::get("longdivision"), scale).createBox(env);
  hb->add(x);
  hb->add(b);

  auto row = sptrOf<RowAtom>(Formula(results[0])._root);
  row->add(kern);
  auto d = row->createBox(env);

  auto vb = sptrOf<VBox>();
  d->_shift = hb->_width - d->_width;
  vb->add(d);
  const auto t = env.mathConsts().overbarRuleThickness() * env.scale() * scale;
  auto r = sptrOf<RuleBox>(t, b->_width, 0.f);
  r->_shift = hb->_width - r->_width;
  vb->add(r);
  vb->add(sptrOf<StrutBox>(0.f, -t - 1.f, 0.f, 0.f));
  vb->add(hb);

  return vb;
}

sptr<Box> CancelAtom::createBox(Env& env) {
  auto box = _base->createBox(env);
  vector<float> lines;
  if (_cancelType == SLASH) {
    lines = {0, 0, box->_width, box->_height + box->_depth};
  } else if (_cancelType == BACKSLASH) {
    lines = {box->_width, 0, 0, box->_height + box->_depth};
  } else if (_cancelType == CROSS) {
    lines = {0, 0, box->_width, box->_height + box->_depth, box->_width, 0, 0, box->_height + box->_depth};
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
