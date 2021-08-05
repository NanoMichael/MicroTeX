#include "atom/atom_frac.h"
#include "env/env.h"
#include "env/units.h"
#include "box/box_group.h"
#include "box/box_single.h"

using namespace tex;

FractionAtom::FractionAtom(
  const sptr<Atom>& num, const sptr<Atom>& den, bool rule,
  UnitType unit, float thickness,
  Alignment numAlign, Alignment denAlign
) : _num(num), _dnom(den), _rule(rule),
    _unit(unit), _thickness(thickness) {
  _numAlign = checkAlign(numAlign);
  _denomAlign = checkAlign(denAlign);
  _type = AtomType::inner;
}

sptr<Box> FractionAtom::createBox(Env& env) {
  const auto& math = env.mathConsts();

  const auto theta = [&]() {
    if (!_rule) return 0.f;
    if (_thickness == 0.f) return math.fractionRuleThickness() * env.scale();
    return Units::fsize(_unit, _thickness, env);
  }();

  const auto x = env.withStyle(
    env.numStyle(),
    [&](Env& style) { return _num->createBox(style); }
  );
  const auto z = env.withStyle(
    env.dnomStyle(),
    [&](Env& style) { return _dnom->createBox(style); }
  );
  const auto w = std::max(x->_width, z->_width);

  const auto isDisplay = env.style() < TexStyle::text;

  auto u = 0.f, v = 0.f;
  if (isDisplay) {
    u = (
      _rule
      ? math.fractionNumeratorDisplayStyleShiftUp()
      : math.stackTopDisplayStyleShiftUp()
    );
    v = (
      _rule
      ? math.fractionDenominatorDisplayStyleShiftDown()
      : math.stackBottomDisplayStyleShiftDown()
    );
  } else {
    u = _rule ? math.fractionNumeratorShiftUp() : math.stackTopShiftUp();
    v = _rule ? math.fractionDenominatorShiftDown() : math.stackBottomShiftDown();
  }

  if (!_rule) {
    const auto phi = isDisplay ? math.stackDisplayStyleGapMin() : math.stackGapMin();
    const auto psi = (u - x->_depth) - (z->_depth - v);
    if (psi < phi) {
      u += (phi - psi) / 2;
      v += (phi - psi) / 2;
    }
  } else {
    const auto phi = (
      isDisplay
      ? math.fractionNumeratorDisplayStyleGapMin()
      : math.fractionNumeratorGapMin()
    );
    const auto a = math.axisHeight() * env.scale();
    const auto d = (u - x->_depth) - (a + theta / 2);
    if (d < phi) {
      u += phi - d;
    }
    const auto f = (a - theta / 2) - (z->_height - v);
    if (f < phi) {
      v += phi - f;
    }
  }

  const auto shift = [&](const sptr<Box>& b, Alignment align) {
    b->_shift = align == Alignment::center ? (w - b->_width) / 2 : w - b->_width;
  };
  const auto kern = (x->_height + u + z->_depth + v) - (x->vlen() + z->vlen());
  const auto vbox = sptrOf<VBox>();
  shift(x, _numAlign);
  vbox->add(x);
  if (_rule) {
    const auto k = sptrOf<StrutBox>(0.f, (kern - theta) / 2, 0.f, 0.f);
    vbox->add(k);
    vbox->add(sptrOf<RuleBox>(theta, w, 0));
    vbox->add(k);
  } else {
    vbox->add(sptrOf<StrutBox>(0.f, kern, 0.f, 0.f));
  }
  shift(z, _denomAlign);
  vbox->add(z);
  vbox->_height = x->_height + u;
  vbox->_depth = z->_depth + v;

  return vbox;
}
