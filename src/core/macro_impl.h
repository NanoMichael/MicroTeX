#ifndef MACRO_IMPL_H_INCLUDED
#define MACRO_IMPL_H_INCLUDED

#include <memory>

#include "atom/atom_basic.h"
#include "atom/atom_impl.h"
#include "common.h"
#include "core/core.h"
#include "core/formula.h"
#include "core/macro.h"
#include "core/parser.h"
#include "fonts/alphabet.h"
#include "graphic/graphic.h"

namespace tex {

#ifndef macro
#define macro(name) sptr<Atom> macro_##name(TeXParser& tp, Args& args)
#endif

#ifdef GRAPHICS_DEBUG

inline macro(debug) {
  Formula::setDEBUG(true);
  return nullptr;
}

inline macro(undebug) {
  Formula::setDEBUG(false);
  return nullptr;
}

#endif  // GRAPHICS_DEBUG

inline macro(fatalIfCmdConflict) {
  NewCommandMacro::_errIfConflict = args[1] == L"true";
  return nullptr;
}

inline macro(breakEverywhere) {
  RowAtom::_breakEveywhere = args[1] == L"true";
  return nullptr;
}

inline macro(multirow) {
  if (!tp.isArrayMode()) throw ex_parse("Command \\multirow must used in array environment!");
  int n = 0;
  valueof(args[1], n);
  tp.addAtom(sptrOf<MultiRowAtom>(n, args[2], Formula(tp, args[3])._root));
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

inline macro(spAThat) {
  auto* vra = new VRowAtom(Formula(L"\\displaystyle\\widehat{}")._root);
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
  auto l = sptrOf<SymbolAtom>(left, AtomType::opening, true);
  auto r = sptrOf<SymbolAtom>(right, AtomType::closing, true);
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
  sptr<SymbolAtom> l(new SymbolAtom("lbrack", AtomType::opening, true));
  sptr<SymbolAtom> r(new SymbolAtom("rbrack", AtomType::closing, true));
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

inline macro(mbox) {
  auto group = sptrOf<RomanAtom>(Formula(tp, args[1], "mathnormal", false, false)._root);
  return sptrOf<StyleAtom>(TexStyle::text, group);
}

inline macro(text) {
  return sptrOf<RomanAtom>(Formula(tp, args[1], "mathnormal", false, false)._root);
}

inline macro(underscore) {
  return sptrOf<UnderScoreAtom>();
}

inline macro(accents) {
  const std::string x = wide2utf8(args[0]);
  return sptrOf<AccentedAtom>(Formula(tp, args[1], false)._root, x);
}

inline macro(grkaccent) {
  return sptrOf<AccentedAtom>(
    Formula(tp, args[2], false)._root,
    Formula(tp, args[1], false)._root,
    false
  );
}

inline macro(accent) {
  return sptrOf<AccentedAtom>(
    Formula(tp, args[2], false)._root,
    Formula(tp, args[1], false)._root
  );
}

inline macro(cedilla) {
  return sptrOf<CedillaAtom>(Formula(tp, args[1])._root);
}

inline macro(IJ) {
  return sptrOf<IJAtom>(args[0][0] == 'I');
}

inline macro(TStroke) {
  return sptrOf<TStrokeAtom>(args[0][0] == 'T');
}

inline macro(LCaron) {
  return sptrOf<LCaronAtom>(args[0][0] == 'L');
}

inline macro(tcaron) {
  return sptrOf<TCaronAtom>();
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

inline macro(vdots) {
  return sptrOf<VdotsAtom>();
}

inline macro(ddots) {
  return sptrOf<TypedAtom>(AtomType::inner, AtomType::inner, sptrOf<DdtosAtom>());
}

inline macro(iddots) {
  return sptrOf<TypedAtom>(AtomType::inner, AtomType::inner, sptrOf<IddotsAtom>());
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

inline macro(smallmatrixATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::smallMatrix);
}

inline macro(matrixATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::matrix);
}

inline macro(multicolumn) {
  int n = 0;
  valueof(args[1], n);
  const std::string x = wide2utf8(args[2]);
  tp.addAtom(sptrOf<MulticolumnAtom>(n, x, Formula(tp, args[3])._root));
  ((ArrayFormula*) tp._formula)->addCol(n);
  return nullptr;
}

inline macro(hdotsfor) {
  if (!tp.isArrayMode())
    throw ex_parse("Command 'hdotsfor' only available in array mode!");
  int n = 0;
  valueof(args[1], n);
  float f = 1.f;
  if (!args[2].empty()) valueof(args[2], f);
  tp.addAtom(sptrOf<HdotsforAtom>(n, f));
  ((ArrayFormula*) tp._formula)->addCol(n);
  return nullptr;
}

inline macro(arrayATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser parser(tp.isPartial(), args[2], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), args[1], true);
}

inline macro(alignATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::align);
}

inline macro(flalignATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser parser(tp.isPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::flAlign);
}

inline macro(alignatATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser par(tp.isPartial(), args[2], arr, false);
  par.parse();
  arr->checkDimensions();
  size_t n = 0;
  valueof(args[1], n);
  if (arr->cols() != 2 * n) throw ex_parse("Bad number of equations in alignat environment!");

  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::alignAt);
}

inline macro(alignedATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::aligned);
}

inline macro(alignedatATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser p(tp.isPartial(), args[2], arr, false);
  p.parse();
  arr->checkDimensions();
  size_t n = 0;
  valueof(args[1], n);
  if (arr->cols() != 2 * n) {
    throw ex_parse("Bad number of equations in alignedat environment!");
  }

  return sptrOf<MatrixAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MatrixType::alignedAt);
}

inline macro(multlineATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1) {
    throw ex_parse("Requires exact one column in multiline envrionment!");
  }
  if (arr->cols() == 0) return nullptr;

  return sptrOf<MultlineAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MultiLineType::multiline);
}

inline macro(gatherATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1) throw ex_parse("Requires exact one column in gather envrionment!");
  if (arr->cols() == 0) return nullptr;

  return sptrOf<MultlineAtom>(
    tp.isPartial(), sptr<ArrayFormula>(arr), MultiLineType::gather);
}

inline macro(gatheredATATenv) {
  auto* arr = new ArrayFormula();
  TeXParser p(tp.isPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1) throw ex_parse("Requires exact one column in gathered envrionment!");
  if (arr->cols() == 0) return nullptr;

  return sptrOf<MultlineAtom>(tp.isPartial(), sptr<ArrayFormula>(arr), MultiLineType::gathered);
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

inline macro(questeq) {
  auto eq = SymbolAtom::get(Formula::_symbolMappings['=']);
  auto quest = SymbolAtom::get(Formula::_symbolMappings['?']);
  auto sq = sptrOf<ScaleAtom>(quest, 0.75f);
  auto at = sptrOf<UnderOverAtom>(eq, sq, UnitType::mu, 2.5f, true, true);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, at);
}

inline macro(stackrel) {
  sptr<Atom> a = sptrOf<UnderOverAtom>(
    Formula(tp, args[2], false)._root,
    Formula(tp, args[3], false)._root,
    UnitType::mu,
    0.5f,
    true,
    Formula(tp, args[1], false)._root,
    UnitType::mu,
    2.5f,
    true
  );
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, a);
}

inline macro(stackbin) {
  sptr<Atom> a = sptrOf<UnderOverAtom>(
    Formula(tp, args[2], false)._root,
    Formula(tp, args[3], false)._root,
    UnitType::mu,
    0.5f,
    true,
    Formula(tp, args[1], false)._root,
    UnitType::mu,
    2.5f,
    true
  );
  return sptrOf<TypedAtom>(AtomType::binaryOperator, AtomType::binaryOperator, a);
}

inline macro(overset) {
  sptr<Atom> a = sptrOf<UnderOverAtom>(
    Formula(tp, args[2], false)._root,
    Formula(tp, args[1], false)._root,
    UnitType::mu,
    2.5f,
    true,
    true
  );
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, a);
}

inline macro(underset) {
  sptr<Atom> a = sptrOf<UnderOverAtom>(
    Formula(tp, args[2], false)._root,
    Formula(tp, args[1], false)._root,
    UnitType::mu,
    0.5f,
    true,
    false
  );
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, a);
}

inline macro(accentset) {
  return sptrOf<AccentedAtom>(
    Formula(tp, args[2], false)._root,
    Formula(tp, args[1], false)._root
  );
}

inline macro(underaccent) {
  return sptrOf<UnderOverAtom>(
    Formula(tp, args[2], false)._root,
    Formula(tp, args[1], false)._root,
    UnitType::mu,
    0.3f,
    true,
    false
  );
}

inline macro(undertilde) {
  auto a = Formula(tp, args[1], false)._root;
  auto p = sptrOf<PhantomAtom>(a, true, false, false);
  auto acc = sptrOf<AccentedAtom>(p, "widetilde");
  return sptrOf<UnderOverAtom>(a, acc, UnitType::mu, 0.3f, true, false);
}

inline macro(boldsymbol) {
  return sptrOf<BoldAtom>(Formula(tp, args[1], false)._root);
}

inline macro(mathrm) {
  return sptrOf<RomanAtom>(Formula(tp, args[1], false)._root);
}

inline macro(rm) {
  return sptrOf<RomanAtom>(Formula(tp, tp.getOverArgument(), "", false, tp.isMathMode())._root);
}

inline macro(mathbf) {
  return sptrOf<BoldAtom>(sptrOf<RomanAtom>(Formula(tp, args[1], false)._root));
}

inline macro(bf) {
  return sptrOf<BoldAtom>(
    sptrOf<RomanAtom>(Formula(tp, tp.getOverArgument(), "", false, tp.isMathMode())._root)
  );
}

inline macro(mathtt) {
  return sptrOf<TtAtom>(Formula(tp, args[1], false)._root);
}

inline macro(tt) {
  return sptrOf<TtAtom>(Formula(tp, tp.getOverArgument(), "", false, tp.isMathMode())._root);
}

inline macro(mathit) {
  return sptrOf<ItAtom>(Formula(tp, args[1], false)._root);
}

inline macro(it) {
  return sptrOf<ItAtom>(Formula(tp, tp.getOverArgument(), "", false, tp.isMathMode())._root);
}

inline macro(mathsf) {
  return sptrOf<SsAtom>(Formula(tp, args[1], false)._root);
}

inline macro(sf) {
  return sptrOf<SsAtom>(Formula(tp, tp.getOverArgument(), "", false, tp.isMathMode())._root);
}

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

inline macro(displaystyle) {
  auto g = Formula(tp, tp.getOverArgument(), false)._root;
  return sptrOf<StyleAtom>(TexStyle::display, g);
}

inline macro(scriptstyle) {
  auto g = Formula(tp, tp.getOverArgument(), false)._root;
  return sptrOf<StyleAtom>(TexStyle::script, g);
}

inline macro(textstyle) {
  auto g = Formula(tp, tp.getOverArgument(), false)._root;
  return sptrOf<StyleAtom>(TexStyle::text, g);
}

inline macro(scriptscriptstyle) {
  auto g = Formula(tp, tp.getOverArgument(), false)._root;
  return sptrOf<StyleAtom>(TexStyle::scriptScript, g);
}

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
  auto a = Formula(tp, args[2])._root;
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

inline macro(cong) {
  auto* vra = new VRowAtom(SymbolAtom::get("equals"));
  vra->add(sptrOf<SpaceAtom>(UnitType::mu, 0.f, 1.5f, 0.f));
  vra->add(SymbolAtom::get("sim"));
  vra->setRaise(UnitType::mu, -1);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, sptr<Atom>(vra));
}

inline macro(doteq) {
  auto e = SymbolAtom::get("equals");
  auto l = SymbolAtom::get("ldotp");
  auto u = sptrOf<UnderOverAtom>(e, l, UnitType::mu, 3.7f, false, true);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, u);
}

inline macro(externalfont) {
  std::string x = wide2utf8(args[1]);
  TextRenderingBox::setFont(x);
  return nullptr;
}

inline macro(ctext) {
  return sptrOf<TextRenderingAtom>(args[1], PLAIN);
}

inline macro(textit) {
  return sptrOf<TextRenderingAtom>(args[1], ITALIC);
}

inline macro(textbf) {
  return sptrOf<TextRenderingAtom>(args[1], BOLD);
}

inline macro(textitbf) {
  return sptrOf<TextRenderingAtom>(args[1], BOLD | ITALIC);
}

inline macro(declaremathsizes) {
  float a, b, c, d;
  valueof(args[1], a), valueof(args[2], b), valueof(args[3], c), valueof(args[4], d);
  DefaultTeXFont::setMathSizes(a, b, c, c);
  return nullptr;
}

inline macro(magnification) {
  float x;
  valueof(args[1], x);
  DefaultTeXFont::setMagnification(x);
  return nullptr;
}

inline macro(hline) {
  if (!tp.isArrayMode())
    throw ex_parse("The macro \\hline only available in array mode!");
  return sptrOf<HlineAtom>();
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
  float space) {
  return sptrOf<UnderOverAtom>(
    SymbolAtom::get(base),
    SymbolAtom::get(script),
    UnitType::mu,
    space,
    false,
    true
  );
}

inline sptr<Atom> _colon() {
  return _underover("normaldot", "normaldot", 5.2f);
}

inline macro(dotminus) {
  sptr<Atom> a = _underover("minus", "normaldot", -3.3f);
  return sptrOf<TypedAtom>(AtomType::binaryOperator, AtomType::binaryOperator, a);
}

inline macro(ratio) {
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, _colon());
}

inline macro(geoprop) {
  auto ddot = sptrOf<RowAtom>(SymbolAtom::get("normaldot"));
  ddot->add(sptrOf<SpaceAtom>(UnitType::mu, 4.f, 0.f, 0.f));
  ddot->add(SymbolAtom::get("normaldot"));
  sptr<Atom> a = sptrOf<UnderOverAtom>(
    SymbolAtom::get("minus"),
    ddot,
    UnitType::mu,
    -3.4f,
    false,
    ddot,
    UnitType::mu,
    -3.4f,
    false
  );
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, a);
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

inline macro(smallfrowneq) {
  sptr<Atom> u = sptrOf<UnderOverAtom>(
    SymbolAtom::get("equals"),
    SymbolAtom::get("smallfrown"),
    UnitType::mu,
    -2.f,
    true,
    true
  );
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, u);
}

inline macro(hstrok) {
  auto ra = sptrOf<RowAtom>(sptrOf<SpaceAtom>(UnitType::ex, -0.1f, 0.f, 0.f));
  ra->add(SymbolAtom::get("bar"));
  auto* vra = new VRowAtom(sptrOf<LapedAtom>(ra, 'r'));
  vra->setRaise(UnitType::ex, -0.1f);
  auto* a = new RowAtom(sptr<Atom>(vra));
  a->add(sptrOf<RomanAtom>(sptrOf<CharAtom>('h', tp._formula->_textStyle)));
  return sptr<Atom>(a);
}

inline macro(Hstrok) {
  auto* ra = new RowAtom(sptrOf<SpaceAtom>(UnitType::ex, -0.28f, 0.f, 0.f));
  ra->add(SymbolAtom::get("textendash"));
  auto* vra = new VRowAtom(sptrOf<LapedAtom>(sptr<Atom>(ra), 'r'));
  vra->setRaise(UnitType::ex, 0.55f);
  auto* a = new RowAtom(sptr<Atom>(vra));
  a->add(sptrOf<RomanAtom>(sptrOf<CharAtom>('H', tp._formula->_textStyle)));
  return sptr<Atom>(a);
}

inline macro(dstrok) {
  auto* ra = new RowAtom(sptrOf<SpaceAtom>(UnitType::ex, 0.25f, 0.f, 0.f));
  ra->add(SymbolAtom::get("bar"));
  auto* vra = new VRowAtom(sptrOf<LapedAtom>(sptr<Atom>(ra), 'r'));
  vra->setRaise(UnitType::ex, -0.1f);
  auto* a = new RowAtom(sptr<Atom>(vra));
  a->add(sptrOf<RomanAtom>(sptrOf<CharAtom>('d', tp._formula->_textStyle)));
  return sptr<Atom>(a);
}

inline macro(Dstrok) {
  auto* ra = new RowAtom(sptrOf<SpaceAtom>(UnitType::ex, -0.1f, 0.f, 0.f));
  ra->add(SymbolAtom::get("bar"));
  auto* vra = new VRowAtom(sptrOf<LapedAtom>(sptr<Atom>(ra), 'r'));
  vra->setRaise(UnitType::ex, -0.55f);
  auto* a = new RowAtom(sptr<Atom>(vra));
  a->add(sptrOf<RomanAtom>(sptrOf<CharAtom>('D', tp._formula->_textStyle)));
  return sptr<Atom>(a);
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
  return tp.convertCharacter((wchar_t) n, true);
}

inline macro(T) {
  return sptrOf<RotateAtom>(Formula(tp, args[1])._root, 180.f, L"origin=cc");
}

inline macro(textcircled) {
  return sptrOf<TextCircledAtom>(sptrOf<RomanAtom>(Formula(tp, args[1])._root));
}

inline macro(textsc) {
  return sptrOf<SmallCapAtom>(Formula(tp, args[1], false)._root);
}

inline macro(sc) {
  return sptrOf<SmallCapAtom>(Formula(tp, tp.getOverArgument(), "", false, tp.isMathMode())._root);
}

inline macro(quad) {
  return sptrOf<SpaceAtom>(UnitType::em, 1.f, 0.f, 0.f);
}

inline macro(surd) {
  return sptrOf<VCenteredAtom>(SymbolAtom::get("surdsign"));
}

inline macro(int) {
  auto* integral = new SymbolAtom(*(SymbolAtom::get("int")));
  integral->_limitsType = LimitsType::noLimits;
  return sptr<Atom>(integral);
}

inline macro(oint) {
  auto* integral = new SymbolAtom(*(SymbolAtom::get("oint")));
  integral->_limitsType = LimitsType::noLimits;
  return sptr<Atom>(integral);
}

inline macro(iint) {
  auto* integral = new SymbolAtom(*(SymbolAtom::get("int")));
  integral->_limitsType = LimitsType::noLimits;
  sptr<Atom> i(integral);
  auto* ra = new RowAtom(i);
  ra->add(sptrOf<SpaceAtom>(UnitType::mu, -8.f, 0.f, 0.f));
  ra->add(i);
  ra->_lookAtLastAtom = true;
  return sptrOf<TypedAtom>(AtomType::bigOperator, AtomType::bigOperator, sptr<Atom>(ra));
}

inline macro(iiint) {
  auto* integral = new SymbolAtom(*(SymbolAtom::get("int")));
  integral->_limitsType = LimitsType::noLimits;
  sptr<Atom> i(integral);
  auto* ra = new RowAtom(i);
  ra->add(sptrOf<SpaceAtom>(UnitType::mu, -8.f, 0.f, 0.f));
  ra->add(i);
  ra->add(sptrOf<SpaceAtom>(UnitType::mu, -8.f, 0.f, 0.f));
  ra->add(i);
  ra->_lookAtLastAtom = true;
  return sptrOf<TypedAtom>(AtomType::bigOperator, AtomType::bigOperator, sptr<Atom>(ra));
}

inline macro(iiiint) {
  auto* integral = new SymbolAtom(*(SymbolAtom::get("int")));
  integral->_limitsType = LimitsType::noLimits;
  sptr<Atom> i(integral);
  auto* ra = new RowAtom(i);
  ra->add(sptrOf<SpaceAtom>(UnitType::mu, -8.f, 0.f, 0.f));
  ra->add(i);
  ra->add(sptrOf<SpaceAtom>(UnitType::mu, -8.f, 0.f, 0.f));
  ra->add(i);
  ra->add(sptrOf<SpaceAtom>(UnitType::mu, -8.f, 0.f, 0.f));
  ra->add(i);
  ra->_lookAtLastAtom = true;
  return sptrOf<TypedAtom>(AtomType::bigOperator, AtomType::bigOperator, sptr<Atom>(ra));
}

inline macro(idotsint) {
  auto* integral = new SymbolAtom(*(SymbolAtom::get("int")));
  integral->_limitsType = LimitsType::noLimits;
  sptr<Atom> i(integral);
  auto* ra = new RowAtom(i);
  ra->add(sptrOf<SpaceAtom>(UnitType::mu, -1.f, 0.f, 0.f));
  auto cdotp = SymbolAtom::get("cdotp");
  auto* cdots = new RowAtom(cdotp);
  cdots->add(cdotp);
  cdots->add(cdotp);
  ra->add(sptrOf<TypedAtom>(AtomType::inner, AtomType::inner, sptr<Atom>(cdots)));
  ra->add(sptrOf<SpaceAtom>(UnitType::mu, -1.f, 0.f, 0.f));
  ra->add(i);
  ra->_lookAtLastAtom = true;
  return sptrOf<TypedAtom>(AtomType::bigOperator, AtomType::bigOperator, sptr<Atom>(ra));
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

macro(textstyles);

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

macro(xml);

/**************************************** not implemented *****************************************/

inline macro(includegraphics) {
  return nullptr;
}

inline macro(fcscore) {
  return nullptr;
}

inline macro(GeoGebra) {
  return nullptr;
}

inline macro(dynamic) {
  return nullptr;
}

}  // namespace tex

#endif  // MACRO_IMPL_H_INCLUDED
