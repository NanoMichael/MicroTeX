#ifndef MACRO_IMPL_H_INCLUDED
#define MACRO_IMPL_H_INCLUDED

#include <memory>

#include "atom/atom_basic.h"
#include "atom/atom_impl.h"
#include "atom/atom_sideset.h"
#include "core/split.h"
#include "core/formula.h"
#include "core/parser.h"
#include "macro/macro.h"
#include "macro/macro_decl.h"
#include "graphic/graphic.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

namespace tex {

inline macro(fatalIfCmdConflict) {
  NewCommandMacro::_errIfConflict = args[1] == L"true";
  return nullptr;
}

inline macro(breakEverywhere) {
  RowAtom::_breakEveywhere = args[1] == L"true";
  return nullptr;
}

inline macro(longdiv) {
  long dividend = 0;
  valueof(args[1], dividend);
  long divisor = 0;
  valueof(args[2], divisor);
  if (divisor == 0) throw ex_parse("Divisor must not be 0.");
  return sptrOf<LongDivAtom>(divisor, dividend);
}

inline macro(cellcolor) {
  if (!tp.isArrayMode()) throw ex_parse("Command \\cellcolor must used in array environment!");
  color c = ColorAtom::getColor(wide2utf8(args[1]));
  auto atom = sptrOf<CellColorAtom>(c);
  ((ArrayFormula*) tp._formula)->addCellSpecifier(atom);
  return nullptr;
}

inline macro(color) {
  // We do not care the \color command in non-array mode, since we did pass a color as a parameter
  // when parsing a LaTeX string, it is useless to specify a global foreground color again, but in
  // array mode, the \color command is useful to specify the foreground color of the columns.
  if (tp.isArrayMode()) {
    color c = ColorAtom::getColor(wide2utf8(args[1]));
    return sptrOf<CellForegroundAtom>(c);
  }
  return nullptr;
}

inline macro(newcolumntype) {
  MatrixAtom::defineColumnSpecifier(args[1], args[2]);
  return nullptr;
}

inline macro(arrayrulecolor) {
  color c = ColorAtom::getColor(wide2utf8(args[1]));
  MatrixAtom::LINE_COLOR = c;
  return nullptr;
}

inline macro(columnbg) {
  color c = ColorAtom::getColor(wide2utf8(args[1]));
  return sptrOf<CellColorAtom>(c);
}

inline macro(rowcolor) {
  if (!tp.isArrayMode()) throw ex_parse("Command \\rowcolor must used in array environment!");
  color c = ColorAtom::getColor(wide2utf8(args[1]));
  auto spe = sptrOf<CellColorAtom>(c);
  ((ArrayFormula*) tp._formula)->addRowSpecifier(spe);
  return nullptr;
}

inline macro(st) {
  auto base = Formula(tp, args[1], false)._root;
  return sptrOf<StrikeThroughAtom>(base);
}

inline macro(Braket) {
  std::wstring str(args[1]);
  replaceall(str, L"\\|", L"\\middle\\vert ");
  return Formula(tp, L"\\left\\langle " + str + L"\\right\\rangle")._root;
}

inline macro(Set) {
  std::wstring str(args[1]);
  replacefirst(str, L"\\|", L"\\middle\\vert ");
  return Formula(tp, L"\\left\\{" + str + L"\\right\\}")._root;
}

inline macro(spATbreve) {
  auto* vra = new VRowAtom(Formula(L"\\displaystyle\\!\\breve{}")._root);
  vra->setRaise(UnitType::ex, 0.6f);
  return sptrOf<SmashedAtom>(sptr<Atom>(vra), "");
}

inline macro(clrlap) {
  return sptrOf<LapedAtom>(Formula(tp, args[1])._root, args[0][0]);
}

inline macro(mathclrlap) {
  return sptrOf<LapedAtom>(Formula(tp, args[1])._root, args[0][4]);
}

inline macro(frac) {
  Formula num(tp, args[1], false);
  Formula den(tp, args[2], false);
  if (num._root == nullptr || den._root == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FractionAtom>(num._root, den._root, true);
}

inline macro(over) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FractionAtom>(num, den, true);
}

inline macro(atop) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FractionAtom>(num, den, false);
}

inline sptr<Atom> _choose(
  const std::string& left, const std::string& right,
  TeXParser& tp, std::vector<std::wstring>& args
) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of choose can't be empty!");
  auto f = sptrOf<FractionAtom>(num, den, false);
  auto l = SymbolAtom::get(left);
  auto r = SymbolAtom::get(right);
  // modify its type to opening and closing
  l->_type = AtomType::opening;
  r->_type = AtomType::closing;
  return sptrOf<FencedAtom>(f, l, r);
}

inline macro(choose) {
  return _choose("lbrack", "rbrack", tp, args);
}

inline macro(brack) {
  return _choose("lsqbrack", "rsqbrack", tp, args);
}

inline macro(bangle) {
  return _choose("langle", "rangle", tp, args);
}

inline macro(brace) {
  return _choose("lbrace", "rbrace", tp, args);
}

inline sptr<Atom> _cancel(
  int cancelType,
  TeXParser& tp, std::vector<std::wstring>& args) {
  auto base = Formula(tp, args[1], false)._root;
  if (base == nullptr)
    throw ex_parse("Cancel content must not be empty!");
  return sptrOf<CancelAtom>(base, cancelType);
}

inline macro(cancel) {
  return _cancel(CancelAtom::SLASH, tp, args);
}

inline macro(bcancel) {
  return _cancel(CancelAtom::BACKSLASH, tp, args);
}

inline macro(xcancel) {
  return _cancel(CancelAtom::CROSS, tp, args);
}

inline macro(binom) {
  Formula num(tp, args[1], false);
  Formula den(tp, args[2], false);
  if (num._root == nullptr || den._root == nullptr)
    throw ex_parse("Both binomial coefficients must be not empty!");
  auto f = sptrOf<FractionAtom>(num._root, den._root, false);
  auto l = SymbolAtom::get("lbrack");
  auto r = SymbolAtom::get("rbrack");
  // modify its type to opening and closing
  l->_type = AtomType::opening;
  r->_type = AtomType::closing;
  return sptrOf<FencedAtom>(f, l, r);
}

inline macro(above) {
  auto num = tp.popFormulaAtom();
  auto[unit, value] = tp.getLength();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");

  return sptrOf<FractionAtom>(num, den, unit, value);
}

inline macro(underscore) {
  return SymbolAtom::get("_");
}

inline macro(accents) {
  const auto name = wide2utf8(args[0]);
  const auto&[acc, fit] = [&]() {
    if (name == "widehat") return std::make_pair<std::string>("hat", true);
    if (name == "widetilde") return std::make_pair<std::string>("tilde", true);
    return std::make_pair(name, false);
  }();
  return sptrOf<AccentedAtom>(Formula(tp, args[1], false)._root, acc, fit);
}

inline macro(accentset) {
  return sptrOf<AccentedAtom>(
    Formula(tp, args[2], false)._root,
    wide2utf8(args[1]).substr(1), false, true
  );
}

inline macro(cedilla) {
  return sptrOf<CedillaAtom>(Formula(tp, args[1])._root);
}

inline macro(ogonek) {
  return sptrOf<OgonekAtom>(Formula(tp, args[1])._root);
}

inline macro(nbsp) {
  return sptrOf<SpaceAtom>();
}

inline macro(sqrt) {
  if (args[2].empty()) return sptrOf<NthRoot>(Formula(tp, args[1], false)._root, nullptr);
  return sptrOf<NthRoot>(
    Formula(tp, args[1], false)._root,
    Formula(tp, args[2], false)._root
  );
}

inline macro(overrightarrow) {
  return sptrOf<UnderOverArrowAtom>(Formula(tp, args[1], false)._root, false, true);
}

inline macro(overleftarrow) {
  return sptrOf<UnderOverArrowAtom>(Formula(tp, args[1], false)._root, true, true);
}

inline macro(overleftrightarrow) {
  return sptrOf<UnderOverArrowAtom>(Formula(tp, args[1], false)._root, true);
}

inline macro(underrightarrow) {
  return sptrOf<UnderOverArrowAtom>(Formula(tp, args[1], false)._root, false, false);
}

inline macro(underleftarrow) {
  return sptrOf<UnderOverArrowAtom>(Formula(tp, args[1], false)._root, true, false);
}

inline macro(underleftrightarrow) {
  return sptrOf<UnderOverArrowAtom>(Formula(tp, args[1], false)._root, false);
}

inline macro(xleftarrow) {
  return sptrOf<XArrowAtom>(
    Formula(tp, args[1], false)._root,
    Formula(tp, args[2])._root,
    true
  );
}

inline macro(xrightarrow) {
  return sptrOf<XArrowAtom>(
    Formula(tp, args[1], false)._root,
    Formula(tp, args[2])._root,
    false
  );
}

inline macro(sideset) {
  auto l = Formula(tp, args[1])._root;
  auto r = Formula(tp, args[2])._root;
  auto op = Formula(tp, args[3])._root;
  if (op == nullptr) {
    auto in = sptrOf<CharAtom>(L'M', "mathnormal");
    op = sptrOf<PhantomAtom>(in, false, true, true);
  }
  auto cl = dynamic_cast<CumulativeScriptsAtom*>(l.get());
  auto cr = dynamic_cast<CumulativeScriptsAtom*>(r.get());
  if (cl != nullptr) l = cl->getScriptsAtom();
  if (cr != nullptr) r = cr->getScriptsAtom();
  return sptrOf<SideSetsAtom>(op, l, r);
}

inline macro(prescript) {
  auto base = Formula(tp, args[3])._root;
  auto p = sptrOf<PhantomAtom>(base, false, true, true);
  auto s = sptrOf<ScriptsAtom>(p, Formula(tp, args[2])._root, Formula(tp, args[1])._root, false);
  tp.addAtom(s);
  tp.addAtom(sptrOf<SpaceAtom>(UnitType::mu, -0.3f, 0.f, 0.f));
  return sptrOf<TypedAtom>(AtomType::ordinary, AtomType::ordinary, base);
}

inline sptr<Atom> _overunder(
  TeXParser& tp,
  std::vector<std::wstring>& args,
  const std::string& name,
  bool isOver
) {
  return sptrOf<OverUnderDelimiter>(
    Formula(tp, args[1], false)._root,
    nullptr,
    SymbolAtom::get(name),
    UnitType::ex,
    0.f,
    isOver
  );
}

inline macro(underbrace) {
  return _overunder(tp, args, "rbrace", false);
}

inline macro(overbrace) {
  return _overunder(tp, args, "lbrace", true);
}

inline macro(underbrack) {
  return _overunder(tp, args, "rsqbrack", false);
}

inline macro(overbrack) {
  return _overunder(tp, args, "lsqbrack", true);
}

inline macro(underparen) {
  return _overunder(tp, args, "rbrack", false);
}

inline macro(overparen) {
  return _overunder(tp, args, "lbrack", true);
}

inline macro(overline) {
  return sptrOf<OverlinedAtom>(Formula(tp, args[1], false)._root);
}

inline macro(underline) {
  return sptrOf<UnderlinedAtom>(Formula(tp, args[1], false)._root);
}

inline sptr<Atom> _math_type(TeXParser& tp, Args& args, AtomType type) {
  return sptrOf<TypedAtom>(type, type, Formula(tp, args[1], false)._root);
}

inline macro(mathop) {
  auto a = _math_type(tp, args, AtomType::bigOperator);
  a->_limitsType = LimitsType::noLimits;
  return a;
}

inline macro(mathpunct) {
  return _math_type(tp, args, AtomType::punctuation);
}

inline macro(mathord) {
  return _math_type(tp, args, AtomType::ordinary);
}

inline macro(mathrel) {
  return _math_type(tp, args, AtomType::relation);
}

inline macro(mathinner) {
  return _math_type(tp, args, AtomType::inner);
}

inline macro(mathbin) {
  return _math_type(tp, args, AtomType::binaryOperator);
}

inline macro(mathopen) {
  return _math_type(tp, args, AtomType::opening);
}

inline macro(mathclose) {
  return _math_type(tp, args, AtomType::closing);
}

inline macro(joinrel) {
  return sptrOf<TypedAtom>(
    AtomType::relation,
    AtomType::relation,
    sptrOf<SpaceAtom>(UnitType::mu, -2.6f, 0.f, 0.f)
  );
}

inline macro(smash) {
  const std::string x = wide2utf8(args[2]);
  return sptrOf<SmashedAtom>(Formula(tp, args[1], false)._root, x);
}

inline macro(leftparenthesis) {
  std::wstring grp = tp.getGroup(L"\\(", L"\\)");
  return sptrOf<MathAtom>(Formula(tp, grp, false)._root, TexStyle::text);
}

inline macro(leftbracket) {
  std::wstring grp = tp.getGroup(L"\\[", L"\\]");
  return sptrOf<MathAtom>(Formula(tp, grp, false)._root, TexStyle::display);
}

inline macro(middle) {
  return sptrOf<MiddleAtom>(Formula(tp, args[1])._root);
}

inline macro(cr) {
  if (tp.isArrayMode()) {
    tp.addRow();
  } else {
    ArrayFormula arr;
    arr.add(tp._formula->_root);
    arr.addRow();
    TeXParser parser(
      tp.isPartial(),
      tp.forwardBalancedGroup(),
      &arr,
      false,
      tp.isMathMode()
    );
    parser.parse();
    arr.checkDimensions();
    tp._formula->_root = arr.getAsVRow();
  }

  return nullptr;
}

inline macro(backslashcr) {
  return macro_cr(tp, args);
}

inline macro(shoveright) {
  auto a = Formula(tp, args[1])._root;
  a->_alignment = Alignment::right;
  return a;
}

inline macro(shoveleft) {
  auto a = Formula(tp, args[1])._root;
  a->_alignment = Alignment::left;
  return a;
}

inline macro(makeatletter) {
  tp.makeAtLetter();
  return nullptr;
}

inline macro(makeatother) {
  tp.makeAtOther();
  return nullptr;
}

inline macro(newenvironment) {
  int opt = 0;
  if (!args[4].empty()) valueof(args[4], opt);

  NewEnvironmentMacro::addNewEnvironment(args[1], args[2], args[3], opt);
  return nullptr;
}

inline macro(renewenvironment) {
  int opt = 0;
  if (!args[4].empty()) valueof(args[4], opt);

  NewEnvironmentMacro::addRenewEnvironment(args[1], args[2], args[3], opt);
  return nullptr;
}

inline macro(fbox) {
  return sptrOf<FBoxAtom>(Formula(tp, args[1], false)._root);
}

// region Stacks

inline macro(stack) {
  const auto& over = StackArgs::autoSpace(Formula(tp, args[1], false)._root);
  const auto& under = StackArgs::autoSpace(Formula(tp, args[3], false)._root);
  return sptrOf<StackAtom>(Formula(tp, args[2], false)._root, over, under);
}

inline macro(stackrel) {
  const auto& stack = macro_stack(tp, args);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, stack);
}

inline macro(stackbin) {
  const auto& stack = macro_stack(tp, args);
  return sptrOf<TypedAtom>(AtomType::binaryOperator, AtomType::binaryOperator, stack);
}

inline macro(overset) {
  const auto& over = StackArgs::autoSpace(Formula(tp, args[1], false)._root);
  sptr<Atom> a = sptrOf<StackAtom>(Formula(tp, args[2], false)._root, over, true);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, a);
}

inline macro(underset) {
  const auto& under = StackArgs::autoSpace(Formula(tp, args[1], false)._root);
  sptr<Atom> a = sptrOf<StackAtom>(Formula(tp, args[2], false)._root, under, false);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, a);
}

inline macro(underaccent) {
  const StackArgs under{Formula(tp, args[1], false)._root, UnitType::mu, 1.f, true};
  return sptrOf<StackAtom>(Formula(tp, args[2], false)._root, under, false);
}

inline macro(undertilde) {
  // TODO
  auto a = Formula(tp, args[1], false)._root;
  auto p = sptrOf<PhantomAtom>(a, true, false, false);
  auto acc = sptrOf<AccentedAtom>(p, "widetilde");
  const StackArgs under{acc, UnitType::mu, 0.5f, true};
  return sptrOf<StackAtom>(a, under, false);
}

// endregion Stacks

inline macro(hphantom) {
  return sptrOf<PhantomAtom>(Formula(tp, args[1], false)._root, true, false, false);
}

inline macro(vphantom) {
  return sptrOf<PhantomAtom>(Formula(tp, args[1], false)._root, false, true, true);
}

inline macro(phantom) {
  return sptr<Atom>(
    new PhantomAtom(Formula(tp, args[1], false)._root, true, true, true));
}

inline sptr<Atom> _big(
  TeXParser& tp,
  std::vector<std::wstring>& args,
  int size,
  AtomType type = AtomType::none
) {
  auto a = Formula(tp, args[1], false)._root;
  auto s = std::dynamic_pointer_cast<SymbolAtom>(a);
  if (s == nullptr) return a;
  auto t = sptrOf<BigDelimiterAtom>(s, size);
  if (type != AtomType::none) t->_type = type;
  return t;
}

inline macro(big) { return _big(tp, args, 1); }

inline macro(Big) { return _big(tp, args, 2); }

inline macro(bigg) { return _big(tp, args, 3); }

inline macro(Bigg) { return _big(tp, args, 4); }

inline macro(bigl) { return _big(tp, args, 1, AtomType::opening); }

inline macro(Bigl) { return _big(tp, args, 2, AtomType::opening); }

inline macro(biggl) { return _big(tp, args, 3, AtomType::opening); }

inline macro(Biggl) { return _big(tp, args, 4, AtomType::opening); }

inline macro(bigr) { return _big(tp, args, 1, AtomType::closing); }

inline macro(Bigr) { return _big(tp, args, 2, AtomType::closing); }

inline macro(biggr) { return _big(tp, args, 3, AtomType::closing); }

inline macro(Biggr) { return _big(tp, args, 4, AtomType::closing); }

inline macro(rotatebox) {
  float angle = 0;
  if (!args[1].empty()) valueof(args[1], angle);
  return sptrOf<RotateAtom>(Formula(tp, args[2])._root, angle, args[3]);
}

inline macro(reflectbox) {
  return sptrOf<ReflectAtom>(Formula(tp, args[1])._root);
}

inline macro(scalebox) {
  float sx = 1, sy = 1;
  valueof(args[1], sx);

  if (args[3].empty()) sy = sx;
  else valueof(args[3], sy);

  if (sx == 0) sx = 1;
  if (sy == 0) sy = 1;
  return sptrOf<ScaleAtom>(Formula(tp, args[2])._root, sx, sy);
}

inline macro(resizebox) {
  const std::string ws = wide2utf8(args[1]);
  const std::string hs = wide2utf8(args[2]);
  return sptrOf<ResizeAtom>(Formula(tp, args[3])._root, ws, hs, ws == "!" || hs == "!");
}

inline macro(shadowbox) {
  return sptrOf<ShadowAtom>(Formula(tp, args[1])._root);
}

inline macro(ovalbox) {
  return sptrOf<OvalAtom>(Formula(tp, args[1])._root);
}

inline macro(cornersize) {
  float size = 0.5f;
  valueof(args[1], size);
  if (size <= 0 || size > 0.5f) size = 0.5f;
  OvalAtom::_multiplier = size;
  OvalAtom::_diameter = 0;
  return nullptr;
}

inline macro(doublebox) {
  return sptrOf<DoubleFramedAtom>(Formula(tp, args[1])._root);
}

inline macro(fgcolor) {
  auto a = Formula(tp, args[2])._root;
  std::string x = wide2utf8(args[1]);
  return sptrOf<ColorAtom>(a, TRANSPARENT, ColorAtom::getColor(x));
}

inline macro(bgcolor) {
  auto a = Formula(tp, args[2])._root;
  std::string x = wide2utf8(args[1]);
  return sptrOf<ColorAtom>(a, ColorAtom::getColor(x), TRANSPARENT);
}

inline macro(textcolor) {
  auto a = Formula(tp, args[2], false, false)._root;
  std::string x = wide2utf8(args[1]);
  return sptrOf<ColorAtom>(a, TRANSPARENT, ColorAtom::getColor(x));
}

inline macro(colorbox) {
  std::string x = wide2utf8(args[1]);
  color c = ColorAtom::getColor(x);
  return sptrOf<FBoxAtom>(Formula(tp, args[2])._root, c, c);
}

inline macro(fcolorbox) {
  std::string x = wide2utf8(args[2]);
  color f = ColorAtom::getColor(x);
  std::string y = wide2utf8(args[1]);
  color b = ColorAtom::getColor(y);
  return sptrOf<FBoxAtom>(Formula(tp, args[3])._root, f, b);
}

inline macro(externalfont) {
  std::string x = wide2utf8(args[1]);
  TextRenderingBox::setFont(x);
  return nullptr;
}

inline macro(ctext) {
  // TODO text
  return sptrOf<EmptyAtom>();
}

inline macro(declaremathsizes) {
  float a, b, c, d;
  valueof(args[1], a), valueof(args[2], b), valueof(args[3], c), valueof(args[4], d);
  // DefaultTeXFont::setMathSizes(a, b, c, c);
  return nullptr;
}

inline macro(magnification) {
  float x;
  valueof(args[1], x);
  // DefaultTeXFont::setMagnification(x);
  return nullptr;
}

inline macro(mathcumsup) {
  return sptrOf<CumulativeScriptsAtom>(
    tp.popLastAtom(),
    nullptr,
    Formula(tp, args[1])._root
  );
}

inline macro(mathcumsub) {
  return sptrOf<CumulativeScriptsAtom>(
    tp.popLastAtom(),
    Formula(tp, args[1])._root,
    nullptr
  );
}

inline sptr<Atom> _underover(
  const std::string& base,
  const std::string& script,
  float space
) {
  const StackArgs under{
    SymbolAtom::get(script),
    UnitType::mu,
    space,
    false
  };
  return sptrOf<StackAtom>(SymbolAtom::get(base), under, true);
}

inline sptr<Atom> _colon() {
  return _underover("normaldot", "normaldot", 5.2f);
}

inline macro(ratio) {
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, _colon());
}

inline macro(minuscolon) {
  auto* ra = new RowAtom(SymbolAtom::get("minus"));
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.095f, 0.f, 0.f));
  ra->add(_colon());
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(minuscoloncolon) {
  auto* ra = new RowAtom(SymbolAtom::get("minus"));
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.095f, 0.f, 0.f));
  sptr<Atom> colon = _colon();
  ra->add(colon);
  ra->add(colon);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(simcolon) {
  auto* ra = new RowAtom(SymbolAtom::get("sim"));
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.095f, 0.f, 0.f));
  ra->add(_colon());
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(simcoloncolon) {
  auto* ra = new RowAtom(SymbolAtom::get("sim"));
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.095f, 0.f, 0.f));
  sptr<Atom> colon = _colon();
  ra->add(colon);
  ra->add(colon);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(approxcolon) {
  auto* ra = new RowAtom(SymbolAtom::get("approx"));
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.095f, 0.f, 0.f));
  ra->add(_colon());
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(approxcoloncolon) {
  auto* ra = new RowAtom(SymbolAtom::get("approx"));
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.095f, 0.f, 0.f));
  sptr<Atom> colon = _colon();
  ra->add(colon);
  ra->add(colon);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(equalscolon) {
  auto* ra = new RowAtom(SymbolAtom::get("equals"));
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.095f, 0.f, 0.f));
  ra->add(_colon());
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(equalscoloncolon) {
  auto* ra = new RowAtom(SymbolAtom::get("equals"));
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.095f, 0.f, 0.f));
  sptr<Atom> colon = _colon();
  ra->add(colon);
  ra->add(colon);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(colonminus) {
  auto* ra = new RowAtom(_colon());
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.32f, 0.f, 0.f));
  ra->add(SymbolAtom::get("minus"));
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(coloncolonminus) {
  sptr<Atom> u = _colon();
  auto* ra = new RowAtom(u);
  ra->add(u);
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.32f, 0.f, 0.f));
  ra->add(SymbolAtom::get("minus"));
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(colonequals) {
  auto* ra = new RowAtom(_colon());
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.32f, 0.f, 0.f));
  ra->add(SymbolAtom::get("equals"));
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(coloncolonequals) {
  sptr<Atom> u = _colon();
  auto* ra = new RowAtom(u);
  ra->add(u);
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.32f, 0.f, 0.f));
  ra->add(SymbolAtom::get("equals"));
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(coloncolon) {
  sptr<Atom> u = _colon();
  auto* ra = new RowAtom(u);
  ra->add(u);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(colonsim) {
  auto* ra = new RowAtom(_colon());
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.32f, 0.f, 0.f));
  ra->add(SymbolAtom::get("sim"));
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(coloncolonsim) {
  sptr<Atom> u = _colon();
  auto* ra = new RowAtom(u);
  ra->add(u);
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.32f, 0.f, 0.f));
  ra->add(SymbolAtom::get("sim"));
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(colonapprox) {
  auto* ra = new RowAtom(_colon());
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.32f, 0.f, 0.f));
  ra->add(SymbolAtom::get("approx"));
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(coloncolonapprox) {
  sptr<Atom> u = _colon();
  auto* ra = new RowAtom(u);
  ra->add(u);
  ra->add(sptrOf<SpaceAtom>(UnitType::em, -0.32f, 0.f, 0.f));
  ra->add(SymbolAtom::get("approx"));
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(ra));
}

inline macro(char) {
  std::string x = wide2utf8(args[1]);
  int radix = 10;
  if (startswith(x, "0x") || startswith(x, "0X")) {
    x = x.substr(2);
    radix = 16;
  } else if (startswith(x, "x") || startswith(x, "X")) {
    x = x.substr(1);
    radix = 16;
  } else if (startswith(x, "0")) {
    x = x.substr(1);
    radix = 8;
  }
  int n = 0;
  str2int(x, n, radix);
  return tp.convertCharacter((wchar_t) n);
}

inline macro(T) {
  return sptrOf<RotateAtom>(Formula(tp, args[1])._root, 180.f, L"origin=cc");
}

inline macro(textcircled) {
  // TODO
  // return sptrOf<TextCircledAtom>(sptrOf<RomanAtom>(Formula(tp, args[1])._root));
}

inline macro(textsc) {
  return sptrOf<SmallCapAtom>(Formula(tp, args[1], false)._root);
}

inline macro(sc) {
  return sptrOf<SmallCapAtom>(Formula(tp, tp.getOverArgument(), false, tp.isMathMode())._root);
}

inline macro(quad) {
  return sptrOf<SpaceAtom>(UnitType::em, 1.f, 0.f, 0.f);
}

inline macro(surd) {
  return sptrOf<VCenteredAtom>(SymbolAtom::get("surdsign"));
}

inline macro(lmoustache) {
  auto* s = new SymbolAtom(*(SymbolAtom::get("lmoustache")));
  auto b = sptrOf<BigDelimiterAtom>(sptr<SymbolAtom>(s), 1);
  b->_type = AtomType::opening;
  return b;
}

inline macro(rmoustache) {
  auto* s = new SymbolAtom(*(SymbolAtom::get("rmoustache")));
  auto b = sptrOf<BigDelimiterAtom>(sptr<SymbolAtom>(s), 1);
  b->_type = AtomType::closing;
  return b;
}

inline macro(insertBreakMark) {
  return sptrOf<BreakMarkAtom>();
}

/**************************************** limits macros *******************************************/

inline sptr<Atom> _limits_type(TeXParser& tp, Args& args, LimitsType type) {
  auto atom = tp.popLastAtom();
  auto copy = atom->clone();
  copy->_limitsType = type;
  return copy;
}

inline macro(nolimits) {
  return _limits_type(tp, args, LimitsType::noLimits);
}

inline macro(limits) {
  return _limits_type(tp, args, LimitsType::limits);
}

inline macro(normal) {
  return _limits_type(tp, args, LimitsType::normal);
}

/***************************************** implement at .cpp **************************************/

macro(kern);

macro(hvspace);

macro(rule);

macro(cfrac);

macro(sfrac);

macro(genfrac);

macro(overwithdelims);

macro(atopwithdelims);

macro(abovewithdelims);

macro(accentbiss);

macro(left);

macro(intertext);

macro(newcommand);

macro(renewcommand);

macro(raisebox);

macro(definecolor);

macro(sizes);

macro(romannumeral);

macro(muskips);

macro(setmathfont);

macro(zstack);

#ifdef GRAPHICS_DEBUG

macro(debug);

macro(undebug);

#endif  // GRAPHICS_DEBUG

/**************************************** not implemented *****************************************/

inline macro(includegraphics) {
  return nullptr;
}

inline macro(fcscore) {
  return nullptr;
}

inline macro(dynamic) {
  return nullptr;
}

}  // namespace tex

#endif  // MACRO_IMPL_H_INCLUDED
