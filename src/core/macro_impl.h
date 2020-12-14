#ifndef MACRO_IMPL_H_INCLUDED
#define MACRO_IMPL_H_INCLUDED

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
#define macro(name) sptr<Atom> macro_##name(TeXParser& tp, std::vector<std::wstring>& args)
#endif

#ifdef GRAPHICS_DEBUG
inline macro(debug) {
  TeXFormula::setDEBUG(true);
  return nullptr;
}

inline macro(undebug) {
  TeXFormula::setDEBUG(false);
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
  tp.addAtom(sptr<Atom>(new MultiRowAtom(n, args[2], TeXFormula(tp, args[3])._root)));
  return nullptr;
}

inline macro(longdiv) {
  long dividend = 0;
  valueof(args[1], dividend);
  long divisor = 0;
  valueof(args[2], divisor);
  if (divisor == 0) throw ex_parse("Divisor must not be 0.");
  return sptr<Atom>(new LongDivAtom(divisor, dividend));
}

inline macro(cellcolor) {
  if (!tp.isArrayMode()) throw ex_parse("Command \\cellcolor must used in array environment!");
  color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
  sptr<CellSpecifier> atom(new CellColorAtom(c));
  ((ArrayOfAtoms*)tp._formula)->addCellSpecifier(atom);
  return nullptr;
}

inline macro(color) {
  // We do not care the \color command in non-array mode, since we did pass a color as a parameter
  // when parsing a LaTeX string, it is useless to specify a global foreground color again, but in
  // array mode, the \color command is useful to specify the foreground color of the columns.
  if (tp.isArrayMode()) {
    color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
    return sptr<Atom>(new CellForegroundAtom(c));
  }
  return nullptr;
}

inline macro(newcolumntype) {
  MatrixAtom::defineColumnSpecifier(args[1], args[2]);
  return nullptr;
}

inline macro(arrayrulecolor) {
  color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
  MatrixAtom::LINE_COLOR = c;
  return nullptr;
}

inline macro(columnbg) {
  color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
  return sptr<Atom>(new CellColorAtom(c));
}

inline macro(rowcolor) {
  if (!tp.isArrayMode()) throw ex_parse("Command \\rowcolor must used in array environment!");
  color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
  sptr<CellSpecifier> spe(new CellColorAtom(c));
  ((ArrayOfAtoms*)tp._formula)->addRowSpecifier(spe);
  return nullptr;
}

inline macro(st) {
  auto base = TeXFormula(tp, args[1], false)._root;
  return sptr<Atom>(new StrikeThroughAtom(base));
}

inline macro(Braket) {
  std::wstring str(args[1]);
  replaceall(str, L"\\|", L"\\middle\\vert ");
  return TeXFormula(tp, L"\\left\\langle " + str + L"\\right\\rangle")._root;
}

inline macro(Set) {
  std::wstring str(args[1]);
  replacefirst(str, L"\\|", L"\\middle\\vert ");
  return TeXFormula(tp, L"\\left\\{" + str + L"\\right\\}")._root;
}

inline macro(spATbreve) {
  VRowAtom* vra = new VRowAtom(TeXFormula(L"\\displaystyle\\!\\breve{}")._root);
  vra->setRaise(UNIT_EX, 0.6f);
  return sptr<Atom>(new SmashedAtom(sptr<Atom>(vra), ""));
}

inline macro(spAThat) {
  VRowAtom* vra = new VRowAtom(TeXFormula(L"\\displaystyle\\widehat{}")._root);
  vra->setRaise(UNIT_EX, 0.6f);
  return sptr<Atom>(new SmashedAtom(sptr<Atom>(vra), ""));
}

inline macro(clrlap) {
  return sptr<Atom>(new LapedAtom(TeXFormula(tp, args[1])._root, args[0][0]));
}

inline macro(mathclrlap) {
  return sptr<Atom>(new LapedAtom(TeXFormula(tp, args[1])._root, args[0][4]));
}

inline macro(frac) {
  TeXFormula num(tp, args[1], false);
  TeXFormula den(tp, args[2], false);
  if (num._root == nullptr || den._root == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptr<Atom>(new FractionAtom(num._root, den._root, true));
}

inline macro(over) {
  auto num = tp.getFormulaAtom();
  auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptr<Atom>(new FractionAtom(num, den, true));
}

inline macro(atop) {
  auto num = tp.getFormulaAtom();
  auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptr<Atom>(new FractionAtom(num, den, false));
}

inline sptr<Atom> _macro_choose(
    const std::string& left, const std::string& right,
    _out_ TeXParser& tp, _out_ std::vector<std::wstring>& args) {
  auto num = tp.getFormulaAtom();
  auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of choos can't be empty!");
  sptr<Atom> f(new FractionAtom(num, den, false));
  sptr<SymbolAtom> l(new SymbolAtom(left, TYPE_OPENING, true));
  sptr<SymbolAtom> r(new SymbolAtom(right, TYPE_CLOSING, true));
  return sptr<Atom>(new FencedAtom(f, l, r));
}

inline macro(choose) {
  return _macro_choose("lbrack", "rbrack", tp, args);
}

inline macro(brack) {
  return _macro_choose("lsqbrack", "rsqbrack", tp, args);
}

inline macro(bangle) {
  return _macro_choose("langle", "rangle", tp, args);
}

inline macro(brace) {
  return _macro_choose("lbrace", "rbrace", tp, args);
}

inline sptr<Atom> _marco_cancel(
    int cancelType,
    _out_ TeXParser& tp, _out_ std::vector<std::wstring>& args) {
  auto base = TeXFormula(tp, args[1], false)._root;
  if (base == nullptr)
    throw ex_parse("Cancel content must not be empty!");
  return sptr<Atom>(new CancelAtom(base, cancelType));
}

inline macro(cancel) {
  return _marco_cancel(CancelAtom::SLASH, tp, args);
}

inline macro(bcancel) {
  return _marco_cancel(CancelAtom::BACKSLASH, tp, args);
}

inline macro(xcancel) {
  return _marco_cancel(CancelAtom::CROSS, tp, args);
}

inline macro(binom) {
  TeXFormula num(tp, args[1], false);
  TeXFormula den(tp, args[2], false);
  if (num._root == nullptr || den._root == nullptr)
    throw ex_parse("Both binomial coefficients must be not empty!");
  sptr<Atom> f(new FractionAtom(num._root, den._root, false));
  sptr<SymbolAtom> s1(new SymbolAtom("lbrack", TYPE_OPENING, true));
  sptr<SymbolAtom> s2(new SymbolAtom("rbrack", TYPE_CLOSING, true));
  return sptr<Atom>(new FencedAtom(f, s1, s2));
}

inline macro(above) {
  auto num = tp.getFormulaAtom();
  std::pair<int, float> dim = tp.getLength();
  auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");

  return sptr<Atom>(new FractionAtom(num, den, dim.first, dim.second));
}

inline macro(mbox) {
  sptr<Atom> group(new RomanAtom(TeXFormula(tp, args[1], "mathnormal", false, false)._root));
  return sptr<Atom>(new StyleAtom(STYLE_TEXT, group));
}

inline macro(text) {
  return sptr<Atom>(new RomanAtom(TeXFormula(tp, args[1], "mathnormal", false, false)._root));
}

inline macro(underscore) {
  return sptr<Atom>(new UnderScoreAtom());
}

inline macro(accents) {
  std::string x;
  wide2utf8(args[0].c_str(), x);
  return sptr<Atom>(new AccentedAtom(TeXFormula(tp, args[1], false)._root, x));
}

inline macro(grkaccent) {
  return sptr<Atom>(new AccentedAtom(
      TeXFormula(tp, args[2], false)._root, TeXFormula(tp, args[1], false)._root, false));
}

inline macro(accent) {
  return sptr<Atom>(new AccentedAtom(
      TeXFormula(tp, args[2], false)._root, TeXFormula(tp, args[1], false)._root));
}

inline macro(cedilla) {
  return sptr<Atom>(new CedillAtom(TeXFormula(tp, args[1])._root));
}

inline macro(IJ) {
  return sptr<Atom>(new IJAtom(args[0][0] == 'I'));
}

inline macro(TStroke) {
  return sptr<Atom>(new TStrokeAtom(args[0][0] == 'T'));
}

inline macro(LCaron) {
  return sptr<Atom>(new LCaronAtom(args[0][0] == 'L'));
}

inline macro(tcaron) {
  return sptr<Atom>(new TCaronAtom());
}

inline macro(ogonek) {
  return sptr<Atom>(new OgonekAtom(TeXFormula(tp, args[1])._root));
}

inline macro(nbsp) {
  return sptr<Atom>(new SpaceAtom());
}

inline macro(sqrt) {
  if (args[2].empty())
    return sptr<Atom>(new NthRoot(TeXFormula(tp, args[1], false)._root, nullptr));
  return sptr<Atom>(new NthRoot(
      TeXFormula(tp, args[1], false)._root, TeXFormula(tp, args[2], false)._root));
}

inline macro(overrightarrow) {
  return sptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, false, true));
}

inline macro(overleftarrow) {
  return sptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, true, true));
}

inline macro(overleftrightarrow) {
  return sptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, true));
}

inline macro(underrightarrow) {
  return sptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, false, false));
}

inline macro(underleftarrow) {
  return sptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, true, false));
}

inline macro(underleftrightarrow) {
  return sptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, false));
}

inline macro(xleftarrow) {
  return sptr<Atom>(new XArrowAtom(
      TeXFormula(tp, args[1], false)._root, TeXFormula(tp, args[2])._root, true));
}

inline macro(xrightarrow) {
  return sptr<Atom>(new XArrowAtom(
      TeXFormula(tp, args[1], false)._root, TeXFormula(tp, args[2])._root, false));
}

inline macro(sideset) {
  auto l = TeXFormula(tp, args[1])._root;
  auto r = TeXFormula(tp, args[2])._root;
  auto op = TeXFormula(tp, args[3])._root;
  if (op == nullptr) {
    sptr<Atom> in(new CharAtom(L'M', "mathnormal"));
    op = sptr<Atom>(new PhantomAtom(in, false, true, true));
  }
  auto cl = dynamic_cast<CumulativeScriptsAtom*>(l.get());
  auto cr = dynamic_cast<CumulativeScriptsAtom*>(r.get());
  if (cl != nullptr) l = cl->getScriptsAtom();
  if (cr != nullptr) r = cr->getScriptsAtom();
  return sptr<Atom>(new SideSetsAtom(op, l, r));
}

inline macro(prescript) {
  auto base = TeXFormula(tp, args[3])._root;
  sptr<Atom> p(new PhantomAtom(base, false, true, true));
  sptr<Atom> s(new ScriptsAtom(
      p, TeXFormula(tp, args[2])._root, TeXFormula(tp, args[1])._root, false));
  tp.addAtom(s);
  tp.addAtom(sptr<Atom>(new SpaceAtom(UNIT_MU, -0.3f, 0, 0)));
  return sptr<Atom>(new TypedAtom(TYPE_ORDINARY, TYPE_ORDINARY, base));
}

inline macro(underbrace) {
  return sptr<Atom>(new OverUnderDelimiter(
      TeXFormula(tp, args[1], false)._root,
      nullptr,
      SymbolAtom::get("rbrace"),
      UNIT_EX,
      0,
      false));
}

inline macro(overbrace) {
  return sptr<Atom>(new OverUnderDelimiter(
      TeXFormula(tp, args[1], false)._root,
      nullptr,
      SymbolAtom::get("lbrace"),
      UNIT_EX,
      0,
      true));
}

inline macro(underbrack) {
  return sptr<Atom>(new OverUnderDelimiter(
      TeXFormula(tp, args[1], false)._root,
      nullptr,
      SymbolAtom::get("rsqbrack"),
      UNIT_EX,
      0,
      false));
}

inline macro(overbrack) {
  return sptr<Atom>(new OverUnderDelimiter(
      TeXFormula(tp, args[1], false)._root,
      nullptr,
      SymbolAtom::get("lsqbrack"),
      UNIT_EX,
      0,
      true));
}

inline macro(underparen) {
  return sptr<Atom>(new OverUnderDelimiter(
      TeXFormula(tp, args[1], false)._root,
      nullptr,
      SymbolAtom::get("rbrack"),
      UNIT_EX,
      0,
      false));
}

inline macro(overparen) {
  return sptr<Atom>(new OverUnderDelimiter(
      TeXFormula(tp, args[1], false)._root,
      nullptr,
      SymbolAtom::get("lbrack"),
      UNIT_EX,
      0,
      true));
}

inline macro(overline) {
  return sptr<Atom>(new OverlinedAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(underline) {
  return sptr<Atom>(new UnderlinedAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(mathop) {
  sptr<TypedAtom> a(new TypedAtom(
      TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, TeXFormula(tp, args[1], false)._root));
  a->_typelimits = SCRIPT_NORMAL;
  return a;
}

inline macro(mathpunct) {
  return sptr<Atom>(new TypedAtom(
      TYPE_PUNCTUATION, TYPE_PUNCTUATION, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathord) {
  return sptr<Atom>(new TypedAtom(
      TYPE_ORDINARY, TYPE_ORDINARY, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathrel) {
  return sptr<Atom>(new TypedAtom(
      TYPE_RELATION, TYPE_RELATION, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathinner) {
  return sptr<Atom>(new TypedAtom(
      TYPE_INNER, TYPE_INNER, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathbin) {
  return sptr<Atom>(new TypedAtom(
      TYPE_BINARY_OPERATOR, TYPE_BINARY_OPERATOR, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathopen) {
  return sptr<Atom>(new TypedAtom(
      TYPE_OPENING, TYPE_OPENING, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathclose) {
  return sptr<Atom>(new TypedAtom(
      TYPE_CLOSING, TYPE_CLOSING, TeXFormula(tp, args[1], false)._root));
}

inline macro(joinrel) {
  return sptr<Atom>(new TypedAtom(
      TYPE_RELATION, TYPE_RELATION, sptr<Atom>(new SpaceAtom(UNIT_MU, -2.6f, 0, 0))));
}

inline macro(smash) {
  std::string x;
  wide2utf8(args[2].c_str(), x);
  return sptr<Atom>(new SmashedAtom(TeXFormula(tp, args[1], false)._root, x));
}

inline macro(vdots) {
  return sptr<Atom>(new VdotsAtom());
}

inline macro(ddots) {
  return sptr<Atom>(new TypedAtom(
      TYPE_INNER, TYPE_INNER, sptr<Atom>(new DdtosAtom())));
}

inline macro(iddots) {
  return sptr<Atom>(new TypedAtom(
      TYPE_INNER, TYPE_INNER, sptr<Atom>(new IddotsAtom())));
}

inline macro(leftparenthesis) {
  std::wstring grp = tp.getGroup(L"\\(", L"\\)");
  return sptr<Atom>(new MathAtom(TeXFormula(tp, grp, false)._root, STYLE_TEXT));
}

inline macro(leftbracket) {
  std::wstring grp = tp.getGroup(L"\\[", L"\\]");
  return sptr<Atom>(new MathAtom(TeXFormula(tp, grp, false)._root, STYLE_DISPLAY));
}

inline macro(middle) {
  return sptr<Atom>(new MiddleAtom(TeXFormula(tp, args[1])._root));
}

inline macro(cr) {
  if (tp.isArrayMode()) {
    tp.addRow();
  } else {
    ArrayOfAtoms arr;
    arr.add(tp._formula->_root);
    arr.addRow();
    TeXParser parser(
        tp.getIsPartial(), tp.forwardFromCurrentPos(), &arr, false, tp.isIgnoreWhiteSpace());
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
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser parser(tp.getIsPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptr<Atom>(new MatrixAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), SMALLMATRIX));
}

inline macro(matrixATATenv) {
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser parser(tp.getIsPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptr<Atom>(new MatrixAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), MATRIX));
}

inline macro(multicolumn) {
  int n = 0;
  valueof(args[1], n);
  std::string x;
  wide2utf8(args[2].c_str(), x);
  tp.addAtom(sptr<Atom>(new MulticolumnAtom(n, x, TeXFormula(tp, args[3])._root)));
  ((ArrayOfAtoms*)tp._formula)->addCol(n);
  return nullptr;
}

inline macro(hdotsfor) {
  if (!tp.isArrayMode())
    throw ex_parse("Command 'hdotsfor' only available in array mode!");
  int n = 0;
  valueof(args[1], n);
  float f = 1.f;
  if (!args[2].empty()) valueof(args[2], f);
  tp.addAtom(sptr<Atom>(new HdotsforAtom(n, f)));
  ((ArrayOfAtoms*)tp._formula)->addCol(n);
  return nullptr;
}

inline macro(arrayATATenv) {
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser parser(tp.getIsPartial(), args[2], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptr<Atom>(new MatrixAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), args[1], true));
}

inline macro(alignATATenv) {
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser parser(tp.getIsPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptr<Atom>(new MatrixAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), ALIGN));
}

inline macro(flalignATATenv) {
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser parser(tp.getIsPartial(), args[1], arr, false);
  parser.parse();
  arr->checkDimensions();
  return sptr<Atom>(new MatrixAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), FLALIGN));
}

inline macro(alignatATATenv) {
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser par(tp.getIsPartial(), args[2], arr, false);
  par.parse();
  arr->checkDimensions();
  size_t n = 0;
  valueof(args[1], n);
  if (arr->cols() != 2 * n) throw ex_parse("Bad number of equations in alignat environment!");

  return sptr<Atom>(new MatrixAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), ALIGNAT));
}

inline macro(alignedATATenv) {
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser p(tp.getIsPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  return sptr<Atom>(new MatrixAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), ALIGNED));
}

inline macro(alignedatATATenv) {
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser p(tp.getIsPartial(), args[2], arr, false);
  p.parse();
  arr->checkDimensions();
  size_t n = 0;
  valueof(args[1], n);
  if (arr->cols() != 2 * n)
    throw ex_parse("Bad number of equations in alignedat environment!");

  return sptr<Atom>(new MatrixAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), ALIGNEDAT));
}

inline macro(multlineATATenv) {
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser p(tp.getIsPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1)
    throw ex_parse("Requires exact one column in multiline envrionment!");
  if (arr->cols() == 0)
    return nullptr;

  return sptr<Atom>(new MultlineAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), MULTILINE));
}

inline macro(gatherATATenv) {
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser p(tp.getIsPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1) throw ex_parse("Requires exact one column in gather envrionment!");
  if (arr->cols() == 0) return nullptr;

  return sptr<Atom>(new MultlineAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), GATHER));
}

inline macro(gatheredATATenv) {
  ArrayOfAtoms* arr = new ArrayOfAtoms();
  TeXParser p(tp.getIsPartial(), args[1], arr, false);
  p.parse();
  arr->checkDimensions();
  if (arr->cols() > 1) throw ex_parse("Requires exact one column in gathered envrionment!");
  if (arr->cols() == 0) return nullptr;

  return sptr<Atom>(new MultlineAtom(
      tp.getIsPartial(), sptr<ArrayOfAtoms>(arr), GATHERED));
}

inline macro(shoveright) {
  auto a = TeXFormula(tp, args[1])._root;
  a->_alignment = ALIGN_RIGHT;
  return a;
}

inline macro(shoveleft) {
  auto a = TeXFormula(tp, args[1])._root;
  a->_alignment = ALIGN_LEFT;
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
  return sptr<Atom>(new FBoxAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(questeq) {
  auto eq = SymbolAtom::get(TeXFormula::_symbolMappings['=']);
  auto quest = SymbolAtom::get(TeXFormula::_symbolMappings['?']);
  auto sq = sptr<Atom>(new ScaleAtom(quest, 0.75f));
  auto at = sptr<Atom>(new UnderOverAtom(eq, sq, UNIT_MU, 2.5f, true, true));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, at));
}

inline macro(stackrel) {
  sptr<Atom> a(new UnderOverAtom(
      TeXFormula(tp, args[2], false)._root,
      TeXFormula(tp, args[3], false)._root,
      UNIT_MU,
      0.5f,
      true,
      TeXFormula(tp, args[1], false)._root,
      UNIT_MU,
      2.5f,
      true));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline macro(stackbin) {
  sptr<Atom> a(new UnderOverAtom(
      TeXFormula(tp, args[2], false)._root,
      TeXFormula(tp, args[3], false)._root,
      UNIT_MU,
      0.5f,
      true,
      TeXFormula(tp, args[1], false)._root,
      UNIT_MU,
      2.5f,
      true));
  return sptr<Atom>(new TypedAtom(TYPE_BINARY_OPERATOR, TYPE_BINARY_OPERATOR, a));
}

inline macro(overset) {
  sptr<Atom> a(new UnderOverAtom(
      TeXFormula(tp, args[2], false)._root,
      TeXFormula(tp, args[1], false)._root,
      UNIT_MU,
      2.5f,
      true,
      true));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline macro(underset) {
  sptr<Atom> a(new UnderOverAtom(
      TeXFormula(tp, args[2], false)._root,
      TeXFormula(tp, args[1], false)._root,
      UNIT_MU,
      0.5f,
      true,
      false));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline macro(accentset) {
  return sptr<Atom>(new AccentedAtom(
      TeXFormula(tp, args[2], false)._root, TeXFormula(tp, args[1], false)._root));
}

inline macro(underaccent) {
  return sptr<Atom>(new UnderOverAtom(
      TeXFormula(tp, args[2], false)._root,
      TeXFormula(tp, args[1], false)._root,
      UNIT_MU,
      0.3f,
      true,
      false));
}

inline macro(undertilde) {
  auto a = TeXFormula(tp, args[1], false)._root;
  sptr<PhantomAtom> p(new PhantomAtom(a, true, false, false));
  sptr<AccentedAtom> acc(new AccentedAtom(p, "widetilde"));
  return sptr<Atom>(new UnderOverAtom(a, acc, UNIT_MU, 0.3f, true, false));
}

inline macro(boldsymbol) {
  return sptr<Atom>(new BoldAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(mathrm) {
  return sptr<Atom>(new RomanAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(rm) {
  return sptr<Atom>(new RomanAtom(
      TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline macro(mathbf) {
  return sptr<Atom>(new BoldAtom(
      sptr<Atom>(new RomanAtom(TeXFormula(tp, args[1], false)._root))));
}

inline macro(bf) {
  return sptr<Atom>(new BoldAtom(sptr<Atom>(new RomanAtom(
      TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root))));
}

inline macro(mathtt) {
  return sptr<Atom>(new TtAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(tt) {
  return sptr<Atom>(new TtAtom(
      TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline macro(mathit) {
  return sptr<Atom>(new ItAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(it) {
  return sptr<Atom>(new ItAtom(
      TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline macro(mathsf) {
  return sptr<Atom>(new SsAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(sf) {
  return sptr<Atom>(new SsAtom(
      TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline macro(LaTeX) {
  return sptr<Atom>(new LaTeXAtom());
}

inline macro(hphantom) {
  return sptr<Atom>(
      new PhantomAtom(TeXFormula(tp, args[1], false)._root, true, false, false));
}

inline macro(vphantom) {
  return sptr<Atom>(
      new PhantomAtom(TeXFormula(tp, args[1], false)._root, false, true, true));
}

inline macro(phantom) {
  return sptr<Atom>(
      new PhantomAtom(TeXFormula(tp, args[1], false)._root, true, true, true));
}

inline sptr<Atom> _macro_big(TeXParser& tp, std::vector<std::wstring>& args, int size, int type = -1) {
  auto a = TeXFormula(tp, args[1], false)._root;
  auto s = std::dynamic_pointer_cast<SymbolAtom>(a);
  if (s == nullptr) return a;
  sptr<Atom> t(new BigDelimiterAtom(s, size));
  if (type != -1) t->_type = type;
  return t;
}

inline macro(big) { return _macro_big(tp, args, 1); }

inline macro(Big) { return _macro_big(tp, args, 2); }

inline macro(bigg) { return _macro_big(tp, args, 3); }

inline macro(Bigg) { return _macro_big(tp, args, 4); }

inline macro(bigl) { return _macro_big(tp, args, 1, TYPE_OPENING); }

inline macro(Bigl) { return _macro_big(tp, args, 2, TYPE_OPENING); }

inline macro(biggl) { return _macro_big(tp, args, 3, TYPE_OPENING); }

inline macro(Biggl) { return _macro_big(tp, args, 4, TYPE_OPENING); }

inline macro(bigr) { return _macro_big(tp, args, 1, TYPE_CLOSING); }

inline macro(Bigr) { return _macro_big(tp, args, 2, TYPE_CLOSING); }

inline macro(biggr) { return _macro_big(tp, args, 3, TYPE_CLOSING); }

inline macro(Biggr) { return _macro_big(tp, args, 4, TYPE_CLOSING); }

inline macro(displaystyle) {
  auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
  return sptr<Atom>(new StyleAtom(STYLE_DISPLAY, g));
}

inline macro(scriptstyle) {
  auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
  return sptr<Atom>(new StyleAtom(STYLE_SCRIPT, g));
}

inline macro(textstyle) {
  auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
  return sptr<Atom>(new StyleAtom(STYLE_TEXT, g));
}

inline macro(scriptscriptstyle) {
  auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
  return sptr<Atom>(new StyleAtom(STYLE_SCRIPT_SCRIPT, g));
}

inline macro(rotatebox) {
  float x = 0;
  if (!args[1].empty()) valueof(args[1], x);
  return sptr<Atom>(new RotateAtom(TeXFormula(tp, args[2])._root, x, args[3]));
}

inline macro(reflectbox) {
  return sptr<Atom>(new ReflectAtom(TeXFormula(tp, args[1])._root));
}

inline macro(scalebox) {
  float sx = 1, sy = 1;
  valueof(args[1], sx);
  if (args[3].empty())
    sy = sx;
  else
    valueof(args[3], sy);
  if (sx == 0) sx = 1;
  if (sy == 0) sy = 1;
  return sptr<Atom>(new ScaleAtom(TeXFormula(tp, args[2])._root, sx, sy));
}

inline macro(resizebox) {
  std::string ws, hs;
  wide2utf8(args[1].c_str(), ws);
  wide2utf8(args[2].c_str(), hs);
  return sptr<Atom>(new ResizeAtom(
      TeXFormula(tp, args[3])._root, ws, hs, ws == "!" || hs == "!"));
}

inline macro(shadowbox) {
  return sptr<Atom>(new ShadowAtom(TeXFormula(tp, args[1])._root));
}

inline macro(ovalbox) {
  return sptr<Atom>(new OvalAtom(TeXFormula(tp, args[1])._root));
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
  return sptr<Atom>(new DoubleFramedAtom(TeXFormula(tp, args[1])._root));
}

inline macro(fgcolor) {
  auto a = TeXFormula(tp, args[2])._root;
  std::string x = wide2utf8(args[1].c_str());
  return sptr<Atom>(new ColorAtom(a, TRANS, ColorAtom::getColor(x)));
}

inline macro(bgcolor) {
  auto a = TeXFormula(tp, args[2])._root;
  std::string x = wide2utf8(args[1].c_str());
  return sptr<Atom>(new ColorAtom(a, ColorAtom::getColor(x), TRANS));
}

inline macro(textcolor) {
  auto a = TeXFormula(tp, args[2])._root;
  std::string x = wide2utf8(args[1].c_str());
  return sptr<Atom>(new ColorAtom(a, TRANS, ColorAtom::getColor(x)));
}

inline macro(colorbox) {
  std::string x = wide2utf8(args[1].c_str());
  color c = ColorAtom::getColor(x);
  return sptr<Atom>(new FBoxAtom(TeXFormula(tp, args[2])._root, c, c));
}

inline macro(fcolorbox) {
  std::string x = wide2utf8(args[2].c_str());
  color f = ColorAtom::getColor(x);
  std::string y = wide2utf8(args[1].c_str());
  color b = ColorAtom::getColor(y);
  return sptr<Atom>(new FBoxAtom(TeXFormula(tp, args[3])._root, f, b));
}

inline macro(cong) {
  VRowAtom* vra = new VRowAtom(SymbolAtom::get("equals"));
  vra->add(sptr<Atom>(new SpaceAtom(UNIT_MU, 0, 1.5f, 0)));
  vra->add(SymbolAtom::get("sim"));
  vra->setRaise(UNIT_MU, -1);
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(vra)));
}

inline macro(doteq) {
  auto e = SymbolAtom::get("equals");
  auto l = SymbolAtom::get("ldotp");
  sptr<Atom> u(new UnderOverAtom(e, l, UNIT_MU, 3.7f, false, true));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, u));
}

inline macro(externalfont) {
  std::string x = wide2utf8(args[1].c_str());
  TextRenderingBox::setFont(x);
  return nullptr;
}

inline macro(ctext) {
  return sptr<Atom>(new TextRenderingAtom(args[1], PLAIN));
}

inline macro(textit) {
  return sptr<Atom>(new TextRenderingAtom(args[1], ITALIC));
}

inline macro(textbf) {
  return sptr<Atom>(new TextRenderingAtom(args[1], BOLD));
}

inline macro(textitbf) {
  return sptr<Atom>(new TextRenderingAtom(args[1], BOLD | ITALIC));
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
  return sptr<Atom>(new HlineAtom());
}

inline macro(mathcumsup) {
  return sptr<Atom>(new CumulativeScriptsAtom(
      tp.popLastAtom(), nullptr, TeXFormula(tp, args[1])._root));
}

inline macro(mathcumsub) {
  return sptr<Atom>(new CumulativeScriptsAtom(
      tp.popLastAtom(), TeXFormula(tp, args[1])._root, nullptr));
}

inline macro(dotminus) {
  sptr<Atom> a(new UnderOverAtom(
      SymbolAtom::get("minus"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      -3.3f,
      false,
      true));
  return sptr<Atom>(new TypedAtom(TYPE_BINARY_OPERATOR, TYPE_BINARY_OPERATOR, a));
}

inline macro(ratio) {
  sptr<Atom> a(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline macro(geoprop) {
  sptr<RowAtom> ddot(new RowAtom(SymbolAtom::get("normaldot")));
  ddot->add(sptr<Atom>(new SpaceAtom(UNIT_MU, 4, 0, 0)));
  ddot->add(SymbolAtom::get("normaldot"));
  sptr<Atom> a(new UnderOverAtom(
      SymbolAtom::get("minus"),
      ddot,
      UNIT_MU,
      -3.4f,
      false,
      ddot,
      UNIT_MU,
      -3.4f,
      false));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline macro(minuscolon) {
  RowAtom* ra = new RowAtom(SymbolAtom::get("minus"));
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
  sptr<Atom> colon(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  ra->add(colon);
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(minuscoloncolon) {
  RowAtom* ra = new RowAtom(SymbolAtom::get("minus"));
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
  sptr<Atom> colon(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  ra->add(colon);
  ra->add(colon);
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(simcolon) {
  RowAtom* ra = new RowAtom(SymbolAtom::get("sim"));
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
  sptr<Atom> colon(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  ra->add(colon);
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(simcoloncolon) {
  RowAtom* ra = new RowAtom(SymbolAtom::get("sim"));
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
  sptr<Atom> colon(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  ra->add(colon);
  ra->add(colon);
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(approxcolon) {
  RowAtom* ra = new RowAtom(SymbolAtom::get("approx"));
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
  sptr<Atom> colon(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  ra->add(colon);
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(approxcoloncolon) {
  RowAtom* ra = new RowAtom(SymbolAtom::get("approx"));
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
  sptr<Atom> colon(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  ra->add(colon);
  ra->add(colon);
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(equalscolon) {
  RowAtom* ra = new RowAtom(SymbolAtom::get("equals"));
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
  sptr<Atom> colon(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  ra->add(colon);
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(equalscoloncolon) {
  RowAtom* ra = new RowAtom(SymbolAtom::get("equals"));
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
  sptr<Atom> colon(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  ra->add(colon);
  ra->add(colon);
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(colonminus) {
  sptr<Atom> u(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  RowAtom* ra = new RowAtom(u);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
  ra->add(SymbolAtom::get("minus"));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(coloncolonminus) {
  sptr<Atom> u(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  RowAtom* ra = new RowAtom(u);
  ra->add(u);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
  ra->add(SymbolAtom::get("minus"));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(colonequals) {
  sptr<Atom> u(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  RowAtom* ra = new RowAtom(u);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
  ra->add(SymbolAtom::get("equals"));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(coloncolonequals) {
  sptr<Atom> u(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  RowAtom* ra = new RowAtom(u);
  ra->add(u);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
  ra->add(SymbolAtom::get("equals"));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(coloncolon) {
  sptr<Atom> u(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  RowAtom* ra = new RowAtom(u);
  ra->add(u);
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(colonsim) {
  sptr<Atom> u(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  RowAtom* ra = new RowAtom(u);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
  ra->add(SymbolAtom::get("sim"));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(coloncolonsim) {
  sptr<Atom> u(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  RowAtom* ra = new RowAtom(u);
  ra->add(u);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
  ra->add(SymbolAtom::get("sim"));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(colonapprox) {
  sptr<Atom> u(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  RowAtom* ra = new RowAtom(u);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
  ra->add(SymbolAtom::get("approx"));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(coloncolonapprox) {
  sptr<Atom> u(new UnderOverAtom(
      SymbolAtom::get("normaldot"),
      SymbolAtom::get("normaldot"),
      UNIT_MU,
      5.2f,
      false,
      true));
  RowAtom* ra = new RowAtom(u);
  ra->add(u);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
  ra->add(SymbolAtom::get("approx"));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, sptr<Atom>(ra)));
}

inline macro(smallfrowneq) {
  sptr<Atom> u(new UnderOverAtom(
      SymbolAtom::get("equals"),
      SymbolAtom::get("smallfrown"),
      UNIT_MU,
      -2,
      true,
      true));
  return sptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, u));
}

inline macro(hstrok) {
  sptr<RowAtom> ra(new RowAtom(sptr<Atom>(new SpaceAtom(UNIT_EX, -0.1f, 0, 0))));
  ra->add(SymbolAtom::get("bar"));
  VRowAtom* vra = new VRowAtom(sptr<Atom>(new LapedAtom(ra, 'r')));
  vra->setRaise(UNIT_EX, -0.1f);
  RowAtom* a = new RowAtom(sptr<Atom>(vra));
  a->add(sptr<Atom>(new RomanAtom(sptr<Atom>(new CharAtom('h', tp._formula->_textStyle)))));
  return sptr<Atom>(a);
}

inline macro(Hstrok) {
  RowAtom* ra = new RowAtom(sptr<Atom>(new SpaceAtom(UNIT_EX, -0.28f, 0, 0)));
  ra->add(SymbolAtom::get("textendash"));
  VRowAtom* vra = new VRowAtom(sptr<Atom>(new LapedAtom(sptr<Atom>(ra), 'r')));
  vra->setRaise(UNIT_EX, 0.55f);
  RowAtom* a = new RowAtom(sptr<Atom>(vra));
  a->add(sptr<Atom>(new RomanAtom(sptr<Atom>(new CharAtom('H', tp._formula->_textStyle)))));
  return sptr<Atom>(a);
}

inline macro(dstrok) {
  RowAtom* ra = new RowAtom(sptr<Atom>(new SpaceAtom(UNIT_EX, 0.25f, 0, 0)));
  ra->add(SymbolAtom::get("bar"));
  VRowAtom* vra = new VRowAtom(sptr<Atom>(new LapedAtom(sptr<Atom>(ra), 'r')));
  vra->setRaise(UNIT_EX, -0.1f);
  RowAtom* a = new RowAtom(sptr<Atom>(vra));
  a->add(sptr<Atom>(new RomanAtom(sptr<Atom>(new CharAtom('d', tp._formula->_textStyle)))));
  return sptr<Atom>(a);
}

inline macro(Dstrok) {
  RowAtom* ra = new RowAtom(sptr<Atom>(new SpaceAtom(UNIT_EX, -0.1f, 0, 0)));
  ra->add(SymbolAtom::get("bar"));
  VRowAtom* vra = new VRowAtom(sptr<Atom>(new LapedAtom(sptr<Atom>(ra), 'r')));
  vra->setRaise(UNIT_EX, -0.55f);
  RowAtom* a = new RowAtom(sptr<Atom>(vra));
  a->add(sptr<Atom>(new RomanAtom(sptr<Atom>(new CharAtom('D', tp._formula->_textStyle)))));
  return sptr<Atom>(a);
}

inline macro(kern) {
  std::pair<int, float> info = SpaceAtom::getLength(args[1]);

  return sptr<Atom>(new SpaceAtom(info.first, info.second, 0, 0));
}

inline macro(char) {
  std::string x;
  wide2utf8(args[1].c_str(), x);
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
  return tp.convertCharacter((wchar_t)n, true);
}

inline macro(T) {
  return sptr<Atom>(new RotateAtom(TeXFormula(tp, args[1])._root, 180, L"origin=cc"));
}

inline macro(textcircled) {
  return sptr<Atom>(new TextCircledAtom(
      sptr<Atom>(new RomanAtom(TeXFormula(tp, args[1])._root))));
}

inline macro(textsc) {
  return sptr<Atom>(new SmallCpaAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(sc) {
  return sptr<Atom>(new SmallCpaAtom(
      TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline macro(quad) {
  return sptr<Atom>(new SpaceAtom(UNIT_EM, 1, 0, 0));
}

inline macro(surd) {
  return sptr<Atom>(new VCenteredAtom(SymbolAtom::get("surdsign")));
}

inline macro(int) {
  SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
  integral->_typelimits = SCRIPT_NOLIMITS;
  return sptr<Atom>(integral);
}

inline macro(oint) {
  SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("oint")));
  integral->_typelimits = SCRIPT_NOLIMITS;
  return sptr<Atom>(integral);
}

inline macro(iint) {
  SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
  integral->_typelimits = SCRIPT_NOLIMITS;
  sptr<Atom> i(integral);
  RowAtom* ra = new RowAtom(i);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
  ra->add(i);
  ra->_lookAtLastAtom = true;
  return sptr<Atom>(new TypedAtom(TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, sptr<Atom>(ra)));
}

inline macro(iiint) {
  SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
  integral->_typelimits = SCRIPT_NOLIMITS;
  sptr<Atom> i(integral);
  RowAtom* ra = new RowAtom(i);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
  ra->add(i);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
  ra->add(i);
  ra->_lookAtLastAtom = true;
  return sptr<Atom>(new TypedAtom(TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, sptr<Atom>(ra)));
}

inline macro(iiiint) {
  SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
  integral->_typelimits = SCRIPT_NOLIMITS;
  sptr<Atom> i(integral);
  RowAtom* ra = new RowAtom(i);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
  ra->add(i);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
  ra->add(i);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
  ra->add(i);
  ra->_lookAtLastAtom = true;
  return sptr<Atom>(new TypedAtom(TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, sptr<Atom>(ra)));
}

inline macro(idotsint) {
  SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
  integral->_typelimits = SCRIPT_NOLIMITS;
  sptr<Atom> i(integral);
  RowAtom* ra = new RowAtom(i);
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_MU, -1.f, 0, 0)));
  auto cdotp = SymbolAtom::get("cdotp");
  RowAtom* cdots = new RowAtom(cdotp);
  cdots->add(cdotp);
  cdots->add(cdotp);
  ra->add(sptr<Atom>(new TypedAtom(TYPE_INNER, TYPE_INNER, sptr<Atom>(cdots))));
  ra->add(sptr<Atom>(new SpaceAtom(UNIT_MU, -1.f, 0, 0)));
  ra->add(i);
  ra->_lookAtLastAtom = true;
  return sptr<Atom>(new TypedAtom(TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, sptr<Atom>(ra)));
}

inline macro(lmoustache) {
  SymbolAtom* s = new SymbolAtom(*(SymbolAtom::get("lmoustache")));
  sptr<Atom> b(new BigDelimiterAtom(sptr<SymbolAtom>(s), 1));
  b->_type = TYPE_OPENING;
  return b;
}

inline macro(rmoustache) {
  SymbolAtom* s = new SymbolAtom(*(SymbolAtom::get("rmoustache")));
  sptr<Atom> b(new BigDelimiterAtom(sptr<SymbolAtom>(s), 1));
  b->_type = TYPE_CLOSING;
  return b;
}

inline macro(insertBreakMark) {
  return sptr<Atom>(new BreakMarkAtom());
}

/**************************************** limits macros *******************************************/

inline sptr<Atom> _macro_typelimits(_out_ TeXParser& tp, _out_ std::vector<std::wstring>& args, int type) {
  auto atom = tp.popLastAtom();
  auto copy = atom->clone();
  copy->_typelimits = type;
  return copy;
}

inline macro(nolimits) {
  return _macro_typelimits(tp, args, SCRIPT_NOLIMITS);
}

inline macro(limits) {
  return _macro_typelimits(tp, args, SCRIPT_LIMITS);
}

inline macro(normal) {
  return _macro_typelimits(tp, args, SCRIPT_NORMAL);
}

/***************************************** implement at .cpp **************************************/

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
