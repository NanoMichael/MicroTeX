#ifndef MACRO_IMPL_H_INCLUDED
#define MACRO_IMPL_H_INCLUDED

#if defined (__clang__)
#include "atom_basic.h"
#include "core.h"
#include "formula.h"
#include "parser.h"
#include "macro.h"
#include "alphabet.h"
#include "port.h"
#include "atom_impl.h"
#elif defined (__GNUC__)
#include "atom/atom_basic.h"
#include "core/core.h"
#include "core/formula.h"
#include "core/parser.h"
#include "core/macro.h"
#include "fonts/alphabet.h"
#include "port/port.h"
#include "atom/atom_impl.h"
#endif // defined
#include "common.h"

#include "memcheck.h"

using namespace std;
using namespace tex;
using namespace tex::port;
using namespace tex::core;

namespace tex {
namespace core {

#ifdef __GA_DEBUG
inline shared_ptr<Atom> debug_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	TeXFormula::setDEBUG(true);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> undebug_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	TeXFormula::setDEBUG(false);
	return shared_ptr<Atom>(nullptr);
}
#endif // __GA_DEBUG

inline shared_ptr<Atom> multirow_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	if (!tp.isArrayMode())
		throw ex_parse("command \\multirow must used in array environment!");
	int n = 0;
	valueof(args[1], n);
	tp.addAtom(shared_ptr<Atom>(new MultiRowAtom(n, args[2], TeXFormula(tp, args[3])._root)));
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> cellcolor_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	if (!tp.isArrayMode())
		throw ex_parse("command \\cellcolor must used in array environment!");
	color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
	shared_ptr<CellSpecifier> atom(new CellColorAtom(c));
	((ArrayOfAtoms*) tp._formula)->addCellSpecifier(atom);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> color_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	// we do not care the \color command in non-array mode, since we did pass a color as a parameter
	// when parsing a LaTeX string, it is useless to specify a global foreground color again, but in
	// array mode, the \color command is useful to specify column foreground
	if (tp.isArrayMode()) {
		color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
		return shared_ptr<Atom>(new CellForegroundAtom(c));
	}
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> newcolumntype_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	MatrixAtom::defineColumnSpecifier(args[1], args[2]);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> arrayrulecolor_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
	MatrixAtom::LINE_COLOR = c;
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> columnbg_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
	return shared_ptr<Atom>(new CellColorAtom(c));
}

inline shared_ptr<Atom> rowcolor_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	if (!tp.isArrayMode())
		throw ex_parse("command \\rowcolor must used in array environment!");
	color c = ColorAtom::getColor(wide2utf8(args[1].c_str()));
	shared_ptr<CellSpecifier> spe(new CellColorAtom(c));
	((ArrayOfAtoms*) tp._formula)->addRowSpecifier(spe);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> st_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto base = TeXFormula(tp, args[1], false)._root;
	return shared_ptr<Atom>(new StrikeThroughAtom(base));
}

inline shared_ptr<Atom> Braket_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	wstring str(args[1]);
	replaceall(str, L"\\|", L"\\middle\\vert ");
	return TeXFormula(tp, L"\\left\\langle " + str + L"\\right\\rangle")._root;
}

inline shared_ptr<Atom> Set_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	wstring str(args[1]);
	replacefirst(str, L"\\|", L"\\middle\\vert ");
	return TeXFormula(tp, L"\\left\\{" + str + L"\\right\\}")._root;
}

inline shared_ptr<Atom> spATbreve_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	VRowAtom* vra = new VRowAtom(TeXFormula(L"\\displaystyle\\!\\breve{}")._root);
	vra->setRaise(UNIT_EX, 0.6f);
	return shared_ptr<Atom>(new SmashedAtom(shared_ptr<Atom>(vra), ""));
}

inline shared_ptr<Atom> spAThat_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	VRowAtom* vra = new VRowAtom(TeXFormula(L"\\displaystyle\\widehat{}")._root);
	vra->setRaise(UNIT_EX, 0.6f);
	return shared_ptr<Atom>(new SmashedAtom(shared_ptr<Atom>(vra), ""));
}

inline shared_ptr<Atom> clrlap_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new LapedAtom(TeXFormula(tp, args[1])._root, args[0][0]));
}

inline shared_ptr<Atom> mathclrlap_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new LapedAtom(TeXFormula(tp, args[1])._root, args[0][4]));
}

inline shared_ptr<Atom> frac_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	TeXFormula num(tp, args[1], false);
	TeXFormula den(tp, args[2], false);
	if (num._root == nullptr || den._root == nullptr)
		throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
	return shared_ptr<Atom>(new FractionAtom(num._root, den._root, true));
}

inline shared_ptr<Atom> over_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto num = tp.getFormulaAtom();
	auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
	if (num == nullptr || den == nullptr)
		throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
	return shared_ptr<Atom>(new FractionAtom(num, den, true));
}

inline shared_ptr<Atom> atop_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto num = tp.getFormulaAtom();
	auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
	if (num == nullptr || den == nullptr)
		throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
	return shared_ptr<Atom>(new FractionAtom(num, den, false));
}

inline shared_ptr<Atom> choose_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto num = tp.getFormulaAtom();
	auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
	if (num == nullptr || den == nullptr)
		throw ex_parse("Both numerator and denominator of choose can't be empty!");
	shared_ptr<Atom> f(new FractionAtom(num, den, false));
	shared_ptr<SymbolAtom> s1(new SymbolAtom("lbrack", TYPE_OPENING, true));
	shared_ptr<SymbolAtom> s2(new SymbolAtom("rbrack", TYPE_CLOSING, true));
	return shared_ptr<Atom>(new FencedAtom(f, s1, s2));
}

inline shared_ptr<Atom> binom_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	TeXFormula num(tp, args[1], false);
	TeXFormula den(tp, args[2], false);
	if (num._root == nullptr || den._root == nullptr)
		throw ex_parse("Both binomial coefficients must be not empty!");
	shared_ptr<Atom> f(new FractionAtom(num._root, den._root, false));
	shared_ptr<SymbolAtom> s1(new SymbolAtom("lbrack", TYPE_OPENING, true));
	shared_ptr<SymbolAtom> s2(new SymbolAtom("rbrack", TYPE_CLOSING, true));
	return shared_ptr<Atom>(new FencedAtom(f, s1, s2));
}

inline shared_ptr<Atom> above_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto num = tp.getFormulaAtom();
	pair<int, float> dim = tp.getLength();
	auto den = TeXFormula(tp, tp.getOverArgument(), false)._root;
	if (num == nullptr || den == nullptr)
		throw ex_parse("Both numerator and denominator of a fraction can't be empty!");

	return shared_ptr<Atom>(new FractionAtom(num, den, dim.first, dim.second));
}

inline shared_ptr<Atom> mbox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> group(new RomanAtom(TeXFormula(tp, args[1], "mathnormal", false, false)._root));
	return shared_ptr<Atom>(new StyleAtom(STYLE_TEXT, group));
}

inline shared_ptr<Atom> text_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new RomanAtom(TeXFormula(tp, args[1], "mathnormal", false, false)._root));
}

inline shared_ptr<Atom> underscore_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new UnderScoreAtom());
}

inline shared_ptr<Atom> accent_macros(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	string x;
	wide2utf8(args[0].c_str(), x);
	return shared_ptr<Atom>(new AccentedAtom(TeXFormula(tp, args[1], false)._root, x));
}

inline shared_ptr<Atom> grkaccent_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new AccentedAtom(TeXFormula(tp, args[2], false)._root, TeXFormula(tp, args[1], false)._root, false));
}

inline shared_ptr<Atom> accent_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new AccentedAtom(TeXFormula(tp, args[2], false)._root, TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> cedilla_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new CedillAtom(TeXFormula(tp, args[1])._root));
}

inline shared_ptr<Atom> IJ_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new IJAtom(args[0][0] == 'I'));
}

inline shared_ptr<Atom> TStroke_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TStrokeAtom(args[0][0] == 'T'));
}

inline shared_ptr<Atom> LCaron_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new LCaronAtom(args[0][0] == 'L'));
}

inline shared_ptr<Atom> tcaron_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TCaronAtom());
}

inline shared_ptr<Atom> ogonek_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new OgonekAtom(TeXFormula(tp, args[1])._root));
}

inline shared_ptr<Atom> nbsp_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new SpaceAtom());
}

inline shared_ptr<Atom> sqrt_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	if (args[2].empty())
		return shared_ptr<Atom>(new NthRoot(TeXFormula(tp, args[1], false)._root, shared_ptr<Atom>(nullptr)));
	return shared_ptr<Atom>(new NthRoot(TeXFormula(tp, args[1], false)._root, TeXFormula(tp, args[2], false)._root));
}

inline shared_ptr<Atom> overrightarrow_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, false, true));
}

inline shared_ptr<Atom> overleftarrow_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, true, true));
}

inline shared_ptr<Atom> overleftrightarrow_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, true));
}

inline shared_ptr<Atom> underrightarrow_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, false, false));
}

inline shared_ptr<Atom> underleftarrow_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, true, false));
}

inline shared_ptr<Atom> underleftrightarrow_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new UnderOverArrowAtom(TeXFormula(tp, args[1], false)._root, false));
}

inline shared_ptr<Atom> xleftarrow_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new XArrowAtom(TeXFormula(tp, args[1], false)._root, TeXFormula(tp, args[2])._root, true));
}

inline shared_ptr<Atom> xrightarrow_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new XArrowAtom(TeXFormula(tp, args[1], false)._root, TeXFormula(tp, args[2])._root, false));
}

inline shared_ptr<Atom> sideset_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	TeXFormula tf;
	tf.add(shared_ptr<Atom>(new PhantomAtom(TeXFormula(tp, args[3])._root, false, true, true)));
	tf.append(tp.getIsPartial(), args[1]);
	tf.add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -0.3f, 0, 0)));
	tf.append(tp.getIsPartial(), args[3] + L"\\nolimits" + args[2]);
	return shared_ptr<Atom>(new TypedAtom(TYPE_ORDINARY, TYPE_ORDINARY, tf._root));
}

inline shared_ptr<Atom> prescript_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto base = TeXFormula(tp, args[3])._root;
	shared_ptr<Atom> p(new PhantomAtom(base, false, true, true));
	shared_ptr<Atom> s(new ScriptsAtom(p, TeXFormula(tp, args[2])._root, TeXFormula(tp, args[1])._root, false));
	tp.addAtom(s);
	tp.addAtom(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -0.3f, 0, 0)));
	return shared_ptr<Atom>(new TypedAtom(TYPE_ORDINARY, TYPE_ORDINARY, base));
}

inline shared_ptr<Atom> underbrace_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new OverUnderDelimiter(TeXFormula(tp, args[1], false)._root, shared_ptr<Atom>(nullptr), SymbolAtom::get("rbrace"), UNIT_EX, 0, false));
}

inline shared_ptr<Atom> overbrace_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new OverUnderDelimiter(TeXFormula(tp, args[1], false)._root, shared_ptr<Atom>(nullptr), SymbolAtom::get("lbrace"), UNIT_EX, 0, true));
}

inline shared_ptr<Atom> underbrack_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new OverUnderDelimiter(TeXFormula(tp, args[1], false)._root, shared_ptr<Atom>(nullptr), SymbolAtom::get("rsqbrack"), UNIT_EX, 0, false));
}

inline shared_ptr<Atom> overbrack_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new OverUnderDelimiter(TeXFormula(tp, args[1], false)._root, shared_ptr<Atom>(nullptr), SymbolAtom::get("lsqbrack"), UNIT_EX, 0, true));
}

inline shared_ptr<Atom> underparen_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new OverUnderDelimiter(TeXFormula(tp, args[1], false)._root, shared_ptr<Atom>(nullptr), SymbolAtom::get("rbrack"), UNIT_EX, 0, false));
}

inline shared_ptr<Atom> overparen_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new OverUnderDelimiter(TeXFormula(tp, args[1], false)._root, shared_ptr<Atom>(nullptr), SymbolAtom::get("lbrack"), UNIT_EX, 0, true));
}

inline shared_ptr<Atom> overline_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new OverlinedAtom(TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> underline_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new UnderlinedAtom(TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> mathop_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<TypedAtom> a(new TypedAtom(TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, TeXFormula(tp, args[1], false)._root));
	a->_typelimits = SCRIPT_NORMAL;
	return a;
}

inline shared_ptr<Atom> mathpunct_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TypedAtom(TYPE_PUNCTUATION, TYPE_PUNCTUATION, TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> mathord_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TypedAtom(TYPE_ORDINARY, TYPE_ORDINARY, TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> mathrel_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> mathinner_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TypedAtom(TYPE_INNER, TYPE_INNER, TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> mathbin_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TypedAtom(TYPE_BINARY_OPERATOR, TYPE_BINARY_OPERATOR, TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> mathopen_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TypedAtom(TYPE_OPENING, TYPE_OPENING, TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> mathclose_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TypedAtom(TYPE_CLOSING, TYPE_CLOSING, TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> joinrel_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -2.6f, 0, 0))));
}

inline shared_ptr<Atom> smash_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	string x;
	wide2utf8(args[2].c_str(), x);
	return shared_ptr<Atom>(new SmashedAtom(TeXFormula(tp, args[1], false)._root, x));
}

inline shared_ptr<Atom> vdots_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new VdotsAtom());
}

inline shared_ptr<Atom> ddots_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TypedAtom(TYPE_INNER, TYPE_INNER, shared_ptr<Atom>(new DdtosAtom())));
}

inline shared_ptr<Atom> iddots_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TypedAtom(TYPE_INNER, TYPE_INNER, shared_ptr<Atom>(new IddotsAtom())));
}

inline shared_ptr<Atom> leftparenthesis_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	wstring grp = tp.getGroup(L"\\(", L"\\)");
	return shared_ptr<Atom>(new MathAtom(TeXFormula(tp, grp, false)._root, STYLE_TEXT));
}

inline shared_ptr<Atom> leftbracket_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	wstring grp = tp.getGroup(L"\\[", L"\\]");
	return shared_ptr<Atom>(new MathAtom(TeXFormula(tp, grp, false)._root, STYLE_DISPLAY));
}

inline shared_ptr<Atom> middle_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new MiddleAtom(TeXFormula(tp, args[1])._root));
}

inline shared_ptr<Atom> cr_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	if (tp.isArrayMode()) {
		tp.addRow();
	} else {
		ArrayOfAtoms arr;
		arr.add(tp._formula->_root);
		arr.addRow();
		TeXParser parser(tp.getIsPartial(), tp.getStringFromCurrentPos(), &arr, false, tp.isIgnoreWhiteSpace());
		parser.parse();
		arr.checkDimensions();
		tp.finish();
		tp._formula->_root = arr.getAsVRow();
	}

	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> backslashcr_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return cr_macro(tp, args);
}

inline shared_ptr<Atom> smallmatrixATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser parser(tp.getIsPartial(), args[1], arr, false);
	parser.parse();
	arr->checkDimensions();
	return shared_ptr<Atom>(new MatrixAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), SMALLMATRIX));
}

inline shared_ptr<Atom> matrixATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser parser(tp.getIsPartial(), args[1], arr, false);
	parser.parse();
	arr->checkDimensions();
	return shared_ptr<Atom>(new MatrixAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), MATRIX));
}

inline shared_ptr<Atom> multicolumn_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	int n = 0;
	valueof(args[1], n);
	string x;
	wide2utf8(args[2].c_str(), x);
	tp.addAtom(shared_ptr<Atom>(new MulticolumnAtom(n, x, TeXFormula(tp, args[3])._root)));
	((ArrayOfAtoms*)tp._formula)->addCol(n);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> hdotsfor_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	int n = 0;
	valueof(args[1], n);
	float f = 1.f;
	if (!args[2].empty())
		valueof(args[2], f);
	tp.addAtom(shared_ptr<Atom>(new HdotsforAtom(n, f)));
	((ArrayOfAtoms*)tp._formula)->addCol(n);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> arrayATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser parser(tp.getIsPartial(), args[2], arr, false);
	parser.parse();
	arr->checkDimensions();
	return shared_ptr<Atom>(new MatrixAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), args[1], true));
}

inline shared_ptr<Atom> alignATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser parser(tp.getIsPartial(), args[1], arr, false);
	parser.parse();
	arr->checkDimensions();
	return shared_ptr<Atom>(new MatrixAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), ALIGN));
}

inline shared_ptr<Atom> flalignATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser parser(tp.getIsPartial(), args[1], arr, false);
	parser.parse();
	arr->checkDimensions();
	return shared_ptr<Atom>(new MatrixAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), FLALIGN));
}

inline shared_ptr<Atom> alignatATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser par(tp.getIsPartial(), args[2], arr, false);
	par.parse();
	arr->checkDimensions();
	size_t n = 0;
	valueof(args[1], n);
	if (arr->_col != 2 * n)
		throw ex_parse("Bad number of equations in alignat environment!");

	return shared_ptr<Atom>(new MatrixAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), ALIGNAT));
}

inline shared_ptr<Atom> alignedATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser p(tp.getIsPartial(), args[1], arr, false);
	p.parse();
	arr->checkDimensions();
	return shared_ptr<Atom>(new MatrixAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), ALIGNED));
}

inline shared_ptr<Atom> alignedatATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser p(tp.getIsPartial(), args[2], arr, false);
	p.parse();
	arr->checkDimensions();
	size_t n = 0;
	valueof(args[1], n);
	if (arr->_col != 2 * n)
		throw ex_parse("Bad number of equations in alignedat environment!");

	return shared_ptr<Atom>(new MatrixAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), ALIGNEDAT));
}

inline shared_ptr<Atom> multlineATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser p(tp.getIsPartial(), args[1], arr, false);
	p.parse();
	arr->checkDimensions();
	if (arr->_col > 1)
		throw ex_parse("character '&' is only available in array mode!");
	if (arr->_col == 0)
		return shared_ptr<Atom>(nullptr);

	return shared_ptr<Atom>(new MultlineAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), MULTILINE));
}

inline shared_ptr<Atom> gatherATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser p(tp.getIsPartial(), args[1], arr, false);
	p.parse();
	arr->checkDimensions();
	if (arr->_col > 1)
		throw ex_parse("character '&' is only available in array mode!");
	if (arr->_col == 0)
		return shared_ptr<Atom>(nullptr);

	return shared_ptr<Atom>(new MultlineAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), GATHER));
}

inline shared_ptr<Atom> gatheredATATenv_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	ArrayOfAtoms* arr = new ArrayOfAtoms();
	TeXParser p(tp.getIsPartial(), args[1], arr, false);
	p.parse();
	arr->checkDimensions();
	if (arr->_col > 1)
		throw ex_parse("character '&' is only available in array mode");
	if (arr->_col == 0)
		return shared_ptr<Atom>(nullptr);

	return shared_ptr<Atom>(new MultlineAtom(tp.getIsPartial(), shared_ptr<ArrayOfAtoms>(arr), GATHERED));
}

inline shared_ptr<Atom> shoveright_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1])._root;
	a->_alignment = ALIGN_RIGHT;
	return a;
}

inline shared_ptr<Atom> shoveleft_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1])._root;
	a->_alignment = ALIGN_LEFT;
	return a;
}

inline shared_ptr<Atom> makeatletter_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	tp.makeAtLetter();
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> makeatother_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	tp.makeAtOther();
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> newenvironment_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	int opt = 0;
	if(!args[4].empty())
		valueof(args[4], opt);

	NewEnvironmentMacro::addNewEnvironment(args[1], args[2], args[3], opt);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> renewenvironment_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	int opt = 0;
	if (!args[4].empty())
		valueof(args[4], opt);

	NewEnvironmentMacro::addRenewEnvironment(args[1], args[2], args[3], opt);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> fbox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new FBoxAtom(TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> stackrel_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> a(new UnderOverAtom(TeXFormula(tp, args[2], false)._root,
	                                     TeXFormula(tp, args[3], false)._root,
	                                     UNIT_MU, 0.5f, true,
	                                     TeXFormula(tp, args[1], false)._root,
	                                     UNIT_MU, 2.5f, true));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline shared_ptr<Atom> stackbin_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> a(new UnderOverAtom(TeXFormula(tp, args[2], false)._root,
	                                     TeXFormula(tp, args[3], false)._root,
	                                     UNIT_MU, 0.5f, true,
	                                     TeXFormula(tp, args[1], false)._root,
	                                     UNIT_MU, 2.5f, true));
	return shared_ptr<Atom>(new TypedAtom(TYPE_BINARY_OPERATOR, TYPE_BINARY_OPERATOR, a));
}

inline shared_ptr<Atom> overset_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> a(new UnderOverAtom(TeXFormula(tp, args[2], false)._root,
	                                     TeXFormula(tp, args[1], false)._root,
	                                     UNIT_MU, 2.5f, true, true));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline shared_ptr<Atom> underset_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> a(new UnderOverAtom(TeXFormula(tp, args[2], false)._root,
	                                     TeXFormula(tp, args[1], false)._root,
	                                     UNIT_MU, 0.5f, true, false));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline shared_ptr<Atom> accentset_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new AccentedAtom(TeXFormula(tp, args[2], false)._root, TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> underaccent_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new UnderOverAtom(TeXFormula(tp, args[2], false)._root, TeXFormula(tp, args[1], false)._root, UNIT_MU, 0.3f, true, false));
}

inline shared_ptr<Atom> undertilde_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	shared_ptr<PhantomAtom> p(new PhantomAtom(a, true, false, false));
	shared_ptr<AccentedAtom> acc(new AccentedAtom(p, "widetilde"));
	return shared_ptr<Atom>(new UnderOverAtom(a, acc, UNIT_MU, 0.3f, true, false));
}

inline shared_ptr<Atom> boldsymbol_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new BoldAtom(TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> mathrm_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new RomanAtom(TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> rm_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new RomanAtom(TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline shared_ptr<Atom> mathbf_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new BoldAtom(shared_ptr<Atom>(new RomanAtom(TeXFormula(tp, args[1], false)._root))));
}

inline shared_ptr<Atom> bf_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new BoldAtom(shared_ptr<Atom>(new RomanAtom(TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root))));
}

inline shared_ptr<Atom> mathtt_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TtAtom(TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> tt_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TtAtom(TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline shared_ptr<Atom> mathit_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new ItAtom(TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> it_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new ItAtom(TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline shared_ptr<Atom> mathsf_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new SsAtom(TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> sf_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new SsAtom(TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline shared_ptr<Atom> LaTeX_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new LaTeXAtom());
}

inline shared_ptr<Atom> hphantom_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new PhantomAtom(TeXFormula(tp, args[1], false)._root, true, false, false));
}

inline shared_ptr<Atom> vphantom_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new PhantomAtom(TeXFormula(tp, args[1], false)._root, false, true, true));
}

inline shared_ptr<Atom> phantom_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new PhantomAtom(TeXFormula(tp, args[1], false)._root, true, true, true));
}

inline shared_ptr<Atom> big_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	return shared_ptr<Atom>(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 1));
}

inline shared_ptr<Atom> Big_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	return shared_ptr<Atom>(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 2));
}

inline shared_ptr<Atom> bigg_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	return shared_ptr<Atom>(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 3));
}

inline shared_ptr<Atom> Bigg_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	return shared_ptr<Atom>(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 4));
}

inline shared_ptr<Atom> bigl_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 1));
	t->_type = TYPE_OPENING;
	return t;
}

inline shared_ptr<Atom> Bigl_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 2));
	t->_type = TYPE_OPENING;
	return t;
}

inline shared_ptr<Atom> biggl_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 3));
	t->_type = TYPE_OPENING;
	return t;
}

inline shared_ptr<Atom> Biggl_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 4));
	t->_type = TYPE_OPENING;
	return t;
}

inline shared_ptr<Atom> bigr_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 1));
	t->_type = TYPE_CLOSING;
	return t;
}

inline shared_ptr<Atom> Bigr_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 2));
	t->_type = TYPE_CLOSING;
	return t;
}

inline shared_ptr<Atom> biggr_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 3));
	t->_type = TYPE_CLOSING;
	return t;
}

inline shared_ptr<Atom> Biggr_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[1], false)._root;
	SymbolAtom* s = dynamic_cast<SymbolAtom*>(a.get());
	if (s == nullptr)
		return a;
	shared_ptr<Atom> t(new BigDelimiterAtom(dynamic_pointer_cast<SymbolAtom>(a), 4));
	t->_type = TYPE_CLOSING;
	return t;
}

inline shared_ptr<Atom> displaystyle_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
	return shared_ptr<Atom>(new StyleAtom(STYLE_DISPLAY, g));
}

inline shared_ptr<Atom> scriptstyle_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
	return shared_ptr<Atom>(new StyleAtom(STYLE_SCRIPT, g));
}

inline shared_ptr<Atom> textstyle_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
	return shared_ptr<Atom>(new StyleAtom(STYLE_TEXT, g));
}

inline shared_ptr<Atom> scriptscriptstyle_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto g = TeXFormula(tp, tp.getOverArgument(), false)._root;
	return shared_ptr<Atom>(new StyleAtom(STYLE_SCRIPT_SCRIPT, g));
}

inline shared_ptr<Atom> rotatebox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	float x = 0;
	if (!args[1].empty())
		valueof(args[1], x);
	return shared_ptr<Atom>(new RotateAtom(TeXFormula(tp, args[2])._root, x, args[3]));
}

inline shared_ptr<Atom> reflectbox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new ReflectAtom(TeXFormula(tp, args[1])._root));
}

inline shared_ptr<Atom> scalebox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	float sx = 1, sy = 1;
	valueof(args[1], sx);
	if (args[3].empty())
		sy = sx;
	else
		valueof(args[3], sy);
	if (sx == 0)
		sx = 1;
	if (sy == 0)
		sy = 1;
	return shared_ptr<Atom>(new ScaleAtom(TeXFormula(tp, args[2])._root, sx, sy));
}

inline shared_ptr<Atom> resizebox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	string ws, hs;
	wide2utf8(args[1].c_str(), ws);
	wide2utf8(args[2].c_str(), hs);
	return shared_ptr<Atom>(new ResizeAtom(TeXFormula(tp, args[3])._root, ws, hs, ws == "!" || hs == "!"));
}

inline shared_ptr<Atom> shadowbox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new ShadowAtom(TeXFormula(tp, args[1])._root));
}

inline shared_ptr<Atom> ovalbox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new OvalAtom(TeXFormula(tp, args[1])._root));
}

inline shared_ptr<Atom> doublebox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new DoubleFramedAtom(TeXFormula(tp, args[1])._root));
}

inline shared_ptr<Atom> fgcolor_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[2])._root;
	string x = wide2utf8(args[1].c_str());
	return shared_ptr<Atom>(new ColorAtom(a, TRANS, ColorAtom::getColor(x)));
}

inline shared_ptr<Atom> bgcolor_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[2])._root;
	string x = wide2utf8(args[1].c_str());
	return shared_ptr<Atom>(new ColorAtom(a, ColorAtom::getColor(x), TRANS));
}

inline shared_ptr<Atom> textcolor_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto a = TeXFormula(tp, args[2])._root;
	string x = wide2utf8(args[1].c_str());
	return shared_ptr<Atom>(new ColorAtom(a, TRANS, ColorAtom::getColor(x)));
}

inline shared_ptr<Atom> colorbox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	string x = wide2utf8(args[1].c_str());
	color c = ColorAtom::getColor(x);
	return shared_ptr<Atom>(new FBoxAtom(TeXFormula(tp, args[2])._root, c, c));
}

inline shared_ptr<Atom> fcolorbox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	string x = wide2utf8(args[2].c_str());
	color f = ColorAtom::getColor(x);
	string y = wide2utf8(args[1].c_str());
	color b = ColorAtom::getColor(y);
	return shared_ptr<Atom>(new FBoxAtom(TeXFormula(tp, args[3])._root, f, b));
}

inline shared_ptr<Atom> cong_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	VRowAtom* vra = new VRowAtom(SymbolAtom::get("equals"));
	vra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, 0, 1.5f, 0)));
	vra->add(SymbolAtom::get("sim"));
	vra->setRaise(UNIT_MU, -1);
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(vra)));
}

inline shared_ptr<Atom> doteq_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto e = SymbolAtom::get("equals");
	auto l = SymbolAtom::get("ldotp");
	shared_ptr<Atom> u(new UnderOverAtom(e, l, UNIT_MU, 3.7f, false, true));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, u));
}

inline shared_ptr<Atom> externalfont_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	string x = wide2utf8(args[1].c_str());
	TextRenderingBox::setFont(x);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> ctext_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TextRenderingAtom(args[1], PLAIN));
}

inline shared_ptr<Atom> textit_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TextRenderingAtom(args[1], ITALIC));
}

inline shared_ptr<Atom> textbf_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TextRenderingAtom(args[1], BOLD));
}

inline shared_ptr<Atom> textitbf_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TextRenderingAtom(args[1], BOLD | ITALIC));
}

inline shared_ptr<Atom> declaremathsizes_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	float a, b, c, d;
	valueof(args[1], a), valueof(args[2], b), valueof(args[3], c), valueof(args[4], d);
	DefaultTeXFont::setMathSizes(a, b, c, c);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> magnification_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	float x;
	valueof(args[1], x);
	DefaultTeXFont::setMagnification(x);
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> hline_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	if (!tp.isArrayMode())
		throw ex_parse("The macro \\hline only available in array mode!");
	return shared_ptr<Atom>(new HlineAtom());
}

inline shared_ptr<Atom> mathcumsup_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new CumulativeScriptsAtom(tp.getLastAtom(), shared_ptr<Atom>(nullptr), TeXFormula(tp, args[1])._root));
}

inline shared_ptr<Atom> mathcumsub_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new CumulativeScriptsAtom(tp.getLastAtom(), TeXFormula(tp, args[1])._root, shared_ptr<Atom>(nullptr)));
}

inline shared_ptr<Atom> dotminus_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> a(new UnderOverAtom(SymbolAtom::get("minus"), SymbolAtom::get("normaldot"), UNIT_MU, -3.3f, false, true));
	return shared_ptr<Atom>(new TypedAtom(TYPE_BINARY_OPERATOR, TYPE_BINARY_OPERATOR, a));
}

inline shared_ptr<Atom> ratio_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> a(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline shared_ptr<Atom> geoprop_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<RowAtom> ddot(new RowAtom(SymbolAtom::get("normaldot")));
	ddot->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, 4, 0, 0)));
	ddot->add(SymbolAtom::get("normaldot"));
	shared_ptr<Atom> a(new UnderOverAtom(SymbolAtom::get("minus"), ddot, UNIT_MU, -3.4f, false, ddot, UNIT_MU, -3.4f, false));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, a));
}

inline shared_ptr<Atom> minuscolon_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(SymbolAtom::get("minus"));
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
	shared_ptr<Atom> colon(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	ra->add(colon);
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> minuscoloncolon_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(SymbolAtom::get("minus"));
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
	shared_ptr<Atom> colon(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false , true));
	ra->add(colon);
	ra->add(colon);
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> simcolon_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(SymbolAtom::get("sim"));
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
	shared_ptr<Atom> colon(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	ra->add(colon);
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> simcoloncolon_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(SymbolAtom::get("sim"));
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
	shared_ptr<Atom> colon(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	ra->add(colon);
	ra->add(colon);
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> approxcolon_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(SymbolAtom::get("approx"));
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
	shared_ptr<Atom> colon(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	ra->add(colon);
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> approxcoloncolon_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(SymbolAtom::get("approx"));
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
	shared_ptr<Atom> colon(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	ra->add(colon);
	ra->add(colon);
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> equalscolon_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(SymbolAtom::get("equals"));
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
	shared_ptr<Atom> colon(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	ra->add(colon);
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> equalscoloncolon_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(SymbolAtom::get("equals"));
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.095f, 0, 0)));
	shared_ptr<Atom> colon(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	ra->add(colon);
	ra->add(colon);
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> colonminus_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> u(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	RowAtom* ra = new RowAtom(u);
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
	ra->add(SymbolAtom::get("minus"));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> coloncolonminus_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> u(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	RowAtom* ra = new RowAtom(u);
	ra->add(u);
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
	ra->add(SymbolAtom::get("minus"));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> colonequals_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> u(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	RowAtom* ra = new RowAtom(u);
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
	ra->add(SymbolAtom::get("equals"));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> coloncolonequals_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> u(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	RowAtom* ra = new RowAtom(u);
	ra->add(u);
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
	ra->add(SymbolAtom::get("equals"));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> coloncolon_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> u(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	RowAtom* ra = new RowAtom(u);
	ra->add(u);
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> colonsim_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> u(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	RowAtom* ra = new RowAtom(u);
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
	ra->add(SymbolAtom::get("sim"));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> coloncolonsim_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> u(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	RowAtom* ra = new RowAtom(u);
	ra->add(u);
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
	ra->add(SymbolAtom::get("sim"));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> colonapprox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> u(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	RowAtom* ra = new RowAtom(u);
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
	ra->add(SymbolAtom::get("approx"));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> coloncolonapprox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> u(new UnderOverAtom(SymbolAtom::get("normaldot"), SymbolAtom::get("normaldot"), UNIT_MU, 5.2f, false, true));
	RowAtom* ra = new RowAtom(u);
	ra->add(u);
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_EM, -0.32f, 0, 0)));
	ra->add(SymbolAtom::get("approx"));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> smallfrowneq_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<Atom> u(new UnderOverAtom(SymbolAtom::get("equals"), SymbolAtom::get("smallfrown"), UNIT_MU, -2, true, true));
	return shared_ptr<Atom>(new TypedAtom(TYPE_RELATION, TYPE_RELATION, u));
}

inline shared_ptr<Atom> hstrok_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	shared_ptr<RowAtom> ra(new RowAtom(shared_ptr<Atom>(new SpaceAtom(UNIT_EX, -0.1f, 0, 0))));
	ra->add(SymbolAtom::get("bar"));
	VRowAtom* vra = new VRowAtom(shared_ptr<Atom>(new LapedAtom(ra, 'r')));
	vra->setRaise(UNIT_EX, -0.1f);
	RowAtom* a = new RowAtom(shared_ptr<Atom>(vra));
	a->add(shared_ptr<Atom>(new RomanAtom(shared_ptr<Atom>(new CharAtom('h', tp._formula->_textStyle)))));
	return shared_ptr<Atom>(a);
}

inline shared_ptr<Atom> Hstrok_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(shared_ptr<Atom>(new SpaceAtom(UNIT_EX, -0.28f, 0, 0)));
	ra->add(SymbolAtom::get("textendash"));
	VRowAtom* vra = new VRowAtom(shared_ptr<Atom>(new LapedAtom(shared_ptr<Atom>(ra), 'r')));
	vra->setRaise(UNIT_EX, 0.55f);
	RowAtom* a = new RowAtom(shared_ptr<Atom>(vra));
	a->add(shared_ptr<Atom>(new RomanAtom(shared_ptr<Atom>(new CharAtom('H', tp._formula->_textStyle)))));
	return shared_ptr<Atom>(a);
}

inline shared_ptr<Atom> dstrok_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(shared_ptr<Atom>(new SpaceAtom(UNIT_EX, 0.25f, 0, 0)));
	ra->add(SymbolAtom::get("bar"));
	VRowAtom* vra = new VRowAtom(shared_ptr<Atom>(new LapedAtom(shared_ptr<Atom>(ra), 'r')));
	vra->setRaise(UNIT_EX, -0.1f);
	RowAtom* a = new RowAtom(shared_ptr<Atom>(vra));
	a->add(shared_ptr<Atom>(new RomanAtom(shared_ptr<Atom>(new CharAtom('d', tp._formula->_textStyle)))));
	return shared_ptr<Atom>(a);
}

inline shared_ptr<Atom> Dstrok_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	RowAtom* ra = new RowAtom(shared_ptr<Atom>(new SpaceAtom(UNIT_EX, -0.1f, 0, 0)));
	ra->add(SymbolAtom::get("bar"));
	VRowAtom* vra = new VRowAtom(shared_ptr<Atom>(new LapedAtom(shared_ptr<Atom>(ra), 'r')));
	vra->setRaise(UNIT_EX, -0.55f);
	RowAtom* a = new RowAtom(shared_ptr<Atom>(vra));
	a->add(shared_ptr<Atom>(new RomanAtom(shared_ptr<Atom>(new CharAtom('D', tp._formula->_textStyle)))));
	return shared_ptr<Atom>(a);
}

inline shared_ptr<Atom> kern_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	pair<int, float> info = SpaceAtom::getLength(args[1]);

	return shared_ptr<Atom>(new SpaceAtom(info.first, info.second, 0, 0));
}

inline shared_ptr<Atom> char_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
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

inline shared_ptr<Atom> T_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new RotateAtom(TeXFormula(tp, args[1])._root, 180, L"origin=cc"));
}

inline shared_ptr<Atom> textcircled_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new TextCircledAtom(shared_ptr<Atom>(new RomanAtom(TeXFormula(tp, args[1])._root))));
}

inline shared_ptr<Atom> textsc_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new SmallCpaAtom(TeXFormula(tp, args[1], false)._root));
}

inline shared_ptr<Atom> sc_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new SmallCpaAtom(TeXFormula(tp, tp.getOverArgument(), "", false, tp.isIgnoreWhiteSpace())._root));
}

inline shared_ptr<Atom> quad_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new SpaceAtom(UNIT_EM, 1, 0, 0));
}

inline shared_ptr<Atom> surd_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new VCenteredAtom(SymbolAtom::get("surdsign")));
}

inline shared_ptr<Atom> int_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
	integral->_typelimits = SCRIPT_NOLIMITS;
	return shared_ptr<Atom>(integral);
}

inline shared_ptr<Atom> oint_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("oint")));
	integral->_typelimits = SCRIPT_NOLIMITS;
	return shared_ptr<Atom>(integral);
}

inline shared_ptr<Atom> iint_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	SymbolAtom* integral = new SymbolAtom(*(SymbolAtom::get("int")));
	integral->_typelimits = SCRIPT_NOLIMITS;
	shared_ptr<Atom> i(integral);
	RowAtom* ra = new RowAtom(i);
	ra->add(shared_ptr<Atom>(new SpaceAtom(UNIT_MU, -8.f, 0, 0)));
	ra->add(i);
	ra->_lookAtLastAtom = true;
	return shared_ptr<Atom>(new TypedAtom(TYPE_BIG_OPERATOR, TYPE_BIG_OPERATOR, shared_ptr<Atom>(ra)));
}

inline shared_ptr<Atom> iiint_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
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

inline shared_ptr<Atom> iiiint_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
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

inline shared_ptr<Atom> idotsint_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
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

inline shared_ptr<Atom> lmoustache_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	SymbolAtom* s = new SymbolAtom(*(SymbolAtom::get("lmoustache")));
	shared_ptr<Atom> b(new BigDelimiterAtom(shared_ptr<SymbolAtom>(s), 1));
	b->_type = TYPE_OPENING;
	return b;
}

inline shared_ptr<Atom> rmoustache_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	SymbolAtom* s = new SymbolAtom(*(SymbolAtom::get("rmoustache")));
	shared_ptr<Atom> b(new BigDelimiterAtom(shared_ptr<SymbolAtom>(s), 1));
	b->_type = TYPE_CLOSING;
	return b;
}

inline shared_ptr<Atom> insertBreakMark_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(new BreakMarkAtom());
}

/******************************************************* implement at .cpp *************************************************/

shared_ptr<Atom> hvspace_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> rule_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> cfrac_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> sfrac_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> genfrac_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> overwithdelims_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> atopwithdelims_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> abovewithdelims_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> textstyle_macros(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> accentbis_macros(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> left_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> intertext_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> newcommand_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> renewcommand_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> raisebox_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> definecolor_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> size_macros(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> romannumeral_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> muskip_macros(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

shared_ptr<Atom> xml_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);

/******************************************************* should be fixed **************************************************/

inline shared_ptr<Atom> nolimits_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto atom = tp.getLastAtom();
	atom->_typelimits = SCRIPT_NOLIMITS;
	return atom;
}

inline shared_ptr<Atom> limits_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto atom = tp.getLastAtom();
	atom->_typelimits = SCRIPT_LIMITS;
	return atom;
}

inline shared_ptr<Atom> normal_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	auto atom = tp.getLastAtom();
	atom->_typelimits = SCRIPT_NORMAL;
	return atom;
}

/******************************************************* not implemented **************************************************/

inline shared_ptr<Atom> includegraphics_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> fcscore_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> GeoGebra_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(nullptr);
}

inline shared_ptr<Atom> dynamic_macro(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
	return shared_ptr<Atom>(nullptr);
}

}
}

#endif // MACRO_IMPL_H_INCLUDED
