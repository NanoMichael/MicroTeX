#include "macro/macro_frac.h"
#include "atom/atom_impl.h"
#include "utils/utf.h"

using namespace std;

namespace tex {

macro(above) {
  auto num = tp.popFormulaAtom();
  auto[unit, value] = tp.getLength();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr) {
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  }
  return sptrOf<FractionAtom>(num, den, true, unit, value);
}

macro(atop) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FractionAtom>(num, den, false);
}

macro(over) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FractionAtom>(num, den, true);
}

sptr<Atom> _frac_with_delims(TeXParser& tp, Args& args, bool rule, bool hasLength) {
  auto num = tp.popFormulaAtom();
  std::pair<UnitType, float> l;
  if (hasLength) l = tp.getLength();
  auto[unit, value] = l;
  auto den = Formula(tp, tp.getOverArgument(), false)._root;

  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");

  auto f = (
    hasLength
    ? sptrOf<FractionAtom>(num, den, rule, unit, value)
    : sptrOf<FractionAtom>(num, den, rule)
  );
  return sptrOf<FencedAtom>(f, wide2utf8(args[1]), wide2utf8(args[2]));
}

macro(overwithdelims) {
  return _frac_with_delims(tp, args, true, false);
}

macro(atopwithdelims) {
  return _frac_with_delims(tp, args, false, false);
}

macro(abovewithdelims) {
  return _frac_with_delims(tp, args, true, true);
}

macro(cfrac) {
//  Alignment numAlign = Alignment::center;
//  if (args[3] == L"r") {
//    numAlign = Alignment::right;
//  } else if (args[3] == L"l") {
//    numAlign = Alignment::left;
//  }
//  Formula num(tp, args[1], false);
//  Formula denom(tp, args[2], false);
//  if (num._root == nullptr || denom._root == nullptr)
//    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
//  auto f = sptrOf<FractionAtom>(num._root, denom._root, true, numAlign, Alignment::center);
//  f->_useKern = false;
//  f->_type = AtomType::inner;
//  auto* r = new RowAtom();
//  r->add(sptrOf<StyleAtom>(TexStyle::display, f));
//  return sptr<Atom>(r);
}

macro(sfrac) {
//  Formula num(tp, args[1], false);
//  Formula den(tp, args[2], false);
//  if (num._root == nullptr || den._root == nullptr)
//    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
//
//  float sx = 0.75f, sy = 0.75f, r = 0.45f, sL = -0.13f, sR = -0.065f;
//  sptr<Atom> slash = SymbolAtom::get("slash");
//
//  if (!tp.isMathMode()) {
//    sx = 0.6f;
//    sy = 0.5f;
//    r = 0.75f;
//    sL = -0.24f;
//    sR = -0.24f;
//    auto in = sptrOf<ScaleAtom>(SymbolAtom::get("textfractionsolidus"), 1.25f, 0.65f);
//    auto* vr = new VRowAtom(in);
//    vr->setRaise(UnitType::ex, 0.4f);
//    slash = sptr<Atom>(vr);
//  }
//
//  auto* snum = new VRowAtom(sptrOf<ScaleAtom>(num._root, sx, sy));
//  snum->setRaise(UnitType::ex, r);
//  auto* ra = new RowAtom(sptr<Atom>(snum));
//  ra->add(sptrOf<SpaceAtom>(UnitType::em, sL, 0.f, 0.f));
//  ra->add(slash);
//  ra->add(sptrOf<SpaceAtom>(UnitType::em, sR, 0.f, 0.f));
//  ra->add(sptrOf<ScaleAtom>(den._root, sx, sy));
//
//  return sptr<Atom>(ra);
}

macro(genfrac) {
//  sptr<SymbolAtom> L, R;
//
//  Formula left(tp, args[1], false);
//  L = dynamic_pointer_cast<SymbolAtom>(left._root);
//
//  Formula right(tp, args[2], false);
//  R = dynamic_pointer_cast<SymbolAtom>(right._root);
//
//  bool rule = true;
//  auto[unit, value] = Units::getLength(args[3]);
//  if (args[3].empty()) {
//    unit = UnitType::em;
//    value = 0.f;
//    rule = false;
//  }
//
//  int style = 0;
//  if (!args[4].empty()) valueof(args[4], style);
//
//  Formula num(tp, args[5], false);
//  Formula den(tp, args[6], false);
//  if (num._root == nullptr || den._root == nullptr) {
//    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
//  }
//  auto fa = sptrOf<FractionAtom>(num._root, den._root, rule, unit, value);
//  auto* ra = new RowAtom();
//  const auto texStyle = static_cast<TexStyle>(style * 2);
//  ra->add(sptrOf<StyleAtom>(texStyle, sptrOf<FencedAtom>(fa, L, R)));
//
//  return sptr<Atom>(ra);
}

}
