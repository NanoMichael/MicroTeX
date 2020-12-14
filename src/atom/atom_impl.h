#ifndef ATOM_IMPL_H_INCLUDED
#define ATOM_IMPL_H_INCLUDED

#include "atom/atom.h"
#include "atom/box.h"
#include "common.h"
#include "core/core.h"
#include "core/formula.h"
#include "fonts/fonts.h"
#include "graphic/graphic.h"

namespace tex {

inline static void parseMap(const std::string& options, _out_ std::map<std::string, std::string>& res) {
  if (options.empty()) return;

  strtokenizer tokens(options, ",");
  const int c = tokens.count_tokens();
  for (int i = 0; i < c; i++) {
    std::string tok = tokens.next_token();
    trim(tok);
    std::vector<std::string> optarg;
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
 * Atom to justify cells in array
 */
class CellSpecifier : public Atom {
public:
  virtual void apply(const sptr<Box>& box) = 0;

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    return sptr<Box>(new StrutBox(0, 0, 0, 0));
  }
};

/**
 * Atom representing column color in array
 */
class CellColorAtom : public CellSpecifier {
private:
  color _color;

public:
  CellColorAtom() = delete;

  CellColorAtom(color c) : _color(c) {}

  void apply(const sptr<Box>& box) override {
    box->_background = _color;
  }

  __decl_clone(CellColorAtom)
};

/**
 * Atom representing column foreground in array
 */
class CellForegroundAtom : public CellSpecifier {
private:
  color _color;

public:
  CellForegroundAtom() = delete;

  CellForegroundAtom(color c) : _color(c) {}

  void apply(const sptr<Box>& box) override {
    box->_foreground = _color;
  }

  __decl_clone(CellForegroundAtom)
};

/**
 * Atom representing multi-row
 */
class MultiRowAtom : public Atom {
private:
  sptr<Atom> _rows;

public:
  int _i, _j, _n;

  MultiRowAtom() = delete;

  MultiRowAtom(int n, const std::wstring& option, const sptr<Atom> rows)
      : _i(0), _j(0), _rows(rows) {
    _n = n == 0 ? 1 : n;
  }

  inline void setRowColumn(int r, int c) {
    _i = r;
    _j = c;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto b = _rows->createBox(env);
    b->_type = TYPE_MULTIROW;
    return b;
  }

  __decl_clone(MultiRowAtom)
};

class VlineAtom;

/**
 * Enum specifies matrix type
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

/**
 * Atom represents matrix
 */
class MatrixAtom : public Atom {
private:
  static std::map<std::wstring, std::wstring> _colspeReplacement;

  static SpaceAtom _align;

  sptr<ArrayOfAtoms> _matrix;
  std::vector<int> _position;
  std::map<int, sptr<VlineAtom>> _vlines;
  std::map<int, sptr<Atom>> _columnSpecifiers;

  int _ttype;
  bool _ispartial;
  bool _spaceAround;

  void parsePositions(std::wstring opt, _out_ std::vector<int>& lpos);

  sptr<Box> generateMulticolumn(
      _out_ TeXEnvironment& env,
      const sptr<Box>& b,
      const float* hsep,
      const float* colWidth,
      int i,
      int j);

  void recalculateLine(
      const int rows,
      sptr<Box>** boxarr,
      std::vector<sptr<Atom>>& multiRows,
      float* height,
      float* depth,
      float drt,
      float vspace);

  float* getColumnSep(_out_ TeXEnvironment& env, float width);

public:
  // The color to draw the rule of the matrix
  static color LINE_COLOR;

  static SpaceAtom _hsep, _semihsep, _vsep_in, _vsep_ext_top, _vsep_ext_bot;

  static sptr<Box> _nullbox;

  MatrixAtom() = delete;

  MatrixAtom(
      bool ispar,
      const sptr<ArrayOfAtoms>& arr,
      const std::wstring& options,
      bool spaceAround);

  MatrixAtom(
      bool ispar,
      const sptr<ArrayOfAtoms>& arr,
      const std::wstring& options);

  MatrixAtom(
      bool ispar,
      const sptr<ArrayOfAtoms>& arr,
      int type);

  MatrixAtom(
      bool ispar,
      const sptr<ArrayOfAtoms>& arr,
      int type,
      int align);

  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  static void defineColumnSpecifier(const std::wstring& rep, const std::wstring& spe);

  __decl_clone(MatrixAtom)
};

/**
 * An atom representing vertical-line in matrix environment
 */
class VlineAtom : public Atom {
private:
  // Number of lines to draw
  int _n;

public:
  float _height, _shift;

  VlineAtom() = delete;

  VlineAtom(int n) : _n(n), _height(0), _shift(0) {}

  inline float getWidth(_in_ TeXEnvironment& env) {
    if (_n != 0) {
      float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
      return drt * (3 * _n - 2);
    }
    return 0;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    if (_n == 0) return sptr<Box>(new StrutBox(0, 0, 0, 0));

    float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
    sptr<Box> b(new HorizontalRule(_height, drt, _shift, MatrixAtom::LINE_COLOR, true));
    sptr<Box> sep(new StrutBox(2 * drt, 0, 0, 0));
    HorizontalBox* hb = new HorizontalBox();
    for (int i = 0; i < _n - 1; i++) {
      hb->add(b);
      hb->add(sep);
    }

    if (_n > 0) hb->add(b);

    return sptr<Box>(hb);
  }

  __decl_clone(VlineAtom)
};

enum MULTILINETYPE {
  MULTILINE = 0,
  GATHER,
  GATHERED
};

/**
 * An atom representing a vertical row of other atoms
 */
class MultlineAtom : public Atom {
private:
  static SpaceAtom _vsep_in;
  sptr<ArrayOfAtoms> _column;
  int _ttype;
  bool _ispartial;

public:
  MultlineAtom() = delete;

  MultlineAtom(bool ispar, const sptr<ArrayOfAtoms>& col, int type) {
    _ispartial = ispar;
    _ttype = type;
    _column = col;
  }

  MultlineAtom(const sptr<ArrayOfAtoms>& col, int type) {
    _ispartial = false;
    _ttype = type;
    _column = col;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(MultlineAtom)
};

/**
 * An atom representing a big delimiter atom (i.e. sigma)
 */
class BigDelimiterAtom : public Atom {
private:
  int _size;

public:
  sptr<SymbolAtom> _delim;

  BigDelimiterAtom() = delete;

  BigDelimiterAtom(const sptr<SymbolAtom>& delim, int s) : _delim(delim), _size(s) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto b = DelimiterFactory::create(*_delim, env, _size);
    HorizontalBox* hb = new HorizontalBox();
    float h = b->_height;
    float total = h + b->_depth;
    float axis = env.getTeXFont()->getAxisHeight(env.getStyle());
    b->_shift = -total / 2 + h - axis;
    hb->add(b);
    return sptr<Box>(hb);
  }

  __decl_clone(BigDelimiterAtom)
};

/**
 * An atom representing a bold atom
 */
class BoldAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  BoldAtom() = delete;

  BoldAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    if (_base != nullptr) {
      TeXEnvironment& e = *(env.copy(env.getTeXFont()->copy()));
      e.getTeXFont()->setBold(true);
      return _base->createBox(e);
    }
    return sptr<Box>(new StrutBox(0, 0, 0, 0));
  }

  __decl_clone(BoldAtom)
};

/**
 * An atom with cedilla
 */
class CedillAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  CedillAtom() = delete;

  CedillAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto b = _base->createBox(env);
    VerticalBox* vb = new VerticalBox();
    vb->add(b);
    Char ch = env.getTeXFont()->getChar("mathcedilla", env.getStyle());
    float italic = ch.getItalic();
    Box* cedilla = new CharBox(ch);
    Box* y;
    if (abs(italic) > PREC) {
      y = new HorizontalBox(sptr<Box>(new StrutBox(-italic, 0, 0, 0)));
      y->add(sptr<Box>(cedilla));
    } else {
      y = cedilla;
    }

    Box* ce = new HorizontalBox(sptr<Box>(y), b->_width, ALIGN_CENTER);
    float x = 0.4f * SpaceAtom::getFactor(UNIT_MU, env);
    vb->add(sptr<Box>(new StrutBox(0, -x, 0, 0)));
    vb->add(sptr<Box>(ce));
    float f = vb->_height + vb->_depth;
    vb->_height = b->_height;
    vb->_depth = f - b->_height;
    return sptr<Box>(vb);
  }

  __decl_clone(CedillAtom)
};

/**
 * An atom representing ddots
 */
class DdtosAtom : public Atom {
public:
  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto ldots = TeXFormula::get(L"ldots")->_root->createBox(env);
    float w = ldots->_width;
    auto dot = SymbolAtom::get("ldotp")->createBox(env);
    HorizontalBox* hb1 = new HorizontalBox(dot, w, ALIGN_LEFT);
    HorizontalBox* hb2 = new HorizontalBox(dot, w, ALIGN_CENTER);
    HorizontalBox* hb3 = new HorizontalBox(dot, w, ALIGN_RIGHT);
    sptr<Box> pt4(SpaceAtom(UNIT_MU, 0, 4, 0).createBox(env));
    VerticalBox* vb = new VerticalBox();
    vb->add(sptr<Box>(hb1));
    vb->add(pt4);
    vb->add(sptr<Box>(hb2));
    vb->add(pt4);
    vb->add(sptr<Box>(hb3));

    float h = vb->_height + vb->_depth;
    vb->_height = h;
    vb->_depth = 0;
    return sptr<Box>(vb);
  }

  __decl_clone(DdtosAtom)
};

/**
 * An atom representing a boxed base atom
 */
class FBoxAtom : public Atom {
protected:
  sptr<Atom> _base;
  color _bg, _line;

public:
  static const float INTERSPACE;

  FBoxAtom() = delete;

  FBoxAtom(const sptr<Atom>& base, color bg = TRANS, color line = TRANS) {
    if (base == nullptr)
      _base = sptr<Atom>(new RowAtom());
    else {
      _base = base;
      _type = base->_type;
    }
    _bg = bg;
    _line = line;
  }

  virtual sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto bbase = _base->createBox(env);
    float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
    float space = INTERSPACE * SpaceAtom::getFactor(UNIT_EM, env);
    if (istrans(_bg)) return sptr<Box>(new FramedBox(bbase, drt, space));
    env._isColored = true;
    return sptr<Box>(new FramedBox(bbase, drt, space, _line, _bg));
  }

  __decl_clone(FBoxAtom)
};

/**
 * An atom representing a boxed base atom
 */
class DoubleFramedAtom : public FBoxAtom {
public:
  DoubleFramedAtom() = delete;

  DoubleFramedAtom(const sptr<Atom>& base) : FBoxAtom(base) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto bbase = _base->createBox(env);
    float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
    float space = INTERSPACE * SpaceAtom::getFactor(UNIT_EM, env);
    float sspace = 1.5f * drt + 0.5f * SpaceAtom::getFactor(UNIT_POINT, env);
    return sptr<Box>(new FramedBox(
        sptr<Box>(new FramedBox(bbase, 0.75 * drt, space)), 1.5f * drt, sspace));
  }

  __decl_clone(DoubleFramedAtom)
};

/**
 * An atom representing a box-shadowed atom
 */
class ShadowAtom : public FBoxAtom {
public:
  ShadowAtom() = delete;

  ShadowAtom(const sptr<Atom>& base) : FBoxAtom(base) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto x = FBoxAtom::createBox(env);
    auto box = std::dynamic_pointer_cast<FramedBox>(x);
    float t = env.getTeXFont()->getDefaultRuleThickness(env.getStyle()) * 4;
    return sptr<Box>(new ShadowBox(box, t));
  }

  __decl_clone(ShadowAtom)
};

/**
 * An atom representing a oval-boxed base atom
 */
class OvalAtom : public FBoxAtom {
public:
  static float _multiplier;
  static float _diameter;

  OvalAtom() = delete;

  OvalAtom(const sptr<Atom>& base) : FBoxAtom(base) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto x = FBoxAtom::createBox(env);
    auto box = std::dynamic_pointer_cast<FramedBox>(x);
    return sptr<Box>(new OvalBox(box, _multiplier, _diameter));
  }

  __decl_clone(OvalAtom)
};

/**
 * An atom representing a base atom surrounded with delimiters that change their
 * size according to the height of the base
 */
class FencedAtom : public Atom {
private:
  static const int DELIMITER_FACTOR;
  static const float DELIMITER_SHORTFALL;
  // base atom
  sptr<Atom> _base;
  // delimiters
  sptr<SymbolAtom> _left;
  sptr<SymbolAtom> _right;
  std::list<sptr<MiddleAtom>> _middle;

  void init(const sptr<Atom>& b, const sptr<SymbolAtom>& l, const sptr<SymbolAtom>& r);

  static void center(_out_ Box& b, float axis);

public:
  FencedAtom(const sptr<Atom>& b, const sptr<SymbolAtom>& l, const sptr<SymbolAtom>& r) {
    init(b, l, r);
  }

  FencedAtom(
      const sptr<Atom>& b,
      const sptr<SymbolAtom>& l,
      const std::list<sptr<MiddleAtom>>& m,
      const sptr<SymbolAtom>& r) {
    init(b, l, r);
    _middle = m;
  }

  int getLeftType() const override {
    return TYPE_INNER;
  }

  int getRightType() const override {
    return TYPE_INNER;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(FencedAtom)
};

/**
 * An atom representing a fraction
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
  sptr<Atom> _numerator, _denominator;
  // thickness of the fraction line
  float _thickness;
  // thickness of the fraction line relative to the default thickness
  float _deffactor;
  // whether the def-factor value should be used
  bool _deffactorset;

  inline int checkAlign(int align) {
    if (align == ALIGN_LEFT || align == ALIGN_RIGHT) return align;
    return ALIGN_CENTER;
  }

  void init(
      const sptr<Atom>& num,
      const sptr<Atom>& den,
      bool nodef,
      int unit,
      float t);

public:
  /**
     * If add space to start and end of fraction, default is true
     */
  bool _useKern;

  FractionAtom() = delete;

  FractionAtom(const sptr<Atom>& num, const sptr<Atom>& den) {
    init(num, den, false, UNIT_PIXEL, 0.f);
  }

  FractionAtom(const sptr<Atom>& num, const sptr<Atom>& den, bool rule) {
    init(num, den, !rule, UNIT_PIXEL, 0.f);
  }

  FractionAtom(
      const sptr<Atom>& num, const sptr<Atom>& den, bool nodef, int unit, float t) {
    init(num, den, nodef, unit, t);
  }

  FractionAtom(
      const sptr<Atom>& num, const sptr<Atom>& den, bool rule, int numAlign, int denomAlign) {
    init(num, den, !rule, UNIT_PIXEL, 0.f);
    _numAlign = checkAlign(numAlign);
    _denomAlign = checkAlign(denomAlign);
  }

  FractionAtom(
      const sptr<Atom>& num, const sptr<Atom>& den, float deffactor, int numAlign, int denomAlign) {
    init(num, den, false, UNIT_PIXEL, 0.f);
    _numAlign = checkAlign(numAlign);
    _denomAlign = checkAlign(denomAlign);
    _deffactor = deffactor;
    _deffactorset = true;
  }

  FractionAtom(
      const sptr<Atom>& num, const sptr<Atom>& den, int unit, float t, int numAlign, int denomAlign) {
    init(num, den, true, unit, t);
    _numAlign = checkAlign(numAlign);
    _denomAlign = checkAlign(denomAlign);
  }

  FractionAtom(const sptr<Atom>& num, const sptr<Atom>& den, int unit, float t) {
    init(num, den, true, unit, t);
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(FractionAtom)
};

/**
 * An atom used in array mode that across several columns
 */
class MulticolumnAtom : public Atom {
protected:
  // Number of columns across
  int _n;
  int _align;
  float _w;
  int _beforeVlines, _afterVlines;
  int _row, _col;
  sptr<Atom> _cols;

  int parseAlign(const std::string& str);

public:
  MulticolumnAtom() = delete;

  MulticolumnAtom(int n, const std::string& align, const sptr<Atom> cols)
      : _w(0), _beforeVlines(0), _afterVlines(0), _row(0), _col(0) {
    _n = n >= 1 ? n : 1;
    _cols = cols;
    _align = parseAlign(align);
  }

  virtual bool isNeedWidth() const {
    return false;
  }

  inline void setColumnWidth(float w) {
    _w = w;
  }

  inline float getColumnWidth() {
    return _w;
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

  virtual sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(MulticolumnAtom)
};

/**
 * An atom used in array mode representing "dots"
 */
class HdotsforAtom : public MulticolumnAtom {
private:
  float _coeff;

  sptr<Box> createBox(
      float space,
      const sptr<Box>& b,
      _out_ TeXEnvironment& env);

public:
  HdotsforAtom() = delete;

  HdotsforAtom(int n, float coeff)
      : MulticolumnAtom(n, "c", SymbolAtom::get("ldotp")), _coeff(coeff) {}

  bool isNeedWidth() const override {
    return true;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(HdotsforAtom)
};

/**
 * An atom representing id-dots
 */
class IddotsAtom : public Atom {
public:
  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto ldots = TeXFormula::get(L"ldots")->_root->createBox(env);
    float w = ldots->_width;
    auto dot = SymbolAtom::get("ldotp")->createBox(env);
    sptr<Box> hb1(new HorizontalBox(dot, w, ALIGN_RIGHT));
    sptr<Box> hb2(new HorizontalBox(dot, w, ALIGN_CENTER));
    sptr<Box> hb3(new HorizontalBox(dot, w, ALIGN_LEFT));
    sptr<Box> pt4 = SpaceAtom(UNIT_MU, 0, 4, 0).createBox(env);
    VerticalBox* vb = new VerticalBox();
    vb->add(hb1);
    vb->add(pt4);
    vb->add(hb2);
    vb->add(pt4);
    vb->add(hb3);

    float h = vb->_height + vb->_depth;
    vb->_height = h;
    vb->_depth = 0;

    return sptr<Box>(vb);
  }

  __decl_clone(IddotsAtom)
};

/**
 * An atom representing an IJ
 */
class IJAtom : public Atom {
private:
  bool _upper;

public:
  IJAtom() = delete;

  IJAtom(bool upper) : _upper(upper) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    CharBox* I = new CharBox(
        env.getTeXFont()->getChar(_upper ? 'I' : 'i', "mathnormal", env.getStyle()));
    CharBox* J = new CharBox(
        env.getTeXFont()->getChar(_upper ? 'J' : 'j', "mathnormal", env.getStyle()));
    HorizontalBox* hb = new HorizontalBox(sptr<Box>(I));
    hb->add(SpaceAtom(UNIT_EM, -0.065f, 0, 0).createBox(env));
    hb->add(sptr<Box>(J));
    return sptr<Box>(hb);
  }

  __decl_clone(IJAtom)
};

/**
 * An atom representing a italic atom
 */
class ItAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  ItAtom() = delete;

  ItAtom(const sptr<Atom> base) : _base(base) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    sptr<Box> box;
    if (_base != nullptr) {
      TeXEnvironment& e = *(env.copy(env.getTeXFont()->copy()));
      e.getTeXFont()->setIt(true);
      box = _base->createBox(e);
    } else {
      box = sptr<Box>(new StrutBox(0, 0, 0, 0));
    }

    return box;
  }

  __decl_clone(ItAtom)
};

/**
 * An atom representing a lapped atom (i.e. with no width)
 */
class LapedAtom : public Atom {
private:
  sptr<Atom> _at;
  wchar_t _type;

public:
  LapedAtom() = delete;

  LapedAtom(const sptr<Atom>& a, wchar_t type) : _at(a), _type(type) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto b = _at->createBox(env);
    VerticalBox* vb = new VerticalBox();
    vb->add(b);
    vb->_width = 0;
    switch (_type) {
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

    return sptr<Box>(vb);
  }

  __decl_clone(LapedAtom)
};

/**
 * An atom representing LaTeX logo. the dimension values can be set using
 * different unit types.
 */
class LaTeXAtom : public Atom {
public:
  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(LaTeXAtom)
};

/**
 * An atom representing an L with a Caron
 */
class LCaronAtom : public Atom {
private:
  bool _upper;

public:
  LCaronAtom() = delete;

  LCaronAtom(bool upper) : _upper(upper) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    CharBox* A = new CharBox(
        env.getTeXFont()->getChar("textapos", env.getStyle()));
    CharBox* L = new CharBox(
        env.getTeXFont()->getChar(_upper ? 'L' : 'l', "mathnormal", env.getStyle()));
    HorizontalBox* hb = new HorizontalBox(sptr<Box>(L));
    if (_upper)
      hb->add(SpaceAtom(UNIT_EM, -0.3f, 0, 0).createBox(env));
    else
      hb->add(SpaceAtom(UNIT_EM, -0.13f, 0, 0).createBox(env));
    hb->add(sptr<Box>(A));
    return sptr<Box>(hb);
  }

  __decl_clone(LCaronAtom)
};

/**
 * An atom representing a mono scale atom
 */
class MonoScaleAtom : public ScaleAtom {
private:
  float _factor;

public:
  MonoScaleAtom() = delete;

  MonoScaleAtom(const sptr<Atom>& base, float factor)
      : ScaleAtom(base, factor, factor), _factor(factor) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    TeXEnvironment& e = *(env.copy());
    float f = e.getScaleFactor();
    e.setScaleFactor(_factor);
    return sptr<Box>(new ScaleBox(_base->createBox(e), _factor / f));
  }

  __decl_clone(MonoScaleAtom)
};

/**
 * An atom with an Ogonek
 */
class OgonekAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  OgonekAtom() = delete;

  OgonekAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto b = _base->createBox(env);
    VerticalBox* vb = new VerticalBox();
    vb->add(b);
    Char ch = env.getTeXFont()->getChar("ogonek", env.getStyle());
    float italic = ch.getItalic();
    Box* ogonek = new CharBox(ch);
    Box* y = nullptr;
    if (abs(italic) > PREC) {
      y = new HorizontalBox(sptr<Box>(new StrutBox(-italic, 0, 0, 0)));
      y->add(sptr<Box>(ogonek));
    } else {
      y = ogonek;
    }

    Box* og = new HorizontalBox(sptr<Box>(y), b->_width, ALIGN_RIGHT);
    vb->add(sptr<Box>(new StrutBox(0, -ogonek->_height, 0, 0)));
    vb->add(sptr<Box>(og));
    float f = vb->_height + vb->_depth;
    vb->_height = b->_height;
    vb->_depth = f - b->_height;
    return sptr<Box>(vb);
  }

  __decl_clone(OgonekAtom)
};

/**
 * An atom representing a over-lined atom
 */
class OverlinedAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  OverlinedAtom() = delete;

  OverlinedAtom(const sptr<Atom>& f) : _base(f) {
    _type = TYPE_ORDINARY;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
    // cramp the style of the formula to be overlined and create
    // vertical box
    auto b = _base == nullptr ? sptr<Box>(new StrutBox(0, 0, 0, 0))
                              : _base->createBox(*(env.crampStyle()));
    OverBar* ob = new OverBar(b, 3 * drt, drt);

    // baseline vertical box = baseline box b
    ob->_depth = b->_depth;
    ob->_height = b->_height + 5 * drt;

    return sptr<Box>(ob);
  }

  __decl_clone(OverlinedAtom)
};

class RaiseAtom : public Atom {
private:
  sptr<Atom> _base;
  int _ru, _hu, _du;
  float _r, _h, _d;

public:
  RaiseAtom() = delete;

  RaiseAtom(const sptr<Atom>& base, int ru, float r, int hu, float h, int du, float d)
      : _base(base), _ru(ru), _r(r), _hu(hu), _h(h), _du(du), _d(d) {}

  int getLeftType() const override {
    return _base->getLeftType();
  }

  int getRightType() const override {
    return _base->getRightType();
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto bbox = _base->createBox(env);
    if (_ru == -1)
      bbox->_shift = 0;
    else
      bbox->_shift = -_r * SpaceAtom::getFactor(_ru, env);

    if (_hu == -1) return bbox;

    HorizontalBox* hbox = new HorizontalBox(bbox);
    hbox->_height = _h * SpaceAtom::getFactor(_hu, env);
    if (_du == -1)
      hbox->_depth = 0;
    else
      hbox->_depth = _d * SpaceAtom::getFactor(_du, env);

    return sptr<Box>(hbox);
  }

  __decl_clone(RaiseAtom)
};

/**
 * An atom representing a reflected atom
 */
class ReflectAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  ReflectAtom() = delete;

  ReflectAtom(const sptr<Atom>& base) : _base(base) {
    _type = _base->_type;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    return sptr<Box>(new ReflectBox(_base->createBox(env)));
  }

  __decl_clone(ReflectAtom)
};

/**
 * An atom representing a resize atom
 */
class ResizeAtom : public Atom {
private:
  sptr<Atom> _base;
  int _wu, _hu;
  float _w, _h;
  bool _keep_aspect_ratio;

public:
  ResizeAtom() = delete;

  ResizeAtom(const sptr<Atom>& base, const std::string& ws, const std::string& hs, bool keepAspectRatio) {
    _type = base->_type;
    _base = base;
    _keep_aspect_ratio = keepAspectRatio;
    auto w = SpaceAtom::getLength(ws);
    auto h = SpaceAtom::getLength(hs);
    _wu = (int)w.first;
    _w = w.second;
    _hu = (int)h.first;
    _h = h.second;
  }

  int getLeftType() const override {
    return _base->getLeftType();
  }

  int getRightType() const override {
    return _base->getRightType();
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto bbox = _base->createBox(env);
    if (_wu == -1 && _hu == -1) return bbox;
    float sx = 1.f, sy = 1.f;
    if (_wu != -1 && _hu != -1) {
      sx = _w * SpaceAtom::getFactor(_wu, env) / bbox->_width;
      sy = _h * SpaceAtom::getFactor(_hu, env) / bbox->_height;
      if (_keep_aspect_ratio) {
        sx = std::min(sx, sy);
        sy = sx;
      }
    } else if (_wu != -1 && _hu == -1) {
      sx = _w * SpaceAtom::getFactor(_wu, env) / bbox->_width;
      sy = sx;
    } else {
      sx = _h * SpaceAtom::getFactor(_hu, env) / bbox->_height;
      sy = sx;
    }

    return sptr<Box>(new ScaleBox(bbox, sx, sy));
  }

  __decl_clone(ResizeAtom)
};

/**
 * An atom representing an nth-root construction
 */
class NthRoot : public Atom {
private:
  static const std::string _sqrtSymbol;
  static const float FACTOR;
  // base atom to be put under the root sign
  sptr<Atom> _base;
  // root atom to be put in the upper left corner above the root sign
  sptr<Atom> _root;

public:
  NthRoot() = delete;

  NthRoot(const sptr<Atom>& base, const sptr<Atom>& root) {
    _base = base == nullptr ? sptr<Atom>(new EmptyAtom()) : base;
    _root = root == nullptr ? sptr<Atom>(new EmptyAtom()) : root;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(NthRoot)
};

/**
 * An atom representing a rotated atom
 */
class RotateAtom : public Atom {
private:
  sptr<Atom> _base;
  float _angle;
  int _option;
  int _xunit, _yunit;
  float _x, _y;

public:
  RotateAtom() = delete;

  RotateAtom(const sptr<Atom>& base, const std::wstring& angle, const std::wstring& option);

  RotateAtom(const sptr<Atom>& base, float angle, const std::wstring& option);

  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(RotateAtom)
};

class RuleAtom : public Atom {
private:
  int _wu, _hu, _ru;
  float _w, _h, _r;

public:
  RuleAtom() = delete;

  RuleAtom(int wu, float w, int hu, float h, int ru, float r)
      : _wu(wu), _hu(hu), _ru(ru), _w(w), _h(h), _r(r) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    float w = SpaceAtom::getFactor(_wu, env) * _w;
    float h = SpaceAtom::getFactor(_hu, env) * _h;
    float r = SpaceAtom::getFactor(_ru, env) * _r;
    return sptr<Box>(new HorizontalRule(h, w, r));
  }

  __decl_clone(RuleAtom)
};

/**
 * An atom representing a small Capital atom
 */
class SmallCpaAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  SmallCpaAtom() = delete;

  SmallCpaAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    bool prev = env.getSmallCap();
    env.setSmallCap(true);
    auto box = _base->createBox(env);
    env.setSmallCap(prev);
    return box;
  }

  __decl_clone(SmallCpaAtom)
};

/**
 * An atom representing a sans-serif atom
 */
class SsAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  SsAtom() = delete;

  SsAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    bool prev = env.getTeXFont()->isSs();
    env.getTeXFont()->setSs(true);
    auto box = _base->createBox(env);
    env.getTeXFont()->setSs(prev);
    return box;
  }

  __decl_clone(SsAtom)
};

/**
 * An atom representing a strike through atom
 */
class StrikeThroughAtom : public Atom {
private:
  sptr<Atom> _at;

public:
  StrikeThroughAtom(const sptr<Atom>& a) : _at(a) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    TeXFont& tf = *(env.getTeXFont());
    int style = env.getStyle();
    float axis = tf.getAxisHeight(style);
    float drt = tf.getDefaultRuleThickness(style);
    auto b = _at->createBox(env);
    HorizontalRule* rule = new HorizontalRule(drt, b->_width, -axis + drt, false);
    HorizontalBox* hb = new HorizontalBox();
    hb->add(b);
    hb->add(sptr<Box>(new StrutBox(-b->_width, 0, 0, 0)));
    hb->add(sptr<Box>(rule));

    return sptr<Box>(hb);
  }

  __decl_clone(StrikeThroughAtom)
};

/**
 * An atom representing a modification of style in a formula
 * (e.g. text-style or display-style)
 */
class StyleAtom : public Atom {
private:
  int _style;
  sptr<Atom> _at;

public:
  StyleAtom() = delete;

  StyleAtom(int style, const sptr<Atom>& a) {
    _style = style;
    _at = a;
    _type = a->_type;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    int s = env.getStyle();
    env.setStyle(_style);
    auto box = _at->createBox(env);
    env.setStyle(s);
    return box;
  }

  __decl_clone(StyleAtom)
};

/**
 * An atom representing an t with a Caron
 */
class TCaronAtom : public Atom {
public:
  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    Char a = env.getTeXFont()->getChar("textapos", env.getStyle());
    CharBox* A = new CharBox(a);
    Char t = env.getTeXFont()->getChar('t', "mathnormal", env.getStyle());
    CharBox* T = new CharBox(t);
    HorizontalBox* hb = new HorizontalBox(sptr<Box>(T));
    hb->add(SpaceAtom(UNIT_EM, -0.3f, 0, 0).createBox(env));
    hb->add(sptr<Box>(A));
    return sptr<Box>(hb);
  }

  __decl_clone(TCaronAtom)
};

class TextCircledAtom : public Atom {
private:
  sptr<Atom> _at;

public:
  TextCircledAtom() = delete;

  TextCircledAtom(const sptr<Atom>& a) : _at(a) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto circle = SymbolAtom::get("bigcirc")->createBox(env);
    circle->_shift = -0.07f * SpaceAtom::getFactor(UNIT_EX, env);
    auto box = _at->createBox(env);
    HorizontalBox* hb = new HorizontalBox(box, circle->_width, ALIGN_CENTER);
    hb->add(sptr<Box>(new StrutBox(-hb->_width, 0, 0, 0)));
    hb->add(circle);
    return sptr<Box>(hb);
  }

  __decl_clone(TextCircledAtom)
};

/**
 * An atom representing a modification of style in a formula
 */
class TextStyleAtom : public Atom {
private:
  std::string _style;
  sptr<Atom> _at;

public:
  TextStyleAtom() = delete;

  TextStyleAtom(const sptr<Atom>& a, const std::string& style) : _style(style), _at(a) {
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    std::string prev = env.getTextStyle();
    env.setTextStyle(_style);
    auto box = _at->createBox(env);
    env.setTextStyle(prev);
    return box;
  }

  __decl_clone(TextStyleAtom)
};

/**
 * An atom with a stroked T
 */
class TStrokeAtom : public Atom {
private:
  bool _upper;

public:
  TStrokeAtom() = delete;

  TStrokeAtom(bool u) : _upper(u) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    Char ch = env.getTeXFont()->getChar("bar", env.getStyle());
    float italic = ch.getItalic();
    Char t = env.getTeXFont()->getChar(_upper ? 'T' : 't', "mathnormal", env.getStyle());
    CharBox* T = new CharBox(t);
    CharBox* B = new CharBox(ch);
    Box* y = nullptr;
    if (abs(italic) > PREC) {
      y = new HorizontalBox(sptr<Box>(new StrutBox(-italic, 0, 0, 0)));
      y->add(sptr<Box>(B));
    } else {
      y = B;
    }
    Box* b = new HorizontalBox(sptr<Box>(y), T->_width, ALIGN_CENTER);
    VerticalBox* vb = new VerticalBox();
    vb->add(sptr<Box>(T));
    vb->add(sptr<Box>(new StrutBox(0, -0.5f * T->_width, 0, 0)));
    vb->add(sptr<Box>(b));
    return sptr<Box>(vb);
  }

  __decl_clone(TStrokeAtom)
};

/**
 * An atom representing a typewriter atom
 */
class TtAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  TtAtom() = delete;

  TtAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    bool prev = env.getTeXFont()->isTt();
    env.getTeXFont()->setTt(true);
    auto box = _base->createBox(env);
    env.getTeXFont()->setTt(prev);
    return box;
  }

  __decl_clone(TtAtom)
};

/**
 * An atom representing another atom with a line under it
 */
class UnderlinedAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  UnderlinedAtom() = delete;

  UnderlinedAtom(const sptr<Atom>& f) : _base(f) {
    _type = TYPE_ORDINARY;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());

    // create formula box in same style
    auto b = _base == nullptr ? sptr<Box>(new StrutBox(0, 0, 0, 0)) : _base->createBox(env);

    // create vertical box
    VerticalBox* vb = new VerticalBox();
    vb->add(b);
    vb->add(sptr<Box>(new StrutBox(0, 3 * drt, 0, 0)));
    vb->add(sptr<Box>(new HorizontalRule(drt, b->_width, 0)));

    // baseline vertical box = baseline box b
    // there's also an invisible strut of height drt under the rule
    vb->_depth = b->_depth + 5 * drt;
    vb->_height = b->_height;

    return sptr<Box>(vb);
  }

  __decl_clone(UnderlinedAtom)
};

/**
 * An atom representing an other atom with an extensible arrow or double-arrow
 * over or under it
 */
class UnderOverArrowAtom : public Atom {
private:
  sptr<Atom> _base;
  bool _over, _left, _dble;

public:
  UnderOverArrowAtom() = delete;

  UnderOverArrowAtom(const sptr<Atom>& base, bool left, bool over) {
    _base = base;
    _left = left;
    _over = over;
    _dble = false;
  }

  UnderOverArrowAtom(const sptr<Atom>& base, bool over) {
    _base = base;
    _over = over;
    _dble = true;
    _left = false;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(UnderOverArrowAtom)
};

/**
 * An atom representing another atom vertically centered with respect to
 * the axis (determined by a general TeXFont parameter)
 */
class VCenteredAtom : public Atom {
private:
  sptr<Atom> _at;

public:
  VCenteredAtom() = delete;

  VCenteredAtom(const sptr<Atom>& a) : _at(a) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
    auto b = _at->createBox(env);

    float total = b->_height + b->_depth;
    float axis = env.getTeXFont()->getAxisHeight(env.getStyle());

    // center on axis
    b->_shift = -(total / 2) - axis;

    return sptr<Box>(new HorizontalBox(b));
  }

  __decl_clone(VCenteredAtom)
};

/**
 * An atom representing vertical-dots
 */
class VdotsAtom : public Atom {
public:
  sptr<Box> createBox(_out_ TeXEnvironment& env) override {
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

    return sptr<Box>(vb);
  }

  __decl_clone(VdotsAtom)
};

/**
 * An atom representing an extensible left or right arrow to handle xleftarrow
 * and xrightarrow commands in LaTeX.
 */
class XArrowAtom : public Atom {
private:
  sptr<Atom> _over, _under;
  bool _left;

public:
  XArrowAtom() = delete;

  XArrowAtom(const sptr<Atom>& over, const sptr<Atom>& under, bool left) {
    _over = over;
    _under = under;
    _left = left;
  }

  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(XArrowAtom)
};

/**
 * An atom representing long division
 */
class LongDivAtom : public VRowAtom {
private:
  long _divisor, _dividend;

  void calculate(_out_ std::vector<std::wstring>& results);

public:
  LongDivAtom() = delete;

  LongDivAtom(long divisor, long dividend);

  __decl_clone(LongDivAtom)
};

/**
 * An atom representing an atom with lines covered
 */
class CancelAtom : public Atom {
private:
  sptr<Atom> _base;
  int _cancelType;

public:
  enum CancelType {
    SLASH,
    BACKSLASH,
    CROSS
  };

  CancelAtom() = delete;

  CancelAtom(const sptr<Atom>& base, int cancelType)
      : _base(base), _cancelType(cancelType) {}

  sptr<Box> createBox(_out_ TeXEnvironment& env) override;

  __decl_clone(CancelAtom)
};

}  // namespace tex

#endif  // ATOM_IMPL_H_INCLUDED
