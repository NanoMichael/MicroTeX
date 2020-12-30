#ifndef ATOM_BASIC_H_INCLUDED
#define ATOM_BASIC_H_INCLUDED

#include <bitset>
#include <map>
#include <string>

#include "atom/atom.h"
#include "atom/box.h"
#include "common.h"
#include "graphic/graphic.h"

namespace tex {

struct CharFont;
class TeXFont;
struct FontInfos;
class TeXFormula;
class Dummy;
class RowAtom;

/**
 * An empty atom
 */
class EmptyAtom : public Atom {
public:
  sptr<Box> createBox(TeXEnvironment& env) override {
    return sptr<Box>(new StrutBox(0, 0, 0, 0));
  }

  __decl_clone(EmptyAtom)
};

/**
 * A placeholder atom
 */
class PlaceholderAtom : public Atom {
private:
  float _w, _h, _d, _s;

public:
  PlaceholderAtom(float w, float h, float d, float s)
      : _w(w), _h(h), _d(d), _s(s) {}

  sptr<Box> createBox(TeXEnvironment& env) override {
    return sptr<Box>(new StrutBox(_w, _h, _d, _s));
  }

  __decl_clone(PlaceholderAtom)
};

/**
 * The string rendering is made in using Graphics2D
 */
class TextRenderingAtom : public Atom {
private:
  std::wstring _str;
  int _type;
  const FontInfos* _infos;

public:
  TextRenderingAtom() = delete;

  TextRenderingAtom(const std::wstring& str, int type) : _str(str), _type(type), _infos(nullptr) {}

  TextRenderingAtom(const std::wstring& str, const FontInfos* info)
      : _str(str), _type(0), _infos(info) {}

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(TextRenderingAtom)
};

/**
 * An atom representing a smashed atom (i.e. with no height and no depth)
 */
class SmashedAtom : public Atom {
private:
  sptr<Atom> _at;
  bool _h, _d;

public:
  SmashedAtom() = delete;

  SmashedAtom(const sptr<Atom>& a, const std::string& opt) : _h(true), _d(true) {
    _at = a;
    if (opt == "opt")
      _d = false;
    else if (opt == "b")
      _h = false;
  }

  SmashedAtom(const sptr<Atom>& a) : _at(a), _h(true), _d(true) {}

  sptr<Box> createBox(TeXEnvironment& env) override {
    sptr<Box> b = _at->createBox(env);
    if (_h) b->_height = 0;
    if (_d) b->_depth = 0;
    return b;
  }

  __decl_clone(SmashedAtom)
};

/**
 * An atom representing a scaled atom
 */
class ScaleAtom : public Atom {
protected:
  sptr<Atom> _base;

private:
  float _sx, _sy;

public:
  ScaleAtom() = delete;

  ScaleAtom(const sptr<Atom>& base, float sx, float sy) {
    _type = base->_type;
    _base = base;
    _sx = sx;
    _sy = sy;
  }

  ScaleAtom(const sptr<Atom>& base, float scale)
      : ScaleAtom(base, scale, scale) {}

  AtomType leftType() const override { return _base->leftType(); }

  AtomType rightType() const override { return _base->rightType(); }

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(ScaleAtom)
};

/**
 * An atom representing a math atom
 */
class MathAtom : public Atom {
private:
  int _style;
  sptr<Atom> _base;

public:
  MathAtom() = delete;

  MathAtom(const sptr<Atom>& base, int style) {
    _base = base;
    _style = style;
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(MathAtom)
};

/**
 * An atom representing a horizontal-line in array environment
 */
class HlineAtom : public Atom {
private:
  float _width, _shift;
  color _color;

public:
  HlineAtom() : _color(trans), _width(0), _shift(0) {}

  inline void setWidth(float w) {
    _width = w;
  }

  inline void setShift(float s) {
    _shift = s;
  }

  inline void setColor(color c) {
    _color = c;
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(HlineAtom)
};

/**
 * An atom representing a cumulative scripts atom
 */
class CumulativeScriptsAtom : public Atom {
private:
  sptr<Atom> _base;
  sptr<RowAtom> _sup, _sub;

public:
  CumulativeScriptsAtom() = delete;

  CumulativeScriptsAtom(const sptr<Atom>& base, const sptr<Atom>& sub, const sptr<Atom>& sup);

  void addSuperscript(const sptr<Atom>& sup);

  void addSubscript(const sptr<Atom>& sub);

  sptr<Atom> getScriptsAtom() const;

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(CumulativeScriptsAtom)
};

/**
 * A "composed atom": An atom that consists of child atoms that will be
 * displayed next to each other horizontally with glue between them.
 */
class Row {
public:
  /**
   * Sets the given dummy containing the atom that comes just before the first
   * child atom of this "composed atom". This method will always be called by
   * another composed atom, so this composed atom will be a child of it
   * (nested). This is necessary to determine the glue to insert between the
   * first child atom of this nested composed atom and the atom that the dummy
   * contains.
   *
   * @param dummy
   *      the dummy that comes just before this "composed atom"
   */
  virtual void setPreviousAtom(const sptr<Dummy>& dummy) = 0;
};

/**
 * An atom representing whitespace. The dimension values can be set using different
 * unit types.
 */
class SpaceAtom : public Atom {
private:
  static const int _units_count;
  static const std::map<std::string, UnitType> _units;
  static const std::function<float(const TeXEnvironment&)> _unitConversions[];
  // whether a hard space should be represented
  bool _blankSpace;
  // thin-mu-skip, med-mu-skip, thick-mu-skip
  SpaceType _blankType;
  // dimensions
  float _width, _height, _depth;
  // units of the dimensions
  UnitType _wUnit, _hUnit, _dUnit;

  void init() {
    _blankSpace = false;
    _blankType = SpaceType::none;
    _width = _height = _depth = 0;
    _wUnit = _hUnit = _dUnit = UnitType::em;
  }

public:
  SpaceAtom() {
    init();
    _blankSpace = true;
  }

  SpaceAtom(SpaceType type) {
    init();
    _blankSpace = true;
    _blankType = type;
  }

  SpaceAtom(UnitType unit, float width, float height, float depth) {
    init();
    _wUnit = _hUnit = _dUnit = unit;
    _width = width;
    _height = height;
    _depth = depth;
  }

  SpaceAtom(UnitType wu, float w, UnitType hu, float h, UnitType du, float d) {
    init();
    _wUnit = wu, _hUnit = hu, _dUnit = du;
    _width = w, _height = h, _depth = d;
  }

  inline static UnitType getUnit(const std::string& unit) {
    auto i = _units.find(unit);
    if (i == _units.end()) return UnitType::pixel;
    return i->second;
  }

  inline static float getFactor(UnitType unit, const TeXEnvironment& env) {
    return _unitConversions[static_cast<int8>(unit)](env);
  }

  inline static float getSize(UnitType unit, float size, const TeXEnvironment& env) {
    return _unitConversions[static_cast<int8>(unit)](env) * size;
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  /**
   * Get the unit and length from given string. The string must be in the format: a number
   * following with the unit (e.g. 10px, 1cm, 8.2em, ...) or (UnitType::pixel, 0) will be returned.
   */
  static std::pair<UnitType, float> getLength(const std::string& lgth);

  /**
   * Get the unit and length from given string. The string must be in the format: a number
   * following with the unit (e.g. 10px, 1cm, 8.2em, ...) or (UnitType::pixel, 0) will be returned.
   */
  static std::pair<UnitType, float> getLength(const std::wstring& lgth);

  __decl_clone(SpaceAtom)
};

/**
 * An atom representing an underscore
 */
class UnderScoreAtom : public Atom {
public:
  static SpaceAtom _w, _s;

  UnderScoreAtom() {}

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(UnderScoreAtom)
};

/**
 * An atom representing a middle atom which must be rounded by a left and right
 * delimiter.
 */
class MiddleAtom : public Atom {
public:
  sptr<Atom> _base;
  sptr<Box> _box;

  MiddleAtom() = delete;

  MiddleAtom(const sptr<Atom>& a)
      : _base(a), _box(new StrutBox(0, 0, 0, 0)) {}

  sptr<Box> createBox(TeXEnvironment& env) override {
    return _box;
  }

  __decl_clone(MiddleAtom)
};

/**
 * An common superclass for atoms that represent one single character and access
 * the font information.
 */
class CharSymbol : public Atom {
private:
  /**
   * Mrow will mark certain CharSymbol atoms as a text symbol. Msubsup wil use
   * this property for a certain spacing rule.
   */
  bool _textSymbol;

public:
  CharSymbol() : _textSymbol(false) {}

  /**
   * Mark as text symbol (used by Dummy)
   */
  inline void markAsTextSymbol() {
    _textSymbol = true;
  }

  /**
   * Remove the mark so the atom remains unchanged (used by Dummy)
   */
  inline void removeMark() {
    _textSymbol = false;
  }

  /**
   * Tests if this atom is marked as a text symbol (used by Msubsup)
   *
   * @return whether this CharSymbol is marked as a text symbol
   */
  bool isMarkedAsTextSymbol() {
    return _textSymbol;
  }

  /**
   * Get the CharFont-object that uniquely identifies the character that is
   * represented by this atom.
   *
   * @param tf
   *       the TeXFont containing all font related information
   * @return a CharFont
   */
  virtual sptr<CharFont> getCharFont(TeXFont& tf) = 0;
};

/**
 * An atom representing a fixed character (not depending on a text style).
 */
class FixedCharAtom : public CharSymbol {
private:
  const sptr<CharFont> _cf;

public:
  FixedCharAtom() = delete;

  FixedCharAtom(const sptr<CharFont>& c) : _cf(c) {}

  sptr<CharFont> getCharFont(TeXFont& tf) override;

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(FixedCharAtom)
};

class SymbolAtom : public CharSymbol {
private:
  // contains all defined symbols
  static std::map<std::string, sptr<SymbolAtom>> _symbols;
  // whether it's a delimiter symbol
  bool _delimiter;
  // symbol name
  std::string _name;
  wchar_t _unicode;

public:
  SymbolAtom() = delete;

  /**
   * Constructs a new symbol. This used by "TeXSymbolParser" and the symbol
   * types are guaranteed to be valid.
   *
   * @param name
   *      symbol name
   * @param type
   *      symbol type constant
   * @param del
   *      whether the symbol is a delimiter
   */
  SymbolAtom(const std::string& name, AtomType type, bool del);

  inline SymbolAtom& setUnicode(wchar_t c) {
    _unicode = c;
    return *this;
  }

  inline wchar_t getUnicode() const {
    return _unicode;
  }

  /** @return true if this symbol can act as a delimiter to embrace formulas */
  inline bool isDelimiter() const {
    return _delimiter;
  }

  inline const std::string& getName() const {
    return _name;
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  sptr<CharFont> getCharFont(TeXFont& tf) override;

  static void addSymbolAtom(const std::string& file);

  static void addSymbolAtom(const sptr<SymbolAtom>& sym);

  /**
   * Looks up the name in the table and returns the corresponding SymbolAtom
   * representing the symbol (if it's found).
   *
   * @param name
   *      the name of the symbol
   * @return a SymbolAtom representing the found symbol
   * @throw ex_symbol_not_found
   *      if no symbol with the given name was found
   */
  static sptr<SymbolAtom> get(const std::string& name);

  static void _init_();

#ifdef HAVE_LOG
  friend std::ostream& operator<<(std::ostream& os, const SymbolAtom& s);
#endif  // HAVE_LOG

  __decl_clone(SymbolAtom)
};

/**
 * An atom representing exactly one alphanumeric character and the text style in
 * which it should be drawn.
 */
class CharAtom : public CharSymbol {
private:
  // alphanumeric character
  wchar_t _c;
  // text style (empty means the default text style)
  std::string _textStyle;
  bool _mathMode;

  /**
   * Get the Char-object representing this character ("c") in the right text
   * style
   */
  Char getChar(TeXFont& tf, int style, bool smallCap);

public:
  CharAtom() = delete;

  /**
   * Creates a CharAtom that will represent the given character in the given
   * text style. Null for the text style means the default text style.
   *
   * @param c
   *      the alphanumeric character
   * @param textStyle
   *      the text style in which the character should be drawn
   */
  CharAtom(wchar_t c, const std::string& textStyle)
      : _c(c), _textStyle(textStyle), _mathMode(false) {}

  CharAtom(wchar_t c, const std::string& textStyle, bool mathMode)
      : _c(c), _textStyle(textStyle), _mathMode(mathMode) {}

  inline wchar_t getCharacter() {
    return _c;
  }

  inline bool isMathMode() {
    return _mathMode;
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  sptr<CharFont> getCharFont(TeXFont& tf) override;

  __decl_clone(CharAtom)
};

/**
 * An empty atom just to add a mark.
 */
class BreakMarkAtom : public Atom {
public:
  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(BreakMarkAtom)
};

/**
 * Used by RowAtom. The "textSymbol"-property and the type of An atom can change
 * (according to the TeX-algorithms used). Or this atom can be replaced by a
 * ligature, (if it was a CharAtom). But atoms cannot be changed, otherwise
 * different boxes could be made from the same TeXFormula, and that is not
 * desired! This "dummy atom" makes sure that changes to an atom (during the
 * createBox-method of a RowAtom) will be reset.
 */
class Dummy {
private:
  sptr<Atom> _atom;
  bool _textSymbol = false;

public:
  AtomType type = AtomType::none;

  Dummy() = delete;

  /**
   * Create a new dummy for the given atom
   * @param atom an atom
   */
  Dummy(const sptr<Atom>& atom) {
    _textSymbol = false;
    _atom = atom;
    type = AtomType::none;
  }

  /** @return the changed type, or the old left type if it has not been changed */
  inline AtomType leftType() const {
    return (type != AtomType::none ? type : _atom->leftType());
  }

  /** @return the changed type, or the old right type if it has not been changed */
  inline AtomType rightType() const {
    return (type != AtomType::none ? type : _atom->rightType());
  }

  inline bool isCharSymbol() const {
    CharSymbol* x = dynamic_cast<CharSymbol*>(_atom.get());
    return (x != nullptr);
  }

  inline bool isCharInMathMode() const {
    CharAtom* at = dynamic_cast<CharAtom*>(_atom.get());
    return at != nullptr && at->isMathMode();
  }

  /** This method will only be called if isCharSymbol returns true. */
  inline sptr<CharFont> getCharFont(TeXFont& tf) const {
    return ((CharSymbol*)_atom.get())->getCharFont(tf);
  }

  /**
   * Changes this atom into the given "ligature atom".
   *
   * @param atom the ligature atom
   */
  inline void changeAtom(const sptr<FixedCharAtom>& atom) {
    _textSymbol = false;
    _atom = atom;
    type = AtomType::none;
  }

  inline sptr<Box> createBox(TeXEnvironment& env) {
    if (_textSymbol) ((CharSymbol*)_atom.get())->markAsTextSymbol();
    auto box = _atom->createBox(env);
    if (_textSymbol) ((CharSymbol*)_atom.get())->removeMark();
    return box;
  }

  inline void markAsTextSymbol() {
    _textSymbol = true;
  }

  inline bool isKern() const {
    SpaceAtom* x = dynamic_cast<SpaceAtom*>(_atom.get());
    return (x != nullptr);
  }

  /** Only for row-elements */
  inline void setPreviousAtom(const sptr<Dummy>& prev) {
    Row* row = dynamic_cast<Row*>(_atom.get());
    if (row != nullptr) row->setPreviousAtom(prev);
  }
};

/**
 * An atom representing a horizontal row of other atoms, to be separated by
 * glue. It's also responsible for inserting kerns and ligature.
 */
class RowAtom : public Atom, public Row {
private:
  // set of atom types that make a previous bin atom change to ord
  static std::bitset<16> _binSet;
  // set of atom types that can possibly need a kern or, together
  // with the previous atom, be replaced by a ligature
  static std::bitset<16> _ligKernSet;
  // whether the box generated can be broken
  bool _canBreak;
  // atoms to be displayed horizontally next to each-other
  std::vector<sptr<Atom>> _elements;
  // previous atom (for nested Row atoms)
  sptr<Dummy> _previousAtom;

  void change2Ord(Dummy* cur, Dummy* prev, Atom* next);

  static std::bitset<16> _initBinset_();

  static std::bitset<16> _initLigKernSet_();

public:
  static bool _breakEveywhere;

  bool _lookAtLastAtom;

  RowAtom() : _lookAtLastAtom(false), _canBreak(true) {}

  RowAtom(const sptr<Atom>& el);

  /**
   * Get the atom at the front in the elements
   */
  sptr<Atom> getFirstAtom();

  /**
   * Get and remove the atom at the tail in the elements
   */
  sptr<Atom> popLastAtom();

  /**
   * Get the atom at position
   * @param pos
   *      the position of the atom to retrieve
   */
  sptr<Atom> get(size_t pos);

  /**
   * Indicate the box generated by this atom can be broken or not
   * @param can
   *      indicate whether the box can be broken
   */
  inline void setCanBreak(bool can) {
    _canBreak = can;
  }

  /**
   * Retrieve the size of the elements
   */
  inline size_t size() const {
    return _elements.size();
  }

  /**
   * Push An atom to back
   */
  void add(const sptr<Atom>& el);

  sptr<Box> createBox(TeXEnvironment& env) override;

  void setPreviousAtom(const sptr<Dummy>& prev) override;

  AtomType leftType() const override;

  AtomType rightType() const override;

  __decl_clone(RowAtom)
};

/**
 * An atom representing a vertical row of other atoms.
 */
class VRowAtom : public Atom {
private:
  std::vector<sptr<Atom>> _elements;
  sptr<SpaceAtom> _raise;
  bool _addInterline;

public:
  Alignment _valign = Alignment::none;
  Alignment _halign = Alignment::none;

  VRowAtom();

  VRowAtom(const sptr<Atom>& el);

  inline void setAddInterline(bool addInterline) {
    _addInterline = addInterline;
  }

  inline bool getAddInterline() const {
    return _addInterline;
  }

  inline void setVtop(bool vtop) {
    _valign = Alignment::top;
  }

  inline bool getVtop() const {
    return _valign == Alignment::top;
  }

  void setRaise(UnitType unit, float r);

  sptr<Atom> popLastAtom();

  /**
   * Add an atom at the front
   */
  void add(const sptr<Atom>& el);

  /**
   * Add an atom at the tail
   */
  void append(const sptr<Atom>& el);

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(VRowAtom)
};

/**
 * An atom representing the foreground and background color of an other atom
 */
class ColorAtom : public Atom, public Row {
private:
  static std::map<std::string, color> _colors;
  static const color _default;

  color _background, _color;
  // RowAtom for which the color settings apply
  sptr<RowAtom> _elements;

public:
  ColorAtom() = delete;

  ColorAtom(const sptr<Atom>& atom, color bg, color c);

  ColorAtom(color bg, color c, const sptr<Atom>& old) {
    ColorAtom* a = dynamic_cast<ColorAtom*>(old.get());
    if (a == nullptr) throw ex_invalid_atom_type("Should be a ColorAtom!");
    _elements = a->_elements;
    _background = istrans(bg) ? a->_background : bg;
    _color = istrans(c) ? a->_color : c;
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  AtomType leftType() const override {
    return _elements->leftType();
  }

  AtomType rightType() const override {
    return _elements->rightType();
  }

  void setPreviousAtom(const sptr<Dummy>& prev) override {
    _elements->setPreviousAtom(prev);
  }

  /**
   * Parse color from given name. The name can be one of the following format:
   * [#AARRGGBB] or [AARRGGBB], [gray color], [c,m,y,k], [c;m;y;k], [r,g,b], [r;g;b]
   * or a predefined color name.  Return black if not found.
   */
  static color getColor(std::string name);

  /**
   * Define a color with given name
   */
  static void defineColor(const std::string& name, color c);

  __decl_clone(ColorAtom)
};

/**
 * An atom representing a roman atom
 */
class RomanAtom : public Atom {
public:
  sptr<Atom> _base;

  RomanAtom() = delete;

  RomanAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(RomanAtom)
};

/**
 * An atom representing another atom that should be drawn invisibly
 */
class PhantomAtom : public Atom, public Row {
private:
  sptr<RowAtom> _elements;
  // if show with width, height or depth
  bool _w, _h, _d;

public:
  PhantomAtom() = delete;

  PhantomAtom(const sptr<Atom>& el);

  PhantomAtom(const sptr<Atom>& el, bool w, bool h, bool d);

  AtomType leftType() const override {
    return _elements->leftType();
  }

  AtomType rightType() const override {
    return _elements->rightType();
  }

  void setPreviousAtom(const sptr<Dummy>& prev) override {
    _elements->setPreviousAtom(prev);
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(PhantomAtom)
};

/**
 * An atom representing another atom with an override left-type and right-type
 * this affects the glue inserted before and after this atom.
 */
class TypedAtom : public Atom {
private:
  // override left-type and right-type
  AtomType _leftType, _rightType;
  // atom for which new types are set
  sptr<Atom> _atom;

public:
  TypedAtom() = delete;

  TypedAtom(AtomType lt, AtomType rt, const sptr<Atom>& atom) {
    _leftType = lt;
    _rightType = rt;
    _atom = atom;
    _limitsType = atom->_limitsType;
  }

  sptr<Atom> getBase() {
    _atom->_limitsType = _limitsType;
    return _atom;
  }

  sptr<Box> createBox(TeXEnvironment& env) override {
    return _atom->createBox(env);
  }

  AtomType leftType() const override {
    return _leftType;
  }

  AtomType rightType() const override {
    return _rightType;
  }

  __decl_clone(TypedAtom)
};

/**
 * An atom representing another atom with an accent symbol above it
 */
class AccentedAtom : public Atom {
public:
  // accent symbol
  sptr<SymbolAtom> _accent;
  bool _acc;
  bool _changeSize;

  // base atom
  sptr<Atom> _base, _underbase;

  void init(const sptr<Atom>& base, const sptr<Atom>& acc);

public:
  AccentedAtom() = delete;

  AccentedAtom(const sptr<Atom>& base, const sptr<Atom>& accent) {
    init(base, accent);
  }

  AccentedAtom(
    const sptr<Atom>& base,
    const sptr<Atom>& accent,
    bool changeSize) {
    init(base, accent);
    _changeSize = changeSize;
  }

  /**
   * Create an AccentedAtom from a base atom and an accent symbol defined by
   * its name
   *
   * @param base
   *      base atom
   * @param name
   *      name of the accent symbol to be put over the base atom
   * @throw ex_invalid_symbol_type
   *      if the symbol is not defined as An accent ('acc')
   * @throw ex_symbol_not_found
   *      if there's no symbol defined with the given name
   */
  AccentedAtom(
    const sptr<Atom>& base,
    const std::string& name);

  /**
   * Creates an AccentedAtom from a base atom and an accent symbol defined as
   * a TeXFormula. This is used for parsing MathML.
   *
   * @param base
   *      base atom
   * @param acc
   *      TeXFormula representing an accent (SymbolAtom)
   * @throw ex_invalid_formula
   *      if the given TeXFormula does not represent a single
   *      SymbolAtom (type "TeXConstants.AtomType::accent")
   * @throw ex_invalid_symbol_type
   *      if the symbol is not defined as an accent ('acc')
   */
  AccentedAtom(
    const sptr<Atom>& base,
    const sptr<TeXFormula>& acc);

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(AccentedAtom)
};

/**
 * An atom representing another atom with an atom above it (if not null)
 * separated by a kern and in a smaller size depending on "overScriptSize"
 * and/or an atom under it (if not null) seperated by a kern and in a smaller
 * size depending on "underScriptSize"
 */
class UnderOverAtom : public Atom {
private:
  // base, under script & over script
  sptr<Atom> _base, _under, _over;
  // kerning between base and under and over script
  float _underSpace, _overSpace;
  // unit
  UnitType _underUnit, _overUnit;
  // whether the under over should be drawn in a smaller size
  bool _underSmall, _overSmall;

  static sptr<Box> changeWidth(const sptr<Box>& b, float maxW);

  inline void init() {
    _underSpace = _overSpace = 0;
    _underUnit = _overUnit = UnitType::em;
    _underSmall = _overSmall = false;
  }

public:
  UnderOverAtom() = delete;

  UnderOverAtom(
    const sptr<Atom>& base, const sptr<Atom>& script,
    UnitType unit, float space, bool small, bool over  //
  ) {
    init();
    _base = base;
    if (over) {
      _under = nullptr;
      _underSpace = 0.f;
      _underUnit = UnitType::em;
      _underSmall = false;
      _over = script;
      _overUnit = unit;
      _overSpace = space;
      _overSmall = small;
    } else {
      _under = script;
      _underUnit = unit;
      _underSpace = space;
      _underSmall = small;
      _overSpace = 0.f;
      _over = nullptr;
      _overUnit = UnitType::em;
      _overSmall = false;
    }
  }

  UnderOverAtom(
    const sptr<Atom>& base,
    const sptr<Atom>& under, UnitType underunit, float underspace, bool undersmall,
    const sptr<Atom>& over, UnitType overunit, float overspace, bool oversmall  //
  ) {
    _base = base;
    _under = under;
    _underUnit = underunit;
    _underSpace = underspace;
    _underSmall = undersmall;
    _over = over;
    _overUnit = overunit;
    _overSpace = overspace;
    _overSmall = oversmall;
  }

  AtomType leftType() const override {
    return _base->leftType();
  }

  AtomType rightType() const override {
    return _base->rightType();
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(UnderOverAtom)
};

/**
 * An atom representing scripts to be attached to another atom
 */
class ScriptsAtom : public Atom {
private:
  static SpaceAtom SCRIPT_SPACE;

public:
  // base atom
  sptr<Atom> _base;
  // subscript and superscript to be attached to the base
  sptr<Atom> _sub;
  sptr<Atom> _sup;
  // scripts alignment
  Alignment _align = Alignment::none;

  ScriptsAtom() = delete;

  ScriptsAtom(const sptr<Atom>& base, const sptr<Atom>& sub, const sptr<Atom>& sup) {
    _base = base;
    _sub = sub;
    _sup = sup;
    _align = Alignment::left;
  }

  ScriptsAtom(const sptr<Atom>& base, const sptr<Atom>& sub, const sptr<Atom>& sup, bool left) {
    _base = base;
    _sub = sub;
    _sup = sup;
    _align = Alignment::left;
    if (!left) _align = Alignment::right;
  }

  AtomType leftType() const override {
    return _base == nullptr ? _type : _base->leftType();
  }

  AtomType rightType() const override {
    return _base == nullptr ? _type : _base->rightType();
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(ScriptsAtom)
};

/**
 * An atom representing a "big operator" (or an atom that acts as one) together
 * with its limits
 */
class BigOperatorAtom : public Atom {
private:
  // limits
  sptr<Atom> _under, _over;
  // atom representing a big operator
  sptr<Atom> _base;
  // whether the "limits"-value should be taken into account
  // (otherwise the default rules will be applied)
  bool _limitsSet;
  // whether limits should be drawn over and under the base (<-> as scripts)
  bool _limits;

  void init(const sptr<Atom>& base, const sptr<Atom>& under, const sptr<Atom>& over);

  sptr<Box> createSideSets(TeXEnvironment& env);

  /**
   * Center the given box in a new box that has the given width
   */
  static sptr<Box> changeWidth(const sptr<Box>& b, float maxW);

public:
  BigOperatorAtom() = delete;

  /**
   * Create a new BigOperatorAtom from the given atoms. The default rules the
   * positioning of the limits will be applied.
   *
   * @param base
   *      atom representing the big operator
   * @param under
   *      atom representing the under limit
   * @param over
   *      atom representing the over limit
   */
  BigOperatorAtom(const sptr<Atom>& base, const sptr<Atom>& under, const sptr<Atom>& over) {
    init(base, under, over);
  }

  /**
   * Create a new BigOperatorAtom from the given atoms. Limits will be drawn
   * according to the "limits"-value
   *
   * @param base
   *      atom representing the big operator
   * @param under
   *      atom representing the under limit
   * @param over
   *      atom representing the over limit
   * @param limits
   *      whether limits should be drawn over and under the base (<-> as
   *      scripts)
   */
  BigOperatorAtom(
    const sptr<Atom>& base, const sptr<Atom>& under, const sptr<Atom>& over, bool limits) {
    init(base, under, over);
    _limits = limits;
    _limitsSet = true;
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(BigOperatorAtom)
};

/**
 * An atom represeting scripts around a base atom
 */
class SideSetsAtom : public Atom {
public:
  sptr<Atom> _left, _right, _base;

  SideSetsAtom() = delete;

  SideSetsAtom(const sptr<Atom>& base, const sptr<Atom>& left, const sptr<Atom>& right)
      : _base(base), _left(left), _right(right) {
    _type = AtomType::bigOperator;
    _limitsType = LimitsType::noLimits;
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(SideSetsAtom)
};

/**
 * An atom representing another atom with a delimiter and a script above or
 * under it, with script and delimiter separated by a kerning
 */
class OverUnderDelimiter : public Atom {
private:
  // base and script atom
  sptr<Atom> _base, _script;
  // delimiter symbol
  sptr<SymbolAtom> _symbol;
  // kerning between delimiter and script
  SpaceAtom _kern;
  // whether the delimiter should be positioned above or under the base
  bool _over;

  static float getMaxWidth(const Box* b, const Box* del, const Box* script);

public:
  OverUnderDelimiter() = delete;

  OverUnderDelimiter(
    const sptr<Atom>& base,
    const sptr<Atom>& script,
    const sptr<SymbolAtom>& symbol, UnitType kernunit, float kern, bool over  //
  ) {
    _base = base;
    _script = script;
    _symbol = symbol;
    _kern = SpaceAtom(kernunit, 0, kern, 0);
    _over = over;
    _type = AtomType::inner;
  }

  inline void addScript(const sptr<Atom>& script) {
    _script = script;
  }

  inline bool isOver() {
    return _over;
  }

  sptr<Box> createBox(TeXEnvironment& env) override;

  __decl_clone(OverUnderDelimiter)
};

}  // namespace tex

#endif  // ATOM_BASIC_H_INCLUDED
