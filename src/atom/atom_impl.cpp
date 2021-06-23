#include "atom/atom_impl.h"

#include <memory>

using namespace std;
using namespace tex;

/**************************************** small atoms *********************************************/

const float FBoxAtom::INTERSPACE = 0.65f;
float OvalAtom::_multiplier = 0.5f;
float OvalAtom::_diameter = 0.f;

const int FencedAtom::DELIMITER_FACTOR = 901;
const float FencedAtom::DELIMITER_SHORTFALL = 5.f;

void FencedAtom::init(const sptr<Atom>& b, const sptr<SymbolAtom>& l, const sptr<SymbolAtom>& r) {
  _base = b == nullptr ? sptrOf<RowAtom>() : b;
  if (l == nullptr || l->name() != "normaldot") _left = l;
  if (r == nullptr || r->name() != "normaldot") _right = r;
}

void FencedAtom::center(Box& b, float axis) {
  float h = b._height, total = h + b._depth;
  b._shift = -(total / 2 - h) - axis;
}

sptr<Box> FencedAtom::createBox(Env& env) {
  return StrutBox::empty();
}

/****************************************** fraction atom *****************************************/

void FractionAtom::init(const sptr<Atom>& num, const sptr<Atom>& den, bool nodef, UnitType unit, float t) {
  _numAlign = Alignment::center;
  _denomAlign = Alignment::center;
  _deffactor = 1.f;

  _numerator = num;
  _denominator = den;
  _nodefault = nodef;
  _thickness = t;
  _unit = unit;
  _type = AtomType::ordinary;
  _useKern = true;
  _deffactorset = false;
}

sptr<Box> FractionAtom::createBox(Env& env) {
  return StrutBox::empty();
}

const string NthRoot::_sqrtSymbol = "sqrt";
const float NthRoot::FACTOR = 0.55f;

sptr<Box> NthRoot::createBox(Env& env) {
  return StrutBox::empty();
}

RotateAtom::RotateAtom(const sptr<Atom>& base, float angle, const wstring& option)
  : _angle(0), _option(Rotation::bl), _xunit(UnitType::em), _yunit(UnitType::em), _x(0), _y(0) {
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
    auto[u, x] = Units::getLength(it->second);
    _xunit = u, _x = x;
  } else {
    _xunit = UnitType::point, _x = 0;
  }
  it = opt.find("y");
  if (it != opt.end()) {
    auto[u, y] = Units::getLength(it->second);
    _yunit = u, _y = y;
  } else {
    _yunit = UnitType::point, _y = 0;
  }
}

RotateAtom::RotateAtom(const sptr<Atom>& base, const wstring& angle, const wstring& option)
  : _angle(0), _option(Rotation::none), _xunit(UnitType::em), _yunit(UnitType::em), _x(0), _y(0) {
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

  const auto x = Units::fsize(_xunit, _x, env);
  const auto y = Units::fsize(_yunit, _y, env);
  return sptrOf<RotateBox>(_base->createBox(env), _angle, x, y);
}

sptr<Box> UnderOverArrowAtom::createBox(Env& env) {
  return StrutBox::empty();
}

sptr<Box> XArrowAtom::createBox(Env& env) {
  return StrutBox::empty();
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
  setVtop(true);
  vector<wstring> results;
  calculate(results);

  auto rule = sptrOf<RuleAtom>(UnitType::ex, 0.f, UnitType::ex, 2.6f, UnitType::ex, 0.5f);

  const int s = results.size();
  for (int i = 0; i < s; i++) {
    auto num = Formula(results[i])._root;
    if (i == 1) {
      wstring divisor = towstring(_divisor);
      auto rparen = SymbolAtom::get("rparen");
      auto big = sptrOf<BigDelimiterAtom>(rparen, 1);
      auto ph = sptrOf<PhantomAtom>(big, false, true, true);
      auto ra = sptrOf<RowAtom>(ph);
      auto raised = sptrOf<RaiseAtom>(
        big,
        UnitType::x8,
        3.5f,
        UnitType::x8,
        0.f,
        UnitType::x8,
        0.f
      );
      ra->add(sptrOf<SmashedAtom>(raised));
      ra->add(num);
      auto oa = sptrOf<OverlinedAtom>(ra);
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
      else append(sptrOf<UnderlinedAtom>(row));
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
