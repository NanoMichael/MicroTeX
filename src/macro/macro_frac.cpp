#include "macro/macro_frac.h"
#include "utils/utf.h"

using namespace std;

namespace tex {

macro(binom) {
  Formula num(tp, args[1], false);
  Formula den(tp, args[2], false);
  if (num._root == nullptr || den._root == nullptr)
    throw ex_parse("Both binomial coefficients must be not empty!");
  auto f = sptrOf<FracAtom>(num._root, den._root, false);
  return sptrOf<FencedAtom>(f, "lparen", "rparen");
}

sptr<Atom> _choose(
  const std::string& left, const std::string& right,
  TeXParser& tp, std::vector<std::wstring>& args
) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of choose can't be empty!");
  auto f = sptrOf<FracAtom>(num, den, false);
  return sptrOf<FencedAtom>(f, left, right);
}

macro(frac) {
  Formula num(tp, args[1], false);
  Formula den(tp, args[2], false);
  if (num._root == nullptr || den._root == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FracAtom>(num._root, den._root, true);
}

macro(above) {
  auto num = tp.popFormulaAtom();
  const auto& thick = tp.getDimen();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr) {
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  }
  return sptrOf<FracAtom>(num, den, true, thick);
}

macro(atop) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FracAtom>(num, den, false);
}

macro(over) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FracAtom>(num, den, true);
}

sptr<Atom> _frac_with_delims(TeXParser& tp, Args& args, bool rule, bool hasLength) {
  auto num = tp.popFormulaAtom();
  const auto& l = hasLength ? tp.getDimen() : Dimen();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;

  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");

  auto f = (
    hasLength
    ? sptrOf<FracAtom>(num, den, rule, l)
    : sptrOf<FracAtom>(num, den, rule)
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
  Alignment numAlign = Alignment::center;
  if (args[3] == L"r") {
    numAlign = Alignment::right;
  } else if (args[3] == L"l") {
    numAlign = Alignment::left;
  }
  Formula num(tp, args[1], false);
  Formula denom(tp, args[2], false);
  if (num._root == nullptr || denom._root == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  const auto n = sptrOf<StyleAtom>(TexStyle::display, num._root);
  const auto d = sptrOf<StyleAtom>(TexStyle::display, denom._root);
  return sptrOf<FracAtom>(n, d, numAlign, Alignment::center);
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
