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

using namespace std;
using namespace tex;

namespace tex {

#ifndef macro
#define macro(name) shared_ptr<Atom> macro_##name(TeXParser& tp, vector<wstring>& args)
#endif

#ifdef __GA_DEBUG
inline macro(debug) {
    TeXFormula::setDEBUG(true);
    return shared_ptr<Atom>(nullptr);
}

inline macro(undebug) {
    TeXFormula::setDEBUG(false);
    return shared_ptr<Atom>(nullptr);
}
#endif  // __GA_DEBUG

inline macro(multirow) {
    if (!tp.isArrayMode())
        throw ex_parse("Command \\multirow must used in array environment!");
    int n = 0;
    valueof(args[1], n);
    tp.addAtom(shared_ptr<Atom>(new MultiRowAtom(n, args[2], TeXFormula(tp, args[3])._root)));
    return shared_ptr<Atom>(nullptr);
}

inline macro(cellcolor) {
    if (!tp.isArrayMode())
        throw ex_parse("Command \\cellcolor must used in array environment!");
    color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
    shared_ptr<CellSpecifier> atom(new CellColorAtom(c));
    ((ArrayOfAtoms*)tp._formula)->addCellSpecifier(atom);
    return shared_ptr<Atom>(nullptr);
}

inline macro(color) {
    // We do not care the \color command in non-array mode, since we did pass a color as a parameter
    // when parsing a LaTeX string, it is useless to specify a global foreground color again, but in
    // array mode, the \color command is useful to specify the foreground color of the columns.
    if (tp.isArrayMode()) {
        color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
        return shared_ptr<Atom>(new CellForegroundAtom(c));
    }
    return shared_ptr<Atom>(nullptr);
}

inline macro(newcolumntype) {
    MatrixAtom::defineColumnSpecifier(args[1], args[2]);
    return shared_ptr<Atom>(nullptr);
}

inline macro(arrayrulecolor) {
    color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
    MatrixAtom::LINE_COLOR = c;
    return shared_ptr<Atom>(nullptr);
}

inline macro(columnbg) {
    color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
    return shared_ptr<Atom>(new CellColorAtom(c));
}

inline macro(rowcolor) {
    if (!tp.isArrayMode())
        throw ex_parse("Command \\rowcolor must used in array environment!");
    color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
    shared_ptr<CellSpecifier> spe(new CellColorAtom(c));
    ((ArrayOfAtoms*)tp._formula)->addRowSpecifier(spe);
    return shared_ptr<Atom>(nullptr);
}

inline macro(st) {
    auto base = TeXFormula(tp, args[1], false)._root;
    return shared_ptr<Atom>(new StrikeThroughAtom(base));
}

inline macro(Braket) {
    wstring str(args[1]);
    replaceall(str, L"\\|", L"\\middle\\vert ");
    return TeXFormula(tp, L"\\left\\langle " + str + L"\\right\\rangle")._root;
}

inline macro(Set) {
    wstring str(args[1]);
    replacefirst(str, L"\\|", L"\\middle\\vert ");
    return TeXFormula(tp, L"\\left\\{" + str + L"\\right\\}")._root;
}

inline macro(spATbreve) {
    VRowAtom* vra = new VRowAtom(TeXFormula(L"\\displaystyle\\!\\breve{}")._root);
    vra->setRaise(UNIT_EX, 0.6f);
    return shared_ptr<Atom>(new SmashedAtom(shared_ptr<Atom>(vra), ""));
}

inline macro(spAThat) {
    VRowAtom* vra = new VRowAtom(TeXFormula(L"\\displaystyle\\widehat{}")._root);
    vra->setRaise(UNIT_EX, 0.6f);
    return shared_ptr<Atom>(new SmashedAtom(shared_ptr<Atom>(vra), ""));
}

inline macro(clrlap) {
    return shared_ptr<Atom>(new LapedAtom(TeXFormula(tp, args[1])._root, args[0][0]));
}

inline macro(mathclrlap) {
    return shared_ptr<Atom>(new LapedAtom(TeXFormula(tp, args[1])._root, args[0][4]));
}

inline macro(frac) {
    TeXFormula num(tp, args[1], false);
    TeXFormula den(tp, args[2], false);
    if (num._root == nullptr || den._root == nullptr)
        throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
    return shared_ptr<Atom>(new FractionAtom(num._root, den._root, true));
}

inline macro(over) {
    auto num = tp.getFormulaAtom();
    auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
    if (num == nullptr || den == nullptr)
        throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
    return shared_ptr<Atom>(new FractionAtom(num, den, true));
}

inline macro(atop) {
    auto num = tp.getFormulaAtom();
    auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
    if (num == nullptr || den == nullptr)
        throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
    return shared_ptr<Atom>(new FractionAtom(num, den, false));
}

inline shared_ptr<Atom> _macro_choose(
    const string& left, const string& right,
    _out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
    auto num = tp.getFormulaAtom();
    auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
    if (num == nullptr || den == nullptr)
        throw ex_parse("Both numerator and denominator of choos can't be empty!");
    shared_ptr<Atom> f(new FractionAtom(num, den, false));
    shared_ptr<SymbolAtom> l(new SymbolAtom(left, TYPE_OPENING, true));
    shared_ptr<SymbolAtom> r(new SymbolAtom(right, TYPE_CLOSING, true));
    return shared_ptr<Atom>(new FencedAtom(f, l, r));
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

inline macro(binom) {
    TeXFormula num(tp, args[1], false);
    TeXFormula den(tp, args[2], false);
    if (num._root == nullptr || den._root == nullptr)
        throw ex_parse("Both binomial coefficients must be not empty!");
    shared_ptr<Atom> f(new FractionAtom(num._root, den._root, false));
    shared_ptr<SymbolAtom> s1(new SymbolAtom("lbrack", TYPE_OPENING, true));
    shared_ptr<SymbolAtom> s2(new SymbolAtom("rbrack", TYPE_CLOSING, true));
    return shared_ptr<Atom>(new FencedAtom(f, s1, s2));
}

inline macro(above) {
    auto num = tp.getFormulaAtom();
    pair<int, float> dim = tp.getLength();
    auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
    if (num == nullptr || den == nullptr)
        throw ex_parse("Both numerator and denominator of a fraction can't be empty!");

    return shared_ptr<Atom>(new FractionAtom(num, den, dim.first, dim.second));
}

inline macro(mbox) {
    shared_ptr<Atom> group(new RomanAtom(TeXFormula(tp, args[1], "mathnormal", false, false)._root));
    return shared_ptr<Atom>(new StyleAtom(STYLE_TEXT, group));
}

inline macro(text) {
    return shared_ptr<Atom>(new RomanAtom(TeXFormula(tp, args[1], "mathnormal", false, false)._root));
}

inline macro(underscore) {
    return shared_ptr<Atom>(new UnderScoreAtom());
}

inline macro(accents) {
    string x;
    wide2utf8(args[0].c_str(), x);
    return shared_ptr<Atom>(new AccentedAtom(TeXFormula(tp, args[1], false)._root, x));
}

inline macro(grkaccent) {
    return shared_ptr<Atom>(new AccentedAtom(
        TeXFormula(tp, args[2], false)._root, TeXFormula(tp, args[1], false)._root, false));
}

inline macro(accent) {
    return shared_ptr<Atom>(new AccentedAtom(
        TeXFormula(tp, args[2], false)._root, TeXFormula(tp, args[1], false)._root));
}

inline macro(cedilla) {
    return shared_ptr<Atom>(new CedillAtom(TeXFormula(tp, args[1])._root));
}

inline macro(IJ) {
    return shared_ptr<Atom>(new IJAtom(args[0][0] == 'I'));
}

inline macro(TStroke) {
    return shared_ptr<Atom>(new TStrokeAtom(args[0][0] == 'T'));
}

inline macro(LCaron) {
    return shared_ptr<Atom>(new LCaronAtom(args[0][0] == 'L'));
}

inline macro(tcaron) {
    return shared_ptr<Atom>(new TCaronAtom());
}

inline macro(ogonek) {
    return shared_ptr<Atom>(new OgonekAtom(TeXFormula(tp, args[1])._root));
}

inline macro(nbsp) {
    return shared_ptr<Atom>(new SpaceAtom());
}

inline macro(sqrt) {
    if (args[2].empty())
        return shared_ptr<Atom>(new NthRoot(
            TeXFormula(tp, args[1], false)._root, shared_ptr<Atom>(nullptr)));
    return shared_ptr<Atom>(new NthRoot(
        TeXFormula(tp, args[1], false)._root, TeXFormula(tp, args[2], false)._root));
}

inline macro(overrightarrow) {
    return shared_ptr<Atom>(new UnderOverArrowAtom(
        TeXFormula(tp, args[1], false)._root, false, true));
}

inline macro(overleftarrow) {
    return shared_ptr<Atom>(new UnderOverArrowAtom(
        TeXFormula(tp, args[1], false)._root, true, true));
}

inline macro(overleftrightarrow) {
    return shared_ptr<Atom>(new UnderOverArrowAtom(
        TeXFormula(tp, args[1], false)._root, true));
}

inline macro(underrightarrow) {
    return shared_ptr<Atom>(new UnderOverArrowAtom(
        TeXFormula(tp, args[1], false)._root, false, false));
}

inline macro(underleftarrow) {
    return shared_ptr<Atom>(new UnderOverArrowAtom(
        TeXFormula(tp, args[1], false)._root, true, false));
}

inline macro(underleftrightarrow) {
    return shared_ptr<Atom>(new UnderOverArrowAtom(
        TeXFormula(tp, args[1], false)._root, false));
}

inline macro(xleftarrow) {
    return shared_ptr<Atom>(new XArrowAtom(
        TeXFormula(tp, args[1], false)._root, TeXFormula(tp, args[2])._root, true));
}

inline macro(xrightarrow) {
    return shared_ptr<Atom>(new XArrowAtom(
        TeXFormula(tp, args[1], false)._root, TeXFormula(tp, args[2])._root, false));
}

inline macro(sideset) {
    TeXFormula tf;
    tf.add(shared_ptr<Atom>(new PhantomAtom(TeXFormula(tp, args[3])._root, false, true, true)));
    tf.append(tp.getIsPartial(), args[1]);
    tf.add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -0.3f, 0, 0)));
    tf.append(tp.getIsPartial(), args[3] + L"\\nolimits" + args[2]);
    return shared_ptr<Atom>(new TypedAtom(TYPE_ORDINARY, TYPE_ORDINARY, tf._root));
}

inline macro(prescript) {
    auto base = TeXFormula(tp, args[3])._root;
    shared_ptr<Atom> p(new PhantomAtom(base, false, true, true));
    shared_ptr<Atom> s(new ScriptsAtom(
        p,
        TeXFormula(tp, args[2])._root, TeXFormula(tp, args[1])._root, false));
    tp.addAtom(s);
    tp.addAtom(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -0.3f, 0, 0)));
    return shared_ptr<Atom>(new TypedAtom(TYPE_ORDINARY, TYPE_ORDINARY, base));
}

inline macro(underbrace) {
    return shared_ptr<Atom>(new OverUnderDelimiter(
        TeXFormula(tp, args[1], false)._root,
        shared_ptr<Atom>(nullptr),
        SymbolAtom::get("rbrace"),
        UNIT_EX, 0, false));
}

inline macro(overbrace) {
    return shared_ptr<Atom>(new OverUnderDelimiter(
        TeXFormula(tp, args[1], false)._root,
        shared_ptr<Atom>(nullptr),
        SymbolAtom::get("lbrace"),
        UNIT_EX, 0, true));
}

inline macro(underbrack) {
    return shared_ptr<Atom>(new OverUnderDelimiter(
        TeXFormula(tp, args[1], false)._root,
        shared_ptr<Atom>(nullptr),
        SymbolAtom::get("rsqbrack"),
        UNIT_EX, 0, false));
}

inline macro(overbrack) {
    return shared_ptr<Atom>(new OverUnderDelimiter(
        TeXFormula(tp, args[1], false)._root,
        shared_ptr<Atom>(nullptr),
        SymbolAtom::get("lsqbrack"),
        UNIT_EX, 0, true));
}

inline macro(underparen) {
    return shared_ptr<Atom>(new OverUnderDelimiter(
        TeXFormula(tp, args[1], false)._root,
        shared_ptr<Atom>(nullptr),
        SymbolAtom::get("rbrack"),
        UNIT_EX, 0, false));
}

inline macro(overparen) {
    return shared_ptr<Atom>(new OverUnderDelimiter(
        TeXFormula(tp, args[1], false)._root,
        shared_ptr<Atom>(nullptr),
        SymbolAtom::get("lbrack"),
        UNIT_EX, 0, true));
}

inline macro(overline) {
    return shared_ptr<Atom>(new OverlinedAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(underline) {
    return shared_ptr<Atom>(new UnderlinedAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(mathop) {
    shared_ptr<TypedAtom> a(new TypedAtom(
        TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, TeXFormula(tp, args[1], false)._root));
    a->_typelimits = SCRIPT_NORMAL;
    return a;
}

inline macro(mathpunct) {
    return shared_ptr<Atom>(new TypedAtom(
        TYPE_PUNCTUATION, TYPE_PUNCTUATION, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathord) {
    return shared_ptr<Atom>(new TypedAtom(
        TYPE_ORDINARY, TYPE_ORDINARY, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathrel) {
    return shared_ptr<Atom>(new TypedAtom(
        TYPE_RELATION, TYPE_RELATION, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathinner) {
    return shared_ptr<Atom>(new TypedAtom(
        TYPE_INNER, TYPE_INNER, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathbin) {
    return shared_ptr<Atom>(new TypedAtom(
        TYPE_BINARY_OPERATOR, TYPE_BINARY_OPERATOR, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathopen) {
    return shared_ptr<Atom>(new TypedAtom(
        TYPE_OPENING, TYPE_OPENING, TeXFormula(tp, args[1], false)._root));
}

inline macro(mathclose) {
    return shared_ptr<Atom>(new TypedAtom(
        TYPE_CLOSING, TYPE_CLOSING, TeXFormula(tp, args[1], false)._root));
}

inline macro(joinrel) {
    return shared_ptr<Atom>(new TypedAtom(
        TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -2.6f, 0, 0))));
}

inline macro(smash) {
    string x;
    wide2utf8(args[2].c_str(), x);
    return shared_ptr<Atom>(new SmashedAtom(TeXFormula(tp, args[1], false)._root, x));
}

inline macro(vdots) {
    return shared_ptr<Atom>(new VdotsAtom());
}

inline macro(ddots) {
    return shared_ptr<Atom>(new TypedAtom(
        TYPE_INNER, TYPE_INNER, shared_ptr<Atom>(new DdtosAtom())));
}

inline macro(iddots) {
    return shared_ptr<Atom>(new TypedAtom(
        TYPE_INNER, TYPE_INNER, shared_ptr<Atom>(new IddotsAtom())));
}

inline macro(leftparenthesis) {
    wstring grp = tp.getGroup(L"\\(", L"\\)");
    return shared_ptr<Atom>(new MathAtom(TeXFormula(tp, grp, false)._root, STYLE_TEXT));
}

inline macro(leftbracket) {
    wstring grp = tp.getGroup(L"\\[", L"\\]");
    return shared_ptr<Atom>(new MathAtom(TeXFormula(tp, grp, false)._root, STYLE_DISPLAY));
}

inline macro(middle) {
    return shared_ptr<Atom>(new MiddleAtom(TeXFormula(tp, args[1])._root));
}

inline macro(cr) {
    if (tp.isArrayMode()) {
        tp.addRow();
    } else {
        ArrayOfAtoms arr;
        arr.add(tp._formula->_root);
        arr.addRow();
        TeXParser parser(
            tp.getIsPartial(), tp.getStringFromCurrentPos(),
            &arr, false, tp.isIgnoreWhiteSpace());
        parser.parse();
        arr.checkDimensions();
        tp.finish();
        tp._formula->_root = arr.getAsVRow();
    }

    return shared_ptr<Atom>(nullptr);
}

inline macro(backslashcr) {
    return macro_cr(tp, args);
}

inline macro(smallmatrixATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser parser(tp.getIsPartial(), args[1], arr, false);
    parser.parse();
    arr->checkDimensions();
    return shared_ptr<Atom>(new MatrixAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), SMALLMATRIX));
}

inline macro(matrixATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser parser(tp.getIsPartial(), args[1], arr, false);
    parser.parse();
    arr->checkDimensions();
    return shared_ptr<Atom>(new MatrixAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), MATRIX));
}

inline macro(multicolumn) {
    int n = 0;
    valueof(args[1], n);
    string x;
    wide2utf8(args[2].c_str(), x);
    tp.addAtom(shared_ptr<Atom>(new MulticolumnAtom(n, x, TeXFormula(tp, args[3])._root)));
    ((ArrayOfAtoms*)tp._formula)->addCol(n);
    return shared_ptr<Atom>(nullptr);
}

inline macro(hdotsfor) {
    int n = 0;
    valueof(args[1], n);
    float f = 1.f;
    if (!args[2].empty()) valueof(args[2], f);
    tp.addAtom(shared_ptr<Atom>(new HdotsforAtom(n, f)));
    ((ArrayOfAtoms*)tp._formula)->addCol(n);
    return shared_ptr<Atom>(nullptr);
}

inline macro(arrayATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser parser(tp.getIsPartial(), args[2], arr, false);
    parser.parse();
    arr->checkDimensions();
    return shared_ptr<Atom>(new MatrixAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), args[1], true));
}

inline macro(alignATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser parser(tp.getIsPartial(), args[1], arr, false);
    parser.parse();
    arr->checkDimensions();
    return shared_ptr<Atom>(new MatrixAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), ALIGN));
}

inline macro(flalignATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser parser(tp.getIsPartial(), args[1], arr, false);
    parser.parse();
    arr->checkDimensions();
    return shared_ptr<Atom>(new MatrixAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), FLALIGN));
}

inline macro(alignatATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser par(tp.getIsPartial(), args[2], arr, false);
    par.parse();
    arr->checkDimensions();
    size_t n = 0;
    valueof(args[1], n);
    if (arr->_col != 2 * n) throw ex_parse("Bad number of equations in alignat environment!");

    return shared_ptr<Atom>(new MatrixAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), ALIGNAT));
}

inline macro(alignedATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser p(tp.getIsPartial(), args[1], arr, false);
    p.parse();
    arr->checkDimensions();
    return shared_ptr<Atom>(new MatrixAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), ALIGNED));
}

inline macro(alignedatATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser p(tp.getIsPartial(), args[2], arr, false);
    p.parse();
    arr->checkDimensions();
    size_t n = 0;
    valueof(args[1], n);
    if (arr->_col != 2 * n)
        throw ex_parse("Bad number of equations in alignedat environment!");

    return shared_ptr<Atom>(new MatrixAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), ALIGNEDAT));
}

inline macro(multlineATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser p(tp.getIsPartial(), args[1], arr, false);
    p.parse();
    arr->checkDimensions();
    if (arr->_col > 1)
        throw ex_parse("Character '&' is only available in array mode!");
    if (arr->_col == 0)
        return shared_ptr<Atom>(nullptr);

    return shared_ptr<Atom>(new MultlineAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), MULTILINE));
}

inline macro(gatherATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser p(tp.getIsPartial(), args[1], arr, false);
    p.parse();
    arr->checkDimensions();
    if (arr->_col > 1) throw ex_parse("Character '&' is only available in array mode!");
    if (arr->_col == 0) return shared_ptr<Atom>(nullptr);

    return shared_ptr<Atom>(new MultlineAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), GATHER));
}

inline macro(gatheredATATenv) {
    ArrayOfAtoms* arr = new ArrayOfAtoms();
    TeXParser p(tp.getIsPartial(), args[1], arr, false);
    p.parse();
    arr->checkDimensions();
    if (arr->_col > 1) throw ex_parse("Character '&' is only available in array mode");
    if (arr->_col == 0) return shared_ptr<Atom>(nullptr);

    return shared_ptr<Atom>(new MultlineAtom(
        tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), GATHERED));
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
    return shared_ptr<Atom>(nullptr);
}

inline macro(makeatother) {
    tp.makeAtOther();
    return shared_ptr<Atom>(nullptr);
}

inline macro(newenvironment) {
    int opt = 0;
    if (!args[4].empty()) valueof(args[4], opt);

    NewEnvironmentMacro::addNewEnvironment(args[1], args[2], args[3], opt);
    return shared_ptr<Atom>(nullptr);
}

inline macro(renewenvironment) {
    int opt = 0;
    if (!args[4].empty()) valueof(args[4], opt);

    NewEnvironmentMacro::addRenewEnvironment(args[1], args[2], args[3], opt);
    return shared_ptr<Atom>(nullptr);
}

inline macro(fbox) {
    return shared_ptr<Atom>(new FBoxAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(questeq) {
    auto eq = SymbolAtom::get(TeXFormula::_symbolMappings['=']);
    auto quest = SymbolAtom::get(TeXFormula::_symbolMappings['?']);
    auto sq = shared_ptr<Atom>(new ScaleAtom(quest, 0.75f));
    auto at = shared_ptr<Atom>(new UnderOverAtom(eq, sq, UNIT_MU, 2.5f, true, true));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, at));
}

inline macro(stackrel) {
    shared_ptr<Atom> a(new UnderOverAtom(
        TeXFormula(tp, args[2], false)._root,
        TeXFormula(tp, args[3], false)._root,
        UNIT_MU, 0.5f, true,
        TeXFormula(tp, args[1], false)._root,
        UNIT_MU, 2.5f, true));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline macro(stackbin) {
    shared_ptr<Atom> a(new UnderOverAtom(
        TeXFormula(tp, args[2], false)._root,
        TeXFormula(tp, args[3], false)._root,
        UNIT_MU, 0.5f, true,
        TeXFormula(tp, args[1], false)._root,
        UNIT_MU, 2.5f, true));
    return shared_ptr<Atom>(new TypedAtom(TYPE_BINARY_OPERATOR, TYPE_BINARY_OPERATOR, a));
}

inline macro(overset) {
    shared_ptr<Atom> a(new UnderOverAtom(
        TeXFormula(tp, args[2], false)._root,
        TeXFormula(tp, args[1], false)._root,
        UNIT_MU, 2.5f, true, true));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline macro(underset) {
    shared_ptr<Atom> a(new UnderOverAtom(
        TeXFormula(tp, args[2], false)._root,
        TeXFormula(tp, args[1], false)._root,
        UNIT_MU, 0.5f, true, false));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline macro(accentset) {
    return shared_ptr<Atom>(new AccentedAtom(
        TeXFormula(tp, args[2], false)._root, TeXFormula(tp, args[1], false)._root));
}

inline macro(underaccent) {
    return shared_ptr<Atom>(new UnderOverAtom(
        TeXFormula(tp, args[2], false)._root,
        TeXFormula(tp, args[1], false)._root,
        UNIT_MU, 0.3f, true, false));
}

inline macro(undertilde) {
    auto a = TeXFormula(tp, args[1], false)._root;
    shared_ptr<PhantomAtom> p(new PhantomAtom(a, true, false, false));
    shared_ptr<AccentedAtom> acc(new AccentedAtom(p, "widetilde"));
    return shared_ptr<Atom>(new UnderOverAtom(a, acc, UNIT_MU, 0.3f, true, false));
}

inline macro(boldsymbol) {
    return shared_ptr<Atom>(new BoldAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(mathrm) {
    return shared_ptr<Atom>(new RomanAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(rm) {
    return shared_ptr<Atom>(new RomanAtom(
        TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline macro(mathbf) {
    return shared_ptr<Atom>(new BoldAtom(
        shared_ptr<Atom>(new RomanAtom(TeXFormula(tp, args[1], false)._root))));
}

inline macro(bf) {
    return shared_ptr<Atom>(new BoldAtom(shared_ptr<Atom>(new RomanAtom(
        TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root))));
}

inline macro(mathtt) {
    return shared_ptr<Atom>(new TtAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(tt) {
    return shared_ptr<Atom>(new TtAtom(
        TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline macro(mathit) {
    return shared_ptr<Atom>(new ItAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(it) {
    return shared_ptr<Atom>(new ItAtom(
        TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline macro(mathsf) {
    return shared_ptr<Atom>(new SsAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(sf) {
    return shared_ptr<Atom>(new SsAtom(
        TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline macro(LaTeX) {
    return shared_ptr<Atom>(new LaTeXAtom());
}

inline macro(hphantom) {
    return shared_ptr<Atom>(
        new PhantomAtom(TeXFormula(tp, args[1], false)._root, true, false, false));
}

inline macro(vphantom) {
    return shared_ptr<Atom>(
        new PhantomAtom(TeXFormula(tp, args[1], false)._root, false, true, true));
}

inline macro(phantom) {
    return shared_ptr<Atom>(
        new PhantomAtom(TeXFormula(tp, args[1], false)._root, true, true, true));
}

inline macro(big) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    return shared_ptr<Atom>(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 1));
}

inline macro(Big) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    return shared_ptr<Atom>(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 2));
}

inline macro(bigg) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    return shared_ptr<Atom>(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 3));
}

inline macro(Bigg) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    return shared_ptr<Atom>(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 4));
}

inline macro(bigl) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 1));
    t->_type = TYPE_OPENING;
    return t;
}

inline macro(Bigl) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 2));
    t->_type = TYPE_OPENING;
    return t;
}

inline macro(biggl) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 3));
    t->_type = TYPE_OPENING;
    return t;
}

inline macro(Biggl) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 4));
    t->_type = TYPE_OPENING;
    return t;
}

inline macro(bigr) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 1));
    t->_type = TYPE_CLOSING;
    return t;
}

inline macro(Bigr) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 2));
    t->_type = TYPE_CLOSING;
    return t;
}

inline macro(biggr) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 3));
    t->_type = TYPE_CLOSING;
    return t;
}

inline macro(Biggr) {
    auto a = TeXFormula(tp, args[1], false)._root;
    SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
    if (s == nullptr) return a;
    shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 4));
    t->_type = TYPE_CLOSING;
    return t;
}

inline macro(displaystyle) {
    auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
    return shared_ptr<Atom>(new StyleAtom(STYLE_DISPLAY, g));
}

inline macro(scriptstyle) {
    auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
    return shared_ptr<Atom>(new StyleAtom(STYLE_SCRIPT, g));
}

inline macro(textstyle) {
    auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
    return shared_ptr<Atom>(new StyleAtom(STYLE_TEXT, g));
}

inline macro(scriptscriptstyle) {
    auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
    return shared_ptr<Atom>(new StyleAtom(STYLE_SCRIPT_SCRIPT, g));
}

inline macro(rotatebox) {
    float x = 0;
    if (!args[1].empty()) valueof(args[1], x);
    return shared_ptr<Atom>(new RotateAtom(TeXFormula(tp, args[2])._root, x, args[3]));
}

inline macro(reflectbox) {
    return shared_ptr<Atom>(new ReflectAtom(TeXFormula(tp, args[1])._root));
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
    return shared_ptr<Atom>(new ScaleAtom(TeXFormula(tp, args[2])._root, sx, sy));
}

inline macro(resizebox) {
    string ws, hs;
    wide2utf8(args[1].c_str(), ws);
    wide2utf8(args[2].c_str(), hs);
    return shared_ptr<Atom>(new ResizeAtom(
        TeXFormula(tp, args[3])._root, ws, hs, ws == "!" || hs == "!"));
}

inline macro(shadowbox) {
    return shared_ptr<Atom>(new ShadowAtom(TeXFormula(tp, args[1])._root));
}

inline macro(ovalbox) {
    return shared_ptr<Atom>(new OvalAtom(TeXFormula(tp, args[1])._root));
}

inline macro(doublebox) {
    return shared_ptr<Atom>(new DoubleFramedAtom(TeXFormula(tp, args[1])._root));
}

inline macro(fgcolor) {
    auto a = TeXFormula(tp, args[2])._root;
    string x = wide2utf8(args[1].c_str());
    return shared_ptr<Atom>(new ColorAtom(a, TRANS, ColorAtom::getColor(x)));
}

inline macro(bgcolor) {
    auto a = TeXFormula(tp, args[2])._root;
    string x = wide2utf8(args[1].c_str());
    return shared_ptr<Atom>(new ColorAtom(a, ColorAtom::getColor(x), TRANS));
}

inline macro(textcolor) {
    auto a = TeXFormula(tp, args[2])._root;
    string x = wide2utf8(args[1].c_str());
    return shared_ptr<Atom>(new ColorAtom(a, TRANS, ColorAtom::getColor(x)));
}

inline macro(colorbox) {
    string x = wide2utf8(args[1].c_str());
    color c = ColorAtom::getColor(x);
    return shared_ptr<Atom>(new FBoxAtom(TeXFormula(tp, args[2])._root, c, c));
}

inline macro(fcolorbox) {
    string x = wide2utf8(args[2].c_str());
    color f = ColorAtom::getColor(x);
    string y = wide2utf8(args[1].c_str());
    color b = ColorAtom::getColor(y);
    return shared_ptr<Atom>(new FBoxAtom(TeXFormula(tp, args[3])._root, f, b));
}

inline macro(cong) {
    VRowAtom* vra = new VRowAtom(SymbolAtom::get("equals"));
    vra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, 0, 1.5f, 0)));
    vra->add(SymbolAtom::get("sim"));
    vra->setRaise(UNIT_MU, -1);
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(vra)));
}

inline macro(doteq) {
    auto e = SymbolAtom::get("equals");
    auto l = SymbolAtom::get("ldotp");
    shared_ptr<Atom> u(new UnderOverAtom(e, l, UNIT_MU, 3.7f, false, true));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, u));
}

inline macro(externalfont) {
    string x = wide2utf8(args[1].c_str());
    TextRenderingBox::setFont(x);
    return shared_ptr<Atom>(nullptr);
}

inline macro(ctext) {
    return shared_ptr<Atom>(new TextRenderingAtom(args[1], PLAIN));
}

inline macro(textit) {
    return shared_ptr<Atom>(new TextRenderingAtom(args[1], ITALIC));
}

inline macro(textbf) {
    return shared_ptr<Atom>(new TextRenderingAtom(args[1], BOLD));
}

inline macro(textitbf) {
    return shared_ptr<Atom>(new TextRenderingAtom(args[1], BOLD | ITALIC));
}

inline macro(declaremathsizes) {
    float a, b, c, d;
    valueof(args[1], a), valueof(args[2], b), valueof(args[3], c), valueof(args[4], d);
    DefaultTeXFont::setMathSizes(a, b, c, c);
    return shared_ptr<Atom>(nullptr);
}

inline macro(magnification) {
    float x;
    valueof(args[1], x);
    DefaultTeXFont::setMagnification(x);
    return shared_ptr<Atom>(nullptr);
}

inline macro(hline) {
    if (!tp.isArrayMode())
        throw ex_parse("The macro \\hline only available in array mode!");
    return shared_ptr<Atom>(new HlineAtom());
}

inline macro(mathcumsup) {
    return shared_ptr<Atom>(new CumulativeScriptsAtom(
        tp.getLastAtom(), shared_ptr<Atom>(nullptr), TeXFormula(tp, args[1])._root));
}

inline macro(mathcumsub) {
    return shared_ptr<Atom>(new CumulativeScriptsAtom(
        tp.getLastAtom(), TeXFormula(tp, args[1])._root, shared_ptr<Atom>(nullptr)));
}

inline macro(dotminus) {
    shared_ptr<Atom> a(new UnderOverAtom(
        SymbolAtom::get("minus"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, -3.3f, false, true));
    return shared_ptr<Atom>(new TypedAtom(TYPE_BINARY_OPERATOR, TYPE_BINARY_OPERATOR, a));
}

inline macro(ratio) {
    shared_ptr<Atom> a(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline macro(geoprop) {
    shared_ptr<RowAtom> ddot(new RowAtom(SymbolAtom::get("normaldot")));
    ddot->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, 4, 0, 0)));
    ddot->add(SymbolAtom::get("normaldot"));
    shared_ptr<Atom> a(new UnderOverAtom(
        SymbolAtom::get("minus"),
        ddot, UNIT_MU, -3.4f, false, ddot, UNIT_MU, -3.4f, false));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline macro(minuscolon) {
    RowAtom* ra = new RowAtom(SymbolAtom::get("minus"));
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
    shared_ptr<Atom> colon(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    ra->add(colon);
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(minuscoloncolon) {
    RowAtom* ra = new RowAtom(SymbolAtom::get("minus"));
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
    shared_ptr<Atom> colon(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    ra->add(colon);
    ra->add(colon);
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(simcolon) {
    RowAtom* ra = new RowAtom(SymbolAtom::get("sim"));
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
    shared_ptr<Atom> colon(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    ra->add(colon);
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(simcoloncolon) {
    RowAtom* ra = new RowAtom(SymbolAtom::get("sim"));
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
    shared_ptr<Atom> colon(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    ra->add(colon);
    ra->add(colon);
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(approxcolon) {
    RowAtom* ra = new RowAtom(SymbolAtom::get("approx"));
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
    shared_ptr<Atom> colon(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    ra->add(colon);
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(approxcoloncolon) {
    RowAtom* ra = new RowAtom(SymbolAtom::get("approx"));
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
    shared_ptr<Atom> colon(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    ra->add(colon);
    ra->add(colon);
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(equalscolon) {
    RowAtom* ra = new RowAtom(SymbolAtom::get("equals"));
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
    shared_ptr<Atom> colon(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    ra->add(colon);
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(equalscoloncolon) {
    RowAtom* ra = new RowAtom(SymbolAtom::get("equals"));
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
    shared_ptr<Atom> colon(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    ra->add(colon);
    ra->add(colon);
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(colonminus) {
    shared_ptr<Atom> u(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    RowAtom* ra = new RowAtom(u);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
    ra->add(SymbolAtom::get("minus"));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(coloncolonminus) {
    shared_ptr<Atom> u(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    RowAtom* ra = new RowAtom(u);
    ra->add(u);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
    ra->add(SymbolAtom::get("minus"));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(colonequals) {
    shared_ptr<Atom> u(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    RowAtom* ra = new RowAtom(u);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
    ra->add(SymbolAtom::get("equals"));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(coloncolonequals) {
    shared_ptr<Atom> u(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    RowAtom* ra = new RowAtom(u);
    ra->add(u);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
    ra->add(SymbolAtom::get("equals"));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(coloncolon) {
    shared_ptr<Atom> u(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    RowAtom* ra = new RowAtom(u);
    ra->add(u);
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(colonsim) {
    shared_ptr<Atom> u(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    RowAtom* ra = new RowAtom(u);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
    ra->add(SymbolAtom::get("sim"));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(coloncolonsim) {
    shared_ptr<Atom> u(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    RowAtom* ra = new RowAtom(u);
    ra->add(u);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
    ra->add(SymbolAtom::get("sim"));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(colonapprox) {
    shared_ptr<Atom> u(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    RowAtom* ra = new RowAtom(u);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
    ra->add(SymbolAtom::get("approx"));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(coloncolonapprox) {
    shared_ptr<Atom> u(new UnderOverAtom(
        SymbolAtom::get("normaldot"),
        SymbolAtom::get("normaldot"),
        UNIT_MU, 5.2f, false, true));
    RowAtom* ra = new RowAtom(u);
    ra->add(u);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
    ra->add(SymbolAtom::get("approx"));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline macro(smallfrowneq) {
    shared_ptr<Atom> u(new UnderOverAtom(
        SymbolAtom::get("equals"),
        SymbolAtom::get("smallfrown"),
        UNIT_MU, -2, true, true));
    return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, u));
}

inline macro(hstrok) {
    shared_ptr<RowAtom> ra(new RowAtom(shared_ptr<Atom>(new SpaceAtom(UNIT_EX, -0.1f, 0, 0))));
    ra->add(SymbolAtom::get("bar"));
    VRowAtom* vra = new VRowAtom(shared_ptr<Atom>(new LapedAtom(ra, 'r')));
    vra->setRaise(UNIT_EX, -0.1f);
    RowAtom* a = new RowAtom(shared_ptr<Atom>(vra));
    a->add(shared_ptr<Atom>(new RomanAtom(shared_ptr<Atom>(new CharAtom('h', tp._formula->_textStyle)))));
    return shared_ptr<Atom>(a);
}

inline macro(Hstrok) {
    RowAtom* ra = new RowAtom(shared_ptr<Atom>(new SpaceAtom(UNIT_EX, -0.28f, 0, 0)));
    ra->add(SymbolAtom::get("textendash"));
    VRowAtom* vra = new VRowAtom(shared_ptr<Atom>(new LapedAtom(shared_ptr<Atom>(ra), 'r')));
    vra->setRaise(UNIT_EX, 0.55f);
    RowAtom* a = new RowAtom(shared_ptr<Atom>(vra));
    a->add(shared_ptr<Atom>(new RomanAtom(shared_ptr<Atom>(new CharAtom('H', tp._formula->_textStyle)))));
    return shared_ptr<Atom>(a);
}

inline macro(dstrok) {
    RowAtom* ra = new RowAtom(shared_ptr<Atom>(new SpaceAtom(UNIT_EX, 0.25f, 0, 0)));
    ra->add(SymbolAtom::get("bar"));
    VRowAtom* vra = new VRowAtom(shared_ptr<Atom>(new LapedAtom(shared_ptr<Atom>(ra), 'r')));
    vra->setRaise(UNIT_EX, -0.1f);
    RowAtom* a = new RowAtom(shared_ptr<Atom>(vra));
    a->add(shared_ptr<Atom>(new RomanAtom(shared_ptr<Atom>(new CharAtom('d', tp._formula->_textStyle)))));
    return shared_ptr<Atom>(a);
}

inline macro(Dstrok) {
    RowAtom* ra = new RowAtom(shared_ptr<Atom>(new SpaceAtom(UNIT_EX, -0.1f, 0, 0)));
    ra->add(SymbolAtom::get("bar"));
    VRowAtom* vra = new VRowAtom(shared_ptr<Atom>(new LapedAtom(shared_ptr<Atom>(ra), 'r')));
    vra->setRaise(UNIT_EX, -0.55f);
    RowAtom* a = new RowAtom(shared_ptr<Atom>(vra));
    a->add(shared_ptr<Atom>(new RomanAtom(shared_ptr<Atom>(new CharAtom('D', tp._formula->_textStyle)))));
    return shared_ptr<Atom>(a);
}

inline macro(kern) {
    pair<int, float> info = SpaceAtom::getLength(args[1]);

    return shared_ptr<Atom>(new SpaceAtom(info.first, info.second, 0, 0));
}

inline macro(char) {
    string x;
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
    return shared_ptr<Atom>(new RotateAtom(TeXFormula(tp, args[1])._root, 180, L"origin=cc"));
}

inline macro(textcircled) {
    return shared_ptr<Atom>(new TextCircledAtom(
        shared_ptr<Atom>(new RomanAtom(TeXFormula(tp, args[1])._root))));
}

inline macro(textsc) {
    return shared_ptr<Atom>(new SmallCpaAtom(TeXFormula(tp, args[1], false)._root));
}

inline macro(sc) {
    return shared_ptr<Atom>(new SmallCpaAtom(
        TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline macro(quad) {
    return shared_ptr<Atom>(new SpaceAtom(UNIT_EM, 1, 0, 0));
}

inline macro(surd) {
    return shared_ptr<Atom>(new VCenteredAtom(SymbolAtom::get("surdsign")));
}

inline macro(int) {
    SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
    integral->_typelimits = SCRIPT_NOLIMITS;
    return shared_ptr<Atom>(integral);
}

inline macro(oint) {
    SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("oint")));
    integral->_typelimits = SCRIPT_NOLIMITS;
    return shared_ptr<Atom>(integral);
}

inline macro(iint) {
    SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
    integral->_typelimits = SCRIPT_NOLIMITS;
    shared_ptr<Atom> i(integral);
    RowAtom* ra = new RowAtom(i);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
    ra->add(i);
    ra->_lookAtLastAtom = true;
    return shared_ptr<Atom>(new TypedAtom(TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, shared_ptr<Atom>(ra)));
}

inline macro(iiint) {
    SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
    integral->_typelimits = SCRIPT_NOLIMITS;
    shared_ptr<Atom> i(integral);
    RowAtom* ra = new RowAtom(i);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
    ra->add(i);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
    ra->add(i);
    ra->_lookAtLastAtom = true;
    return shared_ptr<Atom>(new TypedAtom(TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, shared_ptr<Atom>(ra)));
}

inline macro(iiiint) {
    SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
    integral->_typelimits = SCRIPT_NOLIMITS;
    shared_ptr<Atom> i(integral);
    RowAtom* ra = new RowAtom(i);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
    ra->add(i);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
    ra->add(i);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
    ra->add(i);
    ra->_lookAtLastAtom = true;
    return shared_ptr<Atom>(new TypedAtom(TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, shared_ptr<Atom>(ra)));
}

inline macro(idotsint) {
    SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
    integral->_typelimits = SCRIPT_NOLIMITS;
    shared_ptr<Atom> i(integral);
    RowAtom* ra = new RowAtom(i);
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -1.f, 0, 0)));
    auto cdotp = SymbolAtom::get("cdotp");
    RowAtom* cdots = new RowAtom(cdotp);
    cdots->add(cdotp);
    cdots->add(cdotp);
    ra->add(shared_ptr<Atom>(new TypedAtom(TYPE_INNER, TYPE_INNER, shared_ptr<Atom>(cdots))));
    ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -1.f, 0, 0)));
    ra->add(i);
    ra->_lookAtLastAtom = true;
    return shared_ptr<Atom>(new TypedAtom(TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, shared_ptr<Atom>(ra)));
}

inline macro(lmoustache) {
    SymbolAtom* s = new SymbolAtom(*(SymbolAtom::get("lmoustache")));
    shared_ptr<Atom> b(new BigDelimiterAtom(shared_ptr<SymbolAtom>(s), 1));
    b->_type = TYPE_OPENING;
    return b;
}

inline macro(rmoustache) {
    SymbolAtom* s = new SymbolAtom(*(SymbolAtom::get("rmoustache")));
    shared_ptr<Atom> b(new BigDelimiterAtom(shared_ptr<SymbolAtom>(s), 1));
    b->_type = TYPE_CLOSING;
    return b;
}

inline macro(insertBreakMark) {
    return shared_ptr<Atom>(new BreakMarkAtom());
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

/*************************************** should be fixed ******************************************/

inline macro(nolimits) {
    auto atom = tp.getLastAtom();
    atom->_typelimits = SCRIPT_NOLIMITS;
    return atom;
}

inline macro(limits) {
    auto atom = tp.getLastAtom();
    atom->_typelimits = SCRIPT_LIMITS;
    return atom;
}

inline macro(normal) {
    auto atom = tp.getLastAtom();
    atom->_typelimits = SCRIPT_NORMAL;
    return atom;
}

/******************************************  not implemented **************************************/

inline macro(includegraphics) {
    return shared_ptr<Atom>(nullptr);
}

inline macro(fcscore) {
    return shared_ptr<Atom>(nullptr);
}

inline macro(GeoGebra) {
    return shared_ptr<Atom>(nullptr);
}

inline macro(dynamic) {
    return shared_ptr<Atom>(nullptr);
}

}  // namespace tex

#endif  // MACRO_IMPL_H_INCLUDED
