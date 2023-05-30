#ifndef MICROTEX_ATOM_BASIC_H
#define MICROTEX_ATOM_BASIC_H

#include "atom/atom.h"
#include "atom/atom_row.h"
#include "box/box_single.h"
#include "unimath/math_type.h"

namespace microtex {

class Formula;

/** Atom to mark do not add kern between */
class NokernAtom : public Atom {
public:
  NokernAtom() { _type = AtomType::none; }

  sptr<Box> createBox(Env& env) override { return StrutBox::empty(); }
};

/**
 * An atom representing a modification of style in a formula
 * (e.g. text-style or display-style)
 */
class StyleAtom : public WrapAtom {
private:
  TexStyle _style;

public:
  StyleAtom() = delete;

  StyleAtom(TexStyle style, const sptr<Atom>& a) : _style(style), WrapAtom(a) {}

  sptr<Box> createBox(Env& env) override;
};

/**
 * An atom representing a modification of style in a formula
 * relative to current style
 */
class AStyleAtom : public WrapAtom {
private:
  std::string _name;

public:
  AStyleAtom() = delete;

  AStyleAtom(std::string name, const sptr<Atom>& a) : _name(std::move(name)), WrapAtom(a) {}

  sptr<Box> createBox(Env& env) override;
};

/** An empty atom */
class EmptyAtom : public Atom {
public:
  sptr<Box> createBox(Env& env) override { return StrutBox::empty(); }

  static sptr<EmptyAtom> create() { return sptrOf<EmptyAtom>(); }
};

/** A placeholder atom */
class PlaceholderAtom : public Atom {
private:
  sptr<Box> _box;
  float _italic;

public:
  PlaceholderAtom(float width, float height, float depth, float italic = 0.f) : _italic(italic) {
    _box = sptrOf<StrutBox>(width, height, depth, 0.f);
  }

  PlaceholderAtom(const sptr<Box>& box, float italic = 0.f) : _box(box), _italic(italic) {}

  sptr<Box> createBox(Env& env) override { return _box; }

  inline float italic() const { return _italic; }
};

/** An atom representing a smashed atom (i.e. with no height and no depth) */
class SmashedAtom : public WrapAtom {
private:
  bool _h, _d;

public:
  SmashedAtom() = delete;

  SmashedAtom(const sptr<Atom>& a, const std::string& opt) : _h(true), _d(true), WrapAtom(a) {
    if (opt == "opt")
      _d = false;
    else if (opt == "b")
      _h = false;
  }

  explicit SmashedAtom(const sptr<Atom>& a) : _h(true), _d(true), WrapAtom(a) {}

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing a scaled atom */
class ScaleAtom : public WrapAtom {
private:
  float _sx, _sy;

public:
  ScaleAtom() = delete;

  ScaleAtom(const sptr<Atom>& base, float sx, float sy) noexcept
      : WrapAtom(base), _sx(sx), _sy(sy) {}

  ScaleAtom(const sptr<Atom>& base, float scale) : ScaleAtom(base, scale, scale) {}

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing a math atom */
class MathAtom : public WrapAtom {
private:
  TexStyle _style;

public:
  MathAtom() = delete;

  MathAtom(const sptr<Atom>& base, TexStyle style) noexcept : _style(style), WrapAtom(base) {}

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing a horizontal-line in array environment */
class HlineAtom : public Atom {
private:
  float _width, _shift;
  color _color;

public:
  HlineAtom() noexcept : _color(transparent), _width(0), _shift(0) { _type = AtomType::hline; }

  inline void setWidth(float w) { _width = w; }

  inline void setShift(float s) { _shift = s; }

  inline void setColor(color c) { _color = c; }

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing a cumulative scripts atom */
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

  sptr<Box> createBox(Env& env) override;
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

  sptr<Box> createBox(Env& env) override;

  AtomType leftType() const override { return _elements->leftType(); }

  AtomType rightType() const override { return _elements->rightType(); }

  void setPreviousAtom(const sptr<AtomDecor>& prev) override { _elements->setPreviousAtom(prev); }

  /**
   * Parse color from given name. The name can be one of the following format:
   * [#AARRGGBB] or [AARRGGBB], [gray color], [c,m,y,k], [c;m;y;k], [r,g,b], [r;g;b]
   * or a predefined color name. Return black if not found.
   */
  static color getColor(std::string name);

  /** Define a color with given name */
  static void defineColor(const std::string& name, color c);
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

  AtomType leftType() const override { return _elements->leftType(); }

  AtomType rightType() const override { return _elements->rightType(); }

  void setPreviousAtom(const sptr<AtomDecor>& prev) override { _elements->setPreviousAtom(prev); }

  sptr<Box> createBox(Env& env) override;
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

  sptr<Atom> base() {
    _atom->_limitsType = _limitsType;
    return _atom;
  }

  sptr<Box> createBox(Env& env) override { return _atom->createBox(env); }

  AtomType leftType() const override { return _leftType; }

  AtomType rightType() const override { return _rightType; }
};

class ExtensibleAtom : public Atom {
private:
  std::string _sym;
  bool _vertical;
  std::function<float(const Env&)> _getLen;

public:
  explicit ExtensibleAtom(
    std::string sym,
    std::function<float(const Env&)>&& getLen,
    bool isVertical = false
  )
      : _sym(std::move(sym)), _getLen(getLen), _vertical(isVertical) {}

  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_BASIC_H
