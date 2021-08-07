#include "atom/atom_misc.h"
#include "atom/atom_delim.h"
#include "utils/utf.h"
#include "utils/string_utils.h"
#include "env/units.h"

using namespace std;
using namespace tex;

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

sptr<Box> BigSymbolAtom::createBox(Env& env) {
  auto b = tex::createVDelim(_delim, env, _size);
  const auto axis = env.mathConsts().axisHeight() * env.scale();
  b->_shift = -(b->vlen() / 2 - b->_height) - axis;
  return b;
}

float OvalAtom::_multiplier = 0.5f;
float OvalAtom::_diameter = 0.f;

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

LongDivAtom::LongDivAtom(long divisor, long dividend)
  : _divisor(divisor), _dividend(dividend) {
  _halign = Alignment::right;
  setAlignTop(true);
  vector<wstring> results;
  calculate(results);

  auto rule = sptrOf<RuleAtom>(0._ex, 2.6_ex, 0.5_ex);

  const int s = results.size();
  for (int i = 0; i < s; i++) {
    auto num = Formula(results[i])._root;
    if (i == 1) {
      wstring divisor = towstring(_divisor);
      auto rparen = SymbolAtom::get("longdivision");
      auto big = sptrOf<BigSymbolAtom>(rparen, 1);
      auto ph = sptrOf<PhantomAtom>(big, false, true, true);
      auto ra = sptrOf<RowAtom>(ph);
      auto raised = sptrOf<RaiseAtom>(big, 3._x8, 0._x8, 0._x8);
      ra->add(sptrOf<SmashedAtom>(raised));
      ra->add(num);
      auto oa = sptrOf<OverUnderBar>(ra, true);
      auto row = sptrOf<RowAtom>(Formula(divisor)._root);
      row->add(sptrOf<SpaceAtom>(SpaceType::thinMuSkip));
      row->add(oa);
      append(row);
      continue;
    }
    if (i % 2 == 0) {
      auto row = sptrOf<RowAtom>(num);
      row->add(rule);
      if (i == 0) append(row);
      else append(sptrOf<OverUnderBar>(row, true));
    } else {
      auto row = sptrOf<RowAtom>(num);
      row->add(rule);
      append(row);
    }
  }
}

sptr<Box> CancelAtom::createBox(Env& env) {
  return StrutBox::empty();
}
