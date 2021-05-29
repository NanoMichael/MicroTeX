#ifndef ATOM_BASIC_H_INCLUDED
#define ATOM_BASIC_H_INCLUDED

#include <bitset>
#include <map>
#include <string>
#include <utility>

#include "common.h"
#include "atom/atom_row.h"
#include "atom/atom_char.h"
#include "atom/atom_space.h"
#include "atom/atom.h"
#include "box/box_single.h"
#include "box/box_group.h"
#include "graphic/graphic.h"

namespace tex {

struct CharFont;

class TeXFont;

struct FontInfos;

class Formula;

/** An empty atom */
class EmptyAtom : public Atom {
public:
  sptr<Box> createBox(Environment& env) override {
    return sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f);
  }

  __decl_clone(EmptyAtom)
};

/** A placeholder atom */
class PlaceholderAtom : public Atom {
private:
  float _width, _height, _depth, _shift;

public:
  PlaceholderAtom(float width, float height, float depth, float shift)
    : _width(width), _height(height), _depth(depth), _shift(shift) {}

  sptr<Box> createBox(Environment& env) override {
    return sptrOf<StrutBox>(_width, _height, _depth, _shift);
  }

  __decl_clone(PlaceholderAtom)
};

/** The string rendering is made in using Graphics2D */
class TextRenderingAtom : public Atom {
private:
  std::wstring _str;
  int _type;
  const FontInfos* _infos;

public:
  TextRenderingAtom() = delete;

  TextRenderingAtom(std::wstring str, int type)
    : _str(std::move(str)), _type(type), _infos(nullptr) {}

  TextRenderingAtom(std::wstring str, const FontInfos* info)
    : _str(std::move(str)), _type(0), _infos(info) {}

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(TextRenderingAtom)
};

/** An atom representing a smashed atom (i.e. with no height and no depth) */
class SmashedAtom : public Atom {
private:
  sptr<Atom> _atom;
  bool _h, _d;

public:
  SmashedAtom() = delete;

  SmashedAtom(const sptr<Atom>& a, const std::string& opt) : _h(true), _d(true) {
    _atom = a;
    if (opt == "opt") _d = false;
    else if (opt == "b") _h = false;
  }

  explicit SmashedAtom(const sptr<Atom>& a) : _atom(a), _h(true), _d(true) {}

  sptr<Box> createBox(Environment& env) override {
    sptr<Box> b = _atom->createBox(env);
    if (_h) b->_height = 0;
    if (_d) b->_depth = 0;
    return b;
  }

  __decl_clone(SmashedAtom)
};

/** An atom representing a scaled atom */
class ScaleAtom : public Atom {
protected:
  sptr<Atom> _base;

private:
  float _sx, _sy;

public:
  ScaleAtom() = delete;

  ScaleAtom(const sptr<Atom>& base, float sx, float sy) noexcept
    : _base(base), _sx(sx), _sy(sy) {
    _type = _base->_type;
  }

  ScaleAtom(const sptr<Atom>& base, float scale) : ScaleAtom(base, scale, scale) {}

  AtomType leftType() const override { return _base->leftType(); }

  AtomType rightType() const override { return _base->rightType(); }

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(ScaleAtom)
};

/** An atom representing a math atom */
class MathAtom : public Atom {
private:
  TexStyle _style;
  sptr<Atom> _base;

public:
  MathAtom() = delete;

  MathAtom(const sptr<Atom>& base, TexStyle style) noexcept
    : _base(base), _style(style) {}

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(MathAtom)
};

/** An atom representing a horizontal-line in array environment */
class HlineAtom : public Atom {
private:
  float _width, _shift;
  color _color;

public:
  HlineAtom() noexcept: _color(transparent), _width(0), _shift(0) {}

  inline void setWidth(float w) { _width = w; }

  inline void setShift(float s) { _shift = s; }

  inline void setColor(color c) { _color = c; }

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(HlineAtom)
};

/** An atom representing a cumulative scripts atom */
class CumulativeScriptsAtom : public Atom {
private:
  sptr<Atom> _base;
  sptr<RowAtom> _sup, _sub;

public:
  CumulativeScriptsAtom() = delete;

  CumulativeScriptsAtom(
    const sptr<Atom>& base,
    const sptr<Atom>& sub,
    const sptr<Atom>& sup
  );

  void addSuperscript(const sptr<Atom>& sup);

  void addSubscript(const sptr<Atom>& sub);

  sptr<Atom> getScriptsAtom() const;

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(CumulativeScriptsAtom)
};

/** An atom representing an underscore */
class UnderScoreAtom : public Atom {
private:
  static SpaceAtom _w, _s;

public:
  UnderScoreAtom() = default;

  sptr<Box> createBox(Environment& env) override;

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

  explicit MiddleAtom(const sptr<Atom>& a)
    : _base(a), _box(new StrutBox(0, 0, 0, 0)) {}

  sptr<Box> createBox(Environment& env) override {
    return _box;
  }

  __decl_clone(MiddleAtom)
};

/** An atom representing a vertical row of other atoms. */
class VRowAtom : public Atom {
private:
  std::vector<sptr<Atom>> _elements;
  sptr<SpaceAtom> _raise;
  bool _addInterline;

public:
  Alignment _valign = Alignment::none;
  Alignment _halign = Alignment::none;

  VRowAtom();

  explicit VRowAtom(const sptr<Atom>& el);

  inline void setAddInterline(bool addInterline) {
    _addInterline = addInterline;
  }

  inline bool isAddInterline() const {
    return _addInterline;
  }

  inline void setVtop(bool vtop) {
    _valign = vtop ? Alignment::top : Alignment::center;
  }

  inline bool isVtop() const {
    return _valign == Alignment::top;
  }

  void setRaise(UnitType unit, float r);

  sptr<Atom> popLastAtom();

  /** Add an atom at the front */
  void add(const sptr<Atom>& el);

  /** Add an atom at the tail */
  void append(const sptr<Atom>& el);

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(VRowAtom)
};

/** An atom representing the foreground and background color of an other atom */
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

  sptr<Box> createBox(Environment& env) override;

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
   * or a predefined color name. Return black if not found.
   */
  static color getColor(std::string name);

  /** Define a color with given name */
  static void defineColor(const std::string& name, color c);

  __decl_clone(ColorAtom)
};

/** An atom representing a roman atom */
class RomanAtom : public Atom {
public:
  sptr<Atom> _base;

  RomanAtom() = delete;

  explicit RomanAtom(const sptr<Atom>& base) : _base(base) {}

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(RomanAtom)
};

/** An atom representing another atom that should be drawn invisibly */
class PhantomAtom : public Atom, public Row {
private:
  sptr<RowAtom> _elements;
  // if show with width, height or depth
  bool _w, _h, _d;

public:
  PhantomAtom() = delete;

  explicit PhantomAtom(const sptr<Atom>& el);

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

  sptr<Box> createBox(Environment& env) override;

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

  TypedAtom(AtomType lt, AtomType rt, const sptr<Atom>& atom)
    : _leftType(lt), _rightType(rt), _atom(atom) {
    _limitsType = atom->_limitsType;
  }

  sptr<Atom> getBase() {
    _atom->_limitsType = _limitsType;
    return _atom;
  }

  sptr<Box> createBox(Environment& env) override {
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

/** An atom representing another atom with an accent symbol above it */
class AccentedAtom : public Atom {
public:
  // accent symbol
  sptr<SymbolAtom> _accent;
  bool _acc{};
  bool _changeSize{};

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
    bool changeSize
  ) {
    init(base, accent);
    _changeSize = changeSize;
  }

  /**
   * Create an AccentedAtom from a base atom and an accent symbol defined by
   * its name
   *
   * @param base base atom
   * @param name name of the accent symbol to be put over the base atom
   * @throw ex_invalid_symbol_type if the symbol is not defined as An accent ('acc')
   * @throw ex_symbol_not_found if there's no symbol defined with the given name
   */
  AccentedAtom(const sptr<Atom>& base, const std::string& name);

  /**
   * Creates an AccentedAtom from a base atom and an accent symbol defined as
   * a Formula. This is used for parsing MathML.
   *
   * @param base base atom
   * @param acc Formula representing an accent (SymbolAtom)
   * @throw ex_invalid_formula
   *      if the given Formula does not represent a single
   *      SymbolAtom (type "TeXConstants.AtomType::accent")
   * @throw ex_invalid_symbol_type
   *      if the symbol is not defined as an accent ('acc')
   */
  AccentedAtom(const sptr<Atom>& base, const sptr<Formula>& acc);

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(AccentedAtom)
};

/**
 * An atom representing another atom with an atom above it (if not null)
 * separated by a kern and in a smaller size depending on "overScriptSize"
 * and/or an atom under it (if not null) separated by a kern and in a smaller
 * size depending on "underScriptSize"
 */
class UnderOverAtom : public Atom {
private:
  // base, under script & over script
  sptr<Atom> _base, _under, _over;
  // kerning between base and under and over script
  float _underSpace, _overSpace;
  UnitType _underUnit, _overUnit;
  // whether the under over should be drawn in a smaller size
  bool _underSmall, _overSmall;

  static sptr<Box> changeWidth(const sptr<Box>& b, float maxWidth);

  inline void init() {
    _underSpace = _overSpace = 0;
    _underUnit = _overUnit = UnitType::em;
    _underSmall = _overSmall = false;
  }

public:
  UnderOverAtom() = delete;

  UnderOverAtom(
    const sptr<Atom>& base, const sptr<Atom>& script,
    UnitType unit, float space, bool small, bool over
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
    const sptr<Atom>& over, UnitType overunit, float overspace, bool oversmall
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

  sptr<Box> createBox(Environment& env) override;

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

  ScriptsAtom(const sptr<Atom>& base, const sptr<Atom>& sub, const sptr<Atom>& sup)
    : _base(base), _sub(sub), _sup(sup), _align(Alignment::left) {}

  ScriptsAtom(const sptr<Atom>& base, const sptr<Atom>& sub, const sptr<Atom>& sup, bool left)
    : _base(base), _sub(sub), _sup(sup), _align(left ? Alignment::left : Alignment::right) {}

  AtomType leftType() const override {
    return _base == nullptr ? _type : _base->leftType();
  }

  AtomType rightType() const override {
    return _base == nullptr ? _type : _base->rightType();
  }

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(ScriptsAtom)
};

/**
 * An atom representing a "big operator" (or an atom that acts as one) together
 * with its limits
 */
class BigOperatorAtom : public Atom {
private:
  // limits
  sptr<Atom> _under{}, _over{};
  // atom representing a big operator
  sptr<Atom> _base{};
  // whether the "limits"-value should be taken into account
  // (otherwise the default rules will be applied)
  bool _limitsSet = false;
  // whether limits should be drawn over and under the base (<-> as scripts)
  bool _limits = false;

  void init(const sptr<Atom>& base, const sptr<Atom>& under, const sptr<Atom>& over);

  sptr<Box> createSideSets(Environment& env);

  /** Center the given box in a new box that has the given width */
  static sptr<Box> changeWidth(const sptr<Box>& b, float maxWidth);

public:
  BigOperatorAtom() = delete;

  /**
   * Create a new BigOperatorAtom from the given atoms. The default rules the
   * positioning of the limits will be applied.
   *
   * @param base atom representing the big operator
   * @param under atom representing the under limit
   * @param over atom representing the over limit
   */
  BigOperatorAtom(const sptr<Atom>& base, const sptr<Atom>& under, const sptr<Atom>& over) {
    init(base, under, over);
  }

  /**
   * Create a new BigOperatorAtom from the given atoms. Limits will be drawn
   * according to the "limits"-value
   *
   * @param base atom representing the big operator
   * @param under atom representing the under limit
   * @param over atom representing the over limit
   * @param limits
   *      whether limits should be drawn over and under the base (<-> as
   *      scripts)
   */
  BigOperatorAtom(
    const sptr<Atom>& base, const sptr<Atom>& under, const sptr<Atom>& over, bool limits
  ) {
    init(base, under, over);
    _limits = limits;
    _limitsSet = true;
  }

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(BigOperatorAtom)
};

/** An atom representing scripts around a base atom */
class SideSetsAtom : public Atom {
public:
  sptr<Atom> _left, _right, _base;

  SideSetsAtom() = delete;

  SideSetsAtom(const sptr<Atom>& base, const sptr<Atom>& left, const sptr<Atom>& right)
    : _base(base), _left(left), _right(right) {
    _type = AtomType::bigOperator;
    _limitsType = LimitsType::noLimits;
  }

  sptr<Box> createBox(Environment& env) override;

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
    const sptr<SymbolAtom>& symbol,
    UnitType kernUnit, float kern, bool over
  ) : _base(base), _script(script), _symbol(symbol), _over(over) {
    _kern = SpaceAtom(kernUnit, 0, kern, 0);
    _type = AtomType::inner;
  }

  inline void addScript(const sptr<Atom>& script) {
    _script = script;
  }

  inline bool isOver() {
    return _over;
  }

  sptr<Box> createBox(Environment& env) override;

  __decl_clone(OverUnderDelimiter)
};

}  // namespace tex

#endif  // ATOM_BASIC_H_INCLUDED
