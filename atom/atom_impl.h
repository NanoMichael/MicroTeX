#ifndef ATOM_IMPL_H_INCLUDED
#define ATOM_IMPL_H_INCLUDED

#if defined (__clang__)
#include "atom.h"
#include "box.h"
#include "core.h"
#include "fonts.h"
#include "formula.h"
#include "port.h"
#elif defined (__GNUC__)
#include "atom/atom.h"
#include "atom/box.h"
#include "core/core.h"
#include "fonts/fonts.h"
#include "core/formula.h"
#include "port/port.h"
#endif // defined
#include "common.h"

#include "memcheck.h"

using namespace std;
using namespace tex;
using namespace tex::fonts;
using namespace tex::core;
using namespace tex::port;

namespace tex {
namespace core {

inline static void parseMap(const string& options, _out_ map<string, string>& res) {
	if (options.empty())
		return;

	strtokenizer tokens(options, ",");
	const int c = tokens.count_tokens();
	for (int i = 0; i < c; i++) {
		string tok = tokens.next_token();
		trim(tok);
		vector<string> optarg;
		split(tok, '=', optarg);
		if (!optarg.empty()) {
			if (optarg.size() == 2)
				res[trim(optarg[0])] = trim(optarg[1]);
			else if (optarg.size() == 1)
				res[trim(optarg[0])] = "";
		}
	}
}

/**
 * atom to justify cells in array
 */
class CellSpecifier : public Atom {
public:
	virtual void apply(const shared_ptr<Box>& box) = 0;

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		return shared_ptr<Box>(new StrutBox(0, 0, 0, 0));
	}
};

/**
 * atom representing column color in array
 */
class CellColorAtom : public CellSpecifier {
private:
	color _color;
public:
	CellColorAtom() = delete;

	CellColorAtom(color c) : _color(c) {}

	void apply(const shared_ptr<Box>& box) {
		box->_background = _color;
	}
};

/**
 * atom representing column foreground in array
 */
class CellForegroundAtom : public CellSpecifier {
private:
	color _color;
public:
	CellForegroundAtom() = delete;

	CellForegroundAtom(color c) : _color(c) {}

	void apply(const shared_ptr<Box>& box) {
		box->_foreground = _color;
	}
};

/**
 * atom representing multi-row
 */
class MultiRowAtom : public Atom {
private:
	shared_ptr<Atom> _rows;
public:
	int _i, _j, _n;

	MultiRowAtom() = delete;

	MultiRowAtom(int n, const wstring& option, const shared_ptr<Atom> rows) :
		_i(0), _j(0), _rows(rows) {
		_n = n == 0 ? 1 : n;
	}

	inline void setRowColumn(int r, int c) {
		_i = r;
		_j = c;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto b = _rows->createBox(env);
		b->_type = TYPE_MULTIROW;
		return b;
	}
};

class VlineAtom;

/**
 * atom representing matrix
 */
enum MAT {
	ARRAY = 0,
	MATRIX,
	ALIGN,
	ALIGNAT,
	FLALIGN,
	SMALLMATRIX,
	ALIGNED,
	ALIGNEDAT
};

class MatrixAtom : public Atom {
private:
	static map<wstring, wstring> _colspeReplacement;

	static SpaceAtom _align;

	shared_ptr<ArrayOfAtoms> _matrix;
	vector<int> _position;
	map<int, shared_ptr<VlineAtom>> _vlines;
	map<int, shared_ptr<Atom>> _columnSpecifiers;

	int _ttype;
	bool _ispartial;
	bool _spaceAround;

	void parsePositions(wstring opt, _out_ vector<int>& lpos);

	shared_ptr<Box> generateMulticolumn(_out_ TeXEnvironment& env, const shared_ptr<Box>& b, const float* hsep, const float* rowW, int i, int j);

	void recalculateLine(const int row, shared_ptr<Box>** boxarr, vector<shared_ptr<Atom>>& rows, float* height, float* depth, float drt, float vspace);

	float* getColumnSep(_out_ TeXEnvironment& env, float width);
public:
	static color LINE_COLOR;

	static SpaceAtom _hsep, _semihsep, _vsep_in, _vsep_ext_top, _vsep_ext_bot;

	static shared_ptr<Box> _nullbox;

	MatrixAtom() = delete;

	MatrixAtom(bool ispar, const shared_ptr<ArrayOfAtoms>& arr, const wstring& options, bool sa);

	MatrixAtom(bool ispar, const shared_ptr<ArrayOfAtoms>& arr, const wstring& options);

	MatrixAtom(bool ispar, const shared_ptr<ArrayOfAtoms>& arr, int type);

	MatrixAtom(bool ispar, const shared_ptr<ArrayOfAtoms>& arr, int type, int align);

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;

	static void defineColumnSpecifier(const wstring& rep, const wstring& spe);
};

/**
 * an atom representing vertical-line in matrix environment
 */
class VlineAtom : public Atom {
private:
	int _n;
public:
	float _height, _shift;

	VlineAtom() = delete;

	VlineAtom(int n) : _n(n), _height(0), _shift(0) {
	}

	inline float getWidth(_in_ TeXEnvironment& env) {
		if (_n != 0) {
			float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
			return drt * (3 * _n - 2);
		}
		return 0;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		if (_n == 0)
			return shared_ptr<Box>(new StrutBox(0, 0, 0, 0));

		float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
		shared_ptr<Box> b(new HorizontalRule(_height, drt, _shift, MatrixAtom::LINE_COLOR, true));
		shared_ptr<Box> sep(new StrutBox(2 * drt, 0, 0, 0));
		HorizontalBox* hb = new HorizontalBox();
		for (int i = 0; i < _n - 1; i++) {
			hb->add(b);
			hb->add(sep);
		}

		if (_n > 0)
			hb->add(b);

		return shared_ptr<Box>(hb);
	}
};

enum MULTILINETYPE {
	MULTILINE = 0,
	GATHER,
	GATHERED
};

/**
 * an atom representing a vertical row of other atoms
 */
class MultlineAtom : public Atom {
private:
	static SpaceAtom _vsep_in;
	shared_ptr<ArrayOfAtoms> _column;
	int _ttype;
	bool _ispartial;
public:
	MultlineAtom() = delete;

	MultlineAtom(bool ispar, const shared_ptr<ArrayOfAtoms>& col, int type) {
		_ispartial = ispar;
		_ttype = type;
		_column = col;
	}

	MultlineAtom(const shared_ptr<ArrayOfAtoms>& col, int type) {
		_ispartial = false;
		_ttype = type;
		_column = col;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;
};

/**
 * an atom representing a big delimiter atom (i.e. sigma)
 */
class BigDelimiterAtom : public Atom {
private:
	int _size;
public:
	shared_ptr<SymbolAtom> _delim;

	BigDelimiterAtom() = delete;

	BigDelimiterAtom(const shared_ptr<SymbolAtom>& delim, int s) :
		_delim(delim), _size(s) {
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto b = DelimiterFactory::create(*_delim, env, _size);
		HorizontalBox* hb = new HorizontalBox();
		float h = b->_height;
		float total = h + b->_depth;
		float axis = env.getTeXFont()->getAxisHeight(env.getStyle());
		b->_shift = -total / 2 + h - axis;
		hb->add(b);
		return shared_ptr<Box>(hb);
	}
};

/**
 * an atom representing a bold atom
 */
class BoldAtom : public Atom {

private:
	shared_ptr<Atom> _base;

public:
	BoldAtom() = delete;

	BoldAtom(const shared_ptr<Atom>& base) :
		_base(base) {
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		if (_base != nullptr) {
			TeXEnvironment& e = *(env.copy(env.getTeXFont()->copy()));
			e.getTeXFont()->setBold(true);
			return _base->createBox(e);
		}
		return shared_ptr<Box>(new StrutBox(0, 0, 0, 0));
	}
};

/**
 * an atom with cedilla
 */
class CedillAtom : public Atom {
private:
	shared_ptr<Atom> _base;
public:
	CedillAtom() = delete;

	CedillAtom(const shared_ptr<Atom>& base) : _base(base) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto b = _base->createBox(env);
		VerticalBox* vb = new VerticalBox();
		vb->add(b);
		Char ch = env.getTeXFont()->getChar("mathcedilla", env.getStyle());
		float italic = ch.getItalic();
		Box* cedilla = new CharBox(ch);
		Box* y;
		if (abs(italic) > PREC) {
			y = new HorizontalBox(shared_ptr<Box>(new StrutBox(-italic, 0, 0, 0)));
			y->add(shared_ptr<Box>(cedilla));
		} else {
			y = cedilla;
		}

		Box* ce = new HorizontalBox(shared_ptr<Box>(y), b->_width, ALIGN_CENTER);
		float x = 0.4f * SpaceAtom::getFactor(UNIT_MU, env);
		vb->add(shared_ptr<Box>(new StrutBox(0, -x, 0, 0)));
		vb->add(shared_ptr<Box>(ce));
		float f = vb->_height + vb->_depth;
		vb->_height = b->_height;
		vb->_depth = f - b->_height;
		return shared_ptr<Box>(vb);
	}
};

/**
 * an atom representing ddots
 */
class DdtosAtom : public Atom {
public:
	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto ldots = TeXFormula::get(L"ldots")->_root->createBox(env);
		float w = ldots->_width;
		auto dot = SymbolAtom::get("ldotp")->createBox(env);
		HorizontalBox* hb1 = new HorizontalBox(dot, w, ALIGN_LEFT);
		HorizontalBox* hb2 = new HorizontalBox(dot, w, ALIGN_CENTER);
		HorizontalBox* hb3 = new HorizontalBox(dot, w, ALIGN_RIGHT);
		shared_ptr<Box> pt4(SpaceAtom(UNIT_MU, 0, 4, 0).createBox(env));
		VerticalBox* vb = new VerticalBox();
		vb->add(shared_ptr<Box>(hb1));
		vb->add(pt4);
		vb->add(shared_ptr<Box>(hb2));
		vb->add(pt4);
		vb->add(shared_ptr<Box>(hb3));

		float h = vb->_height + vb->_depth;
		vb->_height = h;
		vb->_depth = 0;
		return shared_ptr<Box>(vb);
	}
};

/**
 * an atom representing a boxed base atom
 */
class FBoxAtom : public Atom {
protected:
	shared_ptr<Atom> _base;
	color _bg, _line;
public:
	static const float INTERSPACE;

	FBoxAtom() = delete;

	FBoxAtom(const shared_ptr<Atom>& base, color bg = TRANS, color line = TRANS) {
		if (base == nullptr)
			_base = shared_ptr<Atom>(new RowAtom());
		else {
			_base = base;
			_type = base->_type;
		}
		_bg = bg;
		_line = line;
	}

	virtual shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto bbase = _base->createBox(env);
		float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
		float space = INTERSPACE * SpaceAtom::getFactor(UNIT_EM, env);
		if (istrans(_bg))
			return shared_ptr<Box>(new FramedBox(bbase, drt, space));
		env._isColored = true;
		return shared_ptr<Box>(new FramedBox(bbase, drt, space, _line, _bg));
	}
};

/**
 * an atom representing a boxed base atom
 */
class DoubleFramedAtom : public FBoxAtom {
public:
	DoubleFramedAtom() = delete;

	DoubleFramedAtom(const shared_ptr<Atom>& base) : FBoxAtom(base) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto bbase = _base->createBox(env);
		float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
		float space = INTERSPACE * SpaceAtom::getFactor(UNIT_EM, env);
		float sspace = 1.5f * drt + 0.5f * SpaceAtom::getFactor(UNIT_POINT, env);
		return shared_ptr<Box>(new FramedBox(shared_ptr<Box>(new FramedBox(bbase, 0.75 * drt, space)), 1.5f * drt, sspace));
	}
};

/**
 * an atom representing a box-shadowed atom
 */
class ShadowAtom : public FBoxAtom {
public:
	ShadowAtom() = delete;

	ShadowAtom(const shared_ptr<Atom>& base) : FBoxAtom(base) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto x = FBoxAtom::createBox(env);
		auto box = dynamic_pointer_cast<FramedBox>(x);
		float t = env.getTeXFont()->getDefaultRuleThickness(env.getStyle()) * 4;
		return shared_ptr<Box>(new ShadowBox(box, t));
	}
};

/**
 * an atom representing a base atom surrounded with delimiters that change their
 * size according to the height of the base
 */
class FencedAtom : public Atom {
private:
	static const int DELIMITER_FACTOR;
	static const float DELIMITER_SHORTFALL;
	// base atom
	shared_ptr<Atom> _base;
	// delimiters
	shared_ptr<SymbolAtom> _left;
	shared_ptr<SymbolAtom> _right;
	list<shared_ptr<MiddleAtom>> _middle;

	void init(const shared_ptr<Atom>& b, const shared_ptr<SymbolAtom>& l, const shared_ptr<SymbolAtom>& r);

	static void center(_out_ Box& b, float axis);
public:
	FencedAtom(const shared_ptr<Atom>& b, const shared_ptr<SymbolAtom>& l, const shared_ptr<SymbolAtom>& r) {
		init(b, l, r);
	}

	FencedAtom(const shared_ptr<Atom>& b, const shared_ptr<SymbolAtom>& l, const list<shared_ptr<MiddleAtom>>& m, const shared_ptr<SymbolAtom>& r) {
		init(b, l, r);
		_middle = m;
	}

	int getLeftType() const override {
		return TYPE_INNER;
	}

	int getRightType() const override {
		return TYPE_INNER;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;
};

/**
 * an atom representing a fraction
 */
class FractionAtom : public Atom {
private:
	// whether the default thickness should not be used for fraction line
	bool _nodefault;
	// unit used for the thickness of the fraction line
	int _unit;
	// alignment settings for the numerator and denominator
	int _numAlign, _denomAlign;
	// the atoms representing the numerator and denominator
	shared_ptr<Atom> _numerator, _denominator;
	// thickness of the fraction line
	float _thickness;
	// thickness of the fraction line relative to the default thickness
	float _deffactor;
	// whether the def-factor value should be used
	bool _deffactorset;

	inline int checkAlign(int align) {
		if (align == ALIGN_LEFT || align == ALIGN_RIGHT)
			return align;
		return ALIGN_CENTER;
	}

	void init(const shared_ptr<Atom>& num, const shared_ptr<Atom>& den, bool nodef, int unit, float t) throw(ex_invalid_unit);
public:
	FractionAtom() = delete;

	FractionAtom(const shared_ptr<Atom>& num, const shared_ptr<Atom>& den) {
		init(num, den, false, UNIT_PIXEL, 0.f);
	}

	FractionAtom(const shared_ptr<Atom>& num, const shared_ptr<Atom>& den, bool rule) {
		init(num, den, !rule, UNIT_PIXEL, 0.f);
	}

	FractionAtom(const shared_ptr<Atom>& num, const shared_ptr<Atom>& den, bool nodef, int unit, float t) throw(ex_invalid_unit) {
		init(num, den, nodef, unit, t);
	}

	FractionAtom(const shared_ptr<Atom>& num, const shared_ptr<Atom>& den, bool rule, int numAlign, int denomAlign) {
		init(num, den, !rule, UNIT_PIXEL, 0.f);
		_numAlign = checkAlign(numAlign);
		_denomAlign = checkAlign(denomAlign);
	}

	FractionAtom(const shared_ptr<Atom>& num, const shared_ptr<Atom>& den, float deffactor, int numAlign, int denomAlign) {
		init(num, den, false, UNIT_PIXEL, 0.f);
		_numAlign = checkAlign(numAlign);
		_denomAlign = checkAlign(denomAlign);
		_deffactor = deffactor;
		_deffactorset = true;
	}

	FractionAtom(const shared_ptr<Atom>& num, const shared_ptr<Atom>& den, int unit, float t, int numAlign, int denomAlign) {
		init(num, den, true, unit, t);
		_numAlign = checkAlign(numAlign);
		_denomAlign = checkAlign(denomAlign);
	}

	FractionAtom(const shared_ptr<Atom>& num, const shared_ptr<Atom>& den, int unit, float t) {
		init(num, den, true, unit, t);
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;
};

/**
 * an atom used in array mode to write on several columns
 */
class MulticolumnAtom : public Atom {
protected:
	int _n;
	int _align;
	float _w;
	int _beforeVlines, _afterVlines;
	int _row, _col;
	shared_ptr<Atom> _cols;

	int parseAlign(const string& str);
public:
	MulticolumnAtom() = delete;

	MulticolumnAtom(int n, const string& align, const shared_ptr<Atom> cols) :
		_w(0), _beforeVlines(0), _afterVlines(0), _row(0), _col(0) {
		_n = n >= 1 ? n : 1;
		_cols = cols;
		_align = parseAlign(align);
	}

	inline void setWidth(float w) {
		_w = w;
	}

	inline int getSkipped() {
		return _n;
	}

	inline bool hasRightVline() {
		return _afterVlines != 0;
	}

	inline void setRowColumn(int i, int j) {
		_row = i;
		_col = j;
	}

	inline int getAlign() {
		return _align;
	}

	inline int getRow() {
		return _row;
	}

	inline int getCol() {
		return _col;
	}

	virtual shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;
};

/**
 * an atom used in array mode to write on several columns
 */
class HdotsforAtom : public MulticolumnAtom {
private:
	static SpaceAtom _thin;
	float _coeff;
public:
	HdotsforAtom() = delete;

	HdotsforAtom(int n, float coeff) :
		MulticolumnAtom(n, "c", SymbolAtom::get("ldotp")), _coeff(coeff) {
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;
};

/**
 * an atom representing id-dots
 */
class IddotsAtom : public Atom {
public:
	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto ldots = TeXFormula::get(L"ldots")->_root->createBox(env);
		float w = ldots->_width;
		auto dot = SymbolAtom::get("ldotp")->createBox(env);
		shared_ptr<Box> hb1(new HorizontalBox(dot, w, ALIGN_RIGHT));
		shared_ptr<Box> hb2(new HorizontalBox(dot, w, ALIGN_CENTER));
		shared_ptr<Box> hb3(new HorizontalBox(dot, w, ALIGN_LEFT));
		shared_ptr<Box> pt4 = SpaceAtom(UNIT_MU, 0, 4, 0).createBox(env);
		VerticalBox* vb = new VerticalBox();
		vb->add(hb1);
		vb->add(pt4);
		vb->add(hb2);
		vb->add(pt4);
		vb->add(hb3);

		float h = vb->_height + vb->_depth;
		vb->_height = h;
		vb->_depth = 0;

		return shared_ptr<Box>(vb);
	}
};

/**
 * an atom representing an IJ
 */
class IJAtom : public Atom {
private:
	bool _upper;
public:
	IJAtom() = delete;

	IJAtom(bool upper) : _upper(upper) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		CharBox* I = new CharBox(env.getTeXFont()->getChar(_upper ? 'I' : 'i', "mathnormal", env.getStyle()));
		CharBox* J = new CharBox(env.getTeXFont()->getChar(_upper ? 'J' : 'j', "mathnormal", env.getStyle()));
		HorizontalBox* hb = new HorizontalBox(shared_ptr<Box>(I));
		hb->add(SpaceAtom(UNIT_EM, -0.065f, 0, 0).createBox(env));
		hb->add(shared_ptr<Box>(J));
		return shared_ptr<Box>(hb);
	}
};

/**
 * an atom representing a italic atom
 */
class ItAtom : public Atom {
private:
	shared_ptr<Atom> _base;
public:
	ItAtom() = delete;

	ItAtom(const shared_ptr<Atom> base) : _base(base) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		shared_ptr<Box> box;
		if (_base != nullptr) {
			TeXEnvironment& e = *(env.copy(env.getTeXFont()->copy()));
			e.getTeXFont()->setIt(true);
			box = _base->createBox(e);
		} else {
			box = shared_ptr<Box>(new StrutBox(0, 0, 0, 0));
		}

		return box;
	}
};

/**
 * an atom representing a lapped atom (i.e. with no width)
 */
class LapedAtom : public Atom {
private:
	shared_ptr<Atom> _at;
	wchar_t _type;
public:
	LapedAtom() = delete;

	LapedAtom(const shared_ptr<Atom>& a, wchar_t type) : _at(a), _type(type) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto b = _at->createBox(env);
		VerticalBox* vb = new VerticalBox();
		vb->add(b);
		vb->_width = 0;
		switch(_type) {
		case 'l':
			b->_shift = -b->_width;
			break;
		case 'r':
			b->_shift = 0;
			break;
		default:
			b->_shift = -b->_width / 2;
			break;
		}

		return shared_ptr<Box>(vb);
	}
};

/**
 * an atom representing LaTeX logo. the dimension values can be set using
 * different unit types.
 */
class LaTeXAtom : public Atom {
public:
	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;
};

/**
 * an atom representing an L with a Caron
 */
class LCaronAtom : public Atom {
private:
	bool _upper;
public:
	LCaronAtom() = delete;

	LCaronAtom(bool upper) : _upper(upper) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		CharBox* A = new CharBox(env.getTeXFont()->getChar("textapos", env.getStyle()));
		CharBox* L = new CharBox(env.getTeXFont()->getChar(_upper ? 'L' : 'l', "mathnormal", env.getStyle()));
		HorizontalBox* hb = new HorizontalBox(shared_ptr<Box>(L));
		if (_upper)
			hb->add(SpaceAtom(UNIT_EM, -0.3f, 0, 0).createBox(env));
		else
			hb->add(SpaceAtom(UNIT_EM, -0.13f, 0, 0).createBox(env));
		hb->add(shared_ptr<Box>(A));
		return shared_ptr<Box>(hb);
	}
};

/**
 * an atom representing a mono scale atom
 */
class MonoScaleAtom : public ScaleAtom {
private:
	float _factor;
public:
	MonoScaleAtom() = delete;

	MonoScaleAtom(const shared_ptr<Atom>& base, float factor) :
		ScaleAtom(base, factor, factor), _factor(factor) {
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		TeXEnvironment& e = *(env.copy());
		float f = e.getScaleFactor();
		e.setScaleFactor(_factor);
		return shared_ptr<Box>(new ScaleBox(_base->createBox(e), _factor / f));
	}
};

/**
 * an atom with an Ogonek
 */
class OgonekAtom : public Atom {
private:
	shared_ptr<Atom> _base;
public:
	OgonekAtom() = delete;

	OgonekAtom(const shared_ptr<Atom>& base) : _base(base) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto b = _base->createBox(env);
		VerticalBox* vb = new VerticalBox();
		vb->add(b);
		Char ch = env.getTeXFont()->getChar("ogonek", env.getStyle());
		float italic = ch.getItalic();
		Box* ogonek = new CharBox(ch);
		Box* y = nullptr;
		if (abs(italic) > PREC) {
			y = new HorizontalBox(shared_ptr<Box>(new StrutBox(-italic, 0, 0, 0)));
			y->add(shared_ptr<Box>(ogonek));
		} else {
			y = ogonek;
		}

		Box* og = new HorizontalBox(shared_ptr<Box>(y), b->_width, ALIGN_RIGHT);
		vb->add(shared_ptr<Box>(new StrutBox(0, -ogonek->_height, 0, 0)));
		vb->add(shared_ptr<Box>(og));
		float f = vb->_height + vb->_depth;
		vb->_height = b->_height;
		vb->_depth = f - b->_height;
		return shared_ptr<Box>(vb);
	}
};

/**
 * an atom representing a boxed base atom
 */
class OvalAtom : public FBoxAtom {
public:
	OvalAtom() = delete;

	OvalAtom(const shared_ptr<Atom>& base) : FBoxAtom(base) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto x = FBoxAtom::createBox(env);
		auto box = dynamic_pointer_cast<FramedBox>(x);
		return shared_ptr<Box>(new OvalBox(box));
	}
};

/**
 * an atom representing a over-lined atom
 */
class OverlinedAtom : public Atom {
private:
	shared_ptr<Atom> _base;
public:
	OverlinedAtom() = delete;

	OverlinedAtom(const shared_ptr<Atom>& f) :
		_base(f) {
		_type = TYPE_ORDINARY;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
		// cramp the style of the formula to be overlined and create
		// vertical box
		auto b = _base == nullptr ? shared_ptr<Box>(new StrutBox(0, 0, 0, 0)) : _base->createBox(*(env.crampStyle()));
		OverBar* ob = new OverBar(b, 3 * drt, drt);

		// baseline vertical box = baseline box b
		ob->_depth = b->_depth;
		ob->_height = b->_height + 5 * drt;

		return shared_ptr<Box>(ob);
	}
};

class RaiseAtom : public Atom {
private:
	shared_ptr<Atom> _base;
	int _ru, _hu, _du;
	float _r, _h, _d;
public:
	RaiseAtom() = delete;

	RaiseAtom(const shared_ptr<Atom>& base, int ru, float r, int hu, float h, int du, float d) {
		_base = base;
		_ru = ru;
		_r = r;
		_hu = hu;
		_h = h;
		_du = du;
		_d = d;
	}

	int getLeftType() const override {
		return _base->getLeftType();
	}

	int getRightType() const override {
		return _base->getRightType();
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto bbox = _base->createBox(env);
		if (_ru == -1)
			bbox->_shift = 0;
		else
			bbox->_shift = -_r * SpaceAtom::getFactor(_ru, env);

		if (_hu == -1)
			return bbox;

		HorizontalBox* hbox = new HorizontalBox(bbox);
		hbox->_height = _h * SpaceAtom::getFactor(_hu, env);
		if (_du == -1)
			hbox->_depth = 0;
		else
			hbox->_depth = _d * SpaceAtom::getFactor(_du, env);

		return shared_ptr<Box>(hbox);
	}
};

/**
 * an atom representing a reflected atom
 */
class ReflectAtom : public Atom {
private:
	shared_ptr<Atom> _base;
public:
	ReflectAtom() = delete;

	ReflectAtom(const shared_ptr<Atom>& base) :
		_base(base) {
		_type = _base->_type;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		return shared_ptr<Box>(new ReflectBox(_base->createBox(env)));
	}
};

/**
 * an atom representing a resize atom
 */
class ResizeAtom : public Atom {
private:
	shared_ptr<Atom> _base;
	int _wu, _hu;
	float _w, _h;
	bool _keep_aspect_ratio;
public:
	ResizeAtom() = delete;

	ResizeAtom(const shared_ptr<Atom>& base, const string& ws, const string& hs, bool keep) {
		_type = base->_type;
		_base = base;
		_keep_aspect_ratio = keep;
		auto w = SpaceAtom::getLength(ws);
		auto h = SpaceAtom::getLength(hs);
		_wu = (int) w.first;
		_w = w.second;
		_hu = (int) h.first;
		_h = h.second;
	}

	int getLeftType() const override {
		return _base->getLeftType();
	}

	int getRightType() const override {
		return _base->getRightType();
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto bbox = _base->createBox(env);
		if (_wu == -1 && _hu == -1)
			return bbox;
		float sx = 1.f, sy = 1.f;
		if (_wu != -1 && _hu != -1) {
			sx = _w * SpaceAtom::getFactor(_wu, env) / bbox->_width;
			sy = _h * SpaceAtom::getFactor(_hu, env) / bbox->_height;
			if (_keep_aspect_ratio) {
				sx = min(sx, sy);
				sy = sx;
			}
		} else if (_wu != -1 && _hu == -1) {
			sx = _w * SpaceAtom::getFactor(_wu, env) / bbox->_width;
			sy = sx;
		} else {
			sx = _h * SpaceAtom::getFactor(_hu, env) / bbox->_height;
			sy = sx;
		}

		return shared_ptr<Box>(new ScaleBox(bbox, sx, sy));
	}
};

/**
 * an atom representing an nth-root construction
 */
class NthRoot : public Atom {
private:
	static const string _sqrtSymbol;
	static const float FACTOR;
	// base atom to be put under the root sign
	shared_ptr<Atom> _base;
	// root atom to be put in the upper left corner above the root sign
	shared_ptr<Atom> _root;
public:
	NthRoot() = delete;

	NthRoot(const shared_ptr<Atom>& base, const shared_ptr<Atom>& root) {
		_base = base == nullptr ? shared_ptr<Atom>(new EmptyAtom()) : base;
		_root = root == nullptr ? shared_ptr<Atom>(new EmptyAtom()) : root;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;
};

/**
 * an atom representing a rotated atom
 */
class RotateAtom : public Atom {
private:
	shared_ptr<Atom> _base;
	float _angle;
	int _option;
	int _xunit, _yunit;
	float _x, _y;
public:
	RotateAtom() = delete;

	RotateAtom(const shared_ptr<Atom>& base, const wstring& angle, const wstring& option);

	RotateAtom(const shared_ptr<Atom>& base, float angle, const wstring& option);

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;
};

class RuleAtom : public Atom {
private:
	int _wu, _hu, _ru;
	float _w, _h, _r;
public:
	RuleAtom() = delete;

	RuleAtom(int wu, float w, int hu, float h, int ru, float r) {
		_wu = wu;
		_hu = hu;
		_ru = ru;
		_w = w;
		_h = h;
		_r = r;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		float w = SpaceAtom::getFactor(_wu, env) * _w;
		float h = SpaceAtom::getFactor(_hu, env) * _h;
		float r = SpaceAtom::getFactor(_ru, env) * _r;
		return shared_ptr<Box>(new HorizontalRule(h, w, r));
	}
};

/**
 * an atom representing a small Capital atom
 */
class SmallCpaAtom : public Atom {
private:
	shared_ptr<Atom> _base;
public:
	SmallCpaAtom() = delete;

	SmallCpaAtom(const shared_ptr<Atom>& base) : _base(base) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		bool prev = env.getSmallCap();
		env.setSmallCap(true);
		auto box = _base->createBox(env);
		env.setSmallCap(prev);
		return box;
	}
};

/**
 * an atom representing a sans-serif atom
 */
class SsAtom : public Atom {
private:
	shared_ptr<Atom> _base;
public:
	SsAtom() = delete;

	SsAtom(const shared_ptr<Atom>& base) : _base(base) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		bool prev = env.getTeXFont()->getSs();
		env.getTeXFont()->setSs(true);
		auto box = _base->createBox(env);
		env.getTeXFont()->setSs(prev);
		return box;
	}
};

/**
 * an atom representing a strike through atom
 */
class StrikeThroughAtom : public Atom {
private:
	shared_ptr<Atom> _at;
public:
	StrikeThroughAtom(const shared_ptr<Atom>& a) : _at(a) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		TeXFont& tf = *(env.getTeXFont());
		int style = env.getStyle();
		float axis = tf.getAxisHeight(style);
		float drt = tf.getDefaultRuleThickness(style);
		auto b = _at->createBox(env);
		HorizontalRule* rule = new HorizontalRule(drt, b->_width, -axis + drt, false);
		HorizontalBox* hb = new HorizontalBox();
		hb->add(b);
		hb->add(shared_ptr<Box>(new StrutBox(-b->_width, 0, 0, 0)));
		hb->add(shared_ptr<Box>(rule));

		return shared_ptr<Box>(hb);
	}
};

/**
 * an atom representing a modification of style in a formula
 * (e.g. text-style or display-style)
 */
class StyleAtom : public Atom {
private:
	int _style;
	shared_ptr<Atom> _at;
public:
	StyleAtom() = delete;

	StyleAtom(int style, const shared_ptr<Atom>& a) {
		_style = style;
		_at = a;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		int s = env.getStyle();
		env.setStyle(_style);
		auto box = _at->createBox(env);
		env.setStyle(s);
		return box;
	}
};

/**
 * an atom representing an t with a Caron
 */
class TCaronAtom : public Atom {
public:
	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		Char a = env.getTeXFont()->getChar("textapos", env.getStyle());
		CharBox* A = new CharBox(a);
		Char t = env.getTeXFont()->getChar('t', "mathnormal", env.getStyle());
		CharBox* T = new CharBox(t);
		HorizontalBox* hb = new HorizontalBox(shared_ptr<Box>(T));
		hb->add(SpaceAtom(UNIT_EM, -0.3f, 0, 0).createBox(env));
		hb->add(shared_ptr<Box>(A));
		return shared_ptr<Box>(hb);
	}
};

class TextCircledAtom : public Atom {
private:
	shared_ptr<Atom> _at;
public:
	TextCircledAtom() = delete;

	TextCircledAtom(const shared_ptr<Atom>& a) : _at(a) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto circle = SymbolAtom::get("bigcirc")->createBox(env);
		circle->_shift = -0.07f * SpaceAtom::getFactor(UNIT_EX, env);
		auto box = _at->createBox(env);
		HorizontalBox* hb = new HorizontalBox(box, circle->_width, ALIGN_CENTER);
		hb->add(shared_ptr<Box>(new StrutBox(-hb->_width, 0, 0, 0)));
		hb->add(circle);
		return shared_ptr<Box>(hb);
	}
};

/**
 * an atom representing a modification of style in a formula
 */
class TextStyleAtom : public Atom {
private:
	string _style;
	shared_ptr<Atom> _at;
public:
	TextStyleAtom() = delete;

	TextStyleAtom(const shared_ptr<Atom>& a, const string& style) :
		_style(style), _at(a) {
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		string prev = env.getTextStyle();
		env.setTextStyle(_style);
		auto box = _at->createBox(env);
		env.setTextStyle(prev);
		return box;
	}
};

/**
 * an atom with a stroked T
 */
class TStrokeAtom : public Atom {
private:
	bool _upper;
public:
	TStrokeAtom() = delete;

	TStrokeAtom(bool u) : _upper(u) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		Char ch = env.getTeXFont()->getChar("bar", env.getStyle());
		float italic = ch.getItalic();
		Char t = env.getTeXFont()->getChar(_upper ? 'T' : 't', "mathnormal", env.getStyle());
		CharBox* T = new CharBox(t);
		CharBox* B = new CharBox(ch);
		Box* y = nullptr;
		if (abs(italic) > PREC) {
			y = new HorizontalBox(shared_ptr<Box>(new StrutBox(-italic, 0, 0, 0)));
			y->add(shared_ptr<Box>(B));
		} else {
			y = B;
		}
		Box* b = new HorizontalBox(shared_ptr<Box>(y), T->_width, ALIGN_CENTER);
		VerticalBox* vb = new VerticalBox();
		vb->add(shared_ptr<Box>(T));
		vb->add(shared_ptr<Box>(new StrutBox(0, -0.5f * T->_width, 0, 0)));
		vb->add(shared_ptr<Box>(b));
		return shared_ptr<Box>(vb);
	}
};

/**
 * an atom representing a typewriter atom
 */
class TtAtom : public Atom {
private:
	shared_ptr<Atom> _base;
public:
	TtAtom() = delete;

	TtAtom(const shared_ptr<Atom>& base) : _base(base) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		bool prev = env.getTeXFont()->getTt();
		env.getTeXFont()->setTt(true);
		auto box = _base->createBox(env);
		env.getTeXFont()->setTt(prev);
		return box;
	}
};

/**
 * an atom representing another atom with a line under it
 */
class UnderlinedAtom : public Atom {
private:
	shared_ptr<Atom> _base;
public:
	UnderlinedAtom() = delete;

	UnderlinedAtom(const shared_ptr<Atom>& f) : _base(f) {
		_type = TYPE_ORDINARY;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());

		// create formula box in same style
		auto b = _base == nullptr ? shared_ptr<Box>(new StrutBox(0, 0, 0, 0)) : _base->createBox(env);

		// create vertical box
		VerticalBox* vb = new VerticalBox();
		vb->add(b);
		vb->add(shared_ptr<Box>(new StrutBox(0, 3 * drt, 0, 0)));
		vb->add(shared_ptr<Box>(new HorizontalRule(drt, b->_width, 0)));

		// baseline vertical box = baseline box b
		// there's also an invisible strut of height drt under the rule
		vb->_depth = b->_depth + 5 * drt;
		vb->_height = b->_height;

		return shared_ptr<Box>(vb);
	}
};

/**
 * an atom representing an other atom with an extensible arrow or double-arrow
 * over or under it
 */
class UnderOverArrowAtom : public Atom {
private:
	shared_ptr<Atom> _base;
	bool _over, _left, _dble;
public:
	UnderOverArrowAtom() = delete;

	UnderOverArrowAtom(const shared_ptr<Atom>& base, bool left, bool over) {
		_base = base;
		_left = left;
		_over = over;
		_dble = false;
	}

	UnderOverArrowAtom(const shared_ptr<Atom>& base, bool over) {
		_base = base;
		_over = over;
		_dble = true;
		_left = false;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;
};

/**
 * an atom representing another atom vertically centered with respect to
 * the axis (determined by a general TeXFont parameter)
 */
class VCenteredAtom : public Atom {
private:
	shared_ptr<Atom> _at;
public:
	VCenteredAtom() = delete;

	VCenteredAtom(const shared_ptr<Atom>& a) : _at(a) {}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto b = _at->createBox(env);

		float total = b->_height + b->_depth;
		float axis = env.getTeXFont()->getAxisHeight(env.getStyle());

		// center on axis
		b->_shift = -(total / 2) - axis;

		return shared_ptr<Box>(new HorizontalBox(b));
	}
};

/**
 * an atom representing vdots
 */
class VdotsAtom : public Atom {
public:
	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override {
		auto dot = SymbolAtom::get("ldotp")->createBox(env);
		VerticalBox* vb = new VerticalBox(dot, 0, ALIGN_BOTTOM);
		auto b = SpaceAtom(UNIT_MU, 0, 4, 0).createBox(env);
		vb->add(b);
		vb->add(dot);
		vb->add(b);
		vb->add(dot);
		float d = vb->_depth;
		float h = vb->_height;
		vb->_depth = 0;
		vb->_height = d + h;

		return shared_ptr<Box>(vb);
	}
};

/**
 * An atom representing an extensible left or right arrow to handle xleftarrow
 * and xrightarrow commands in LaTeX.
 */
class XArrowAtom : public Atom {
private:
	shared_ptr<Atom> _over, _under;
	bool _left;
public:
	XArrowAtom() = delete;

	XArrowAtom(const shared_ptr<Atom>& over, const shared_ptr<Atom>& under, bool left) {
		_over = over;
		_under = under;
		_left = left;
	}

	shared_ptr<Box> createBox(_out_ TeXEnvironment& env) override;
};

}
}

#endif // ATOM_IMPL_H_INCLUDED
