#ifndef LATEX_ATOM_BASIC_H
#define LATEX_ATOM_BASIC_H

#include <bitset>
#include <map>
#include <string>
#include <utility>

#include "atom/atom.h"
#include "atom/atom_row.h"
#include "atom/atom_char.h"
#include "atom/atom_space.h"
#include "atom/atom_stack.h"
#include "atom/atom_accent.h"
#include "atom/atom_scripts.h"
#include "atom/atom_vrow.h"
#include "atom/atom_operator.h"
#include "box/box_single.h"
#include "box/box_group.h"
#include "graphic/graphic.h"

namespace tex {

class Formula;

/** Atom to mark do not add kern between */
class NokernAtom : public Atom {
public:
  NokernAtom() {
    _type = AtomType::none;
  }

  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(NokernAtom)
};

/** Atom to modify math font and style */
class MathFontAtom : public Atom {
private:
  MathStyle _mathStyle;
  std::string _fontName;

public:
  MathFontAtom(MathStyle style, std::string fontName)
    : _mathStyle(style), _fontName(std::move(fontName)) {}

  sptr<Box> createBox(Env& env) override;

  __decl_clone(MathFontAtom);
};

/** An empty atom */
class EmptyAtom : public Atom {
public:
  sptr<Box> createBox(Env& env) override {
    return StrutBox::empty();
  }

  __decl_clone(EmptyAtom)
};

/** A placeholder atom */
class PlaceholderAtom : public Atom {
private:
  sptr<Box> _box;
  float _italic;

public:
  PlaceholderAtom(float width, float height, float depth, float italic = 0.f)
    : _italic(italic) {
    _box = sptrOf<StrutBox>(width, height, depth, 0.f);
  }

  PlaceholderAtom(const sptr<Box>& box, float italic = 0.f)
    : _box(box), _italic(italic) {}

  sptr<Box> createBox(Env& env) override {
    return _box;
  }

  inline float italic() const { return _italic; }

  __decl_clone(PlaceholderAtom)
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

  sptr<Box> createBox(Env& env) override {
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

  sptr<Box> createBox(Env& env) override;

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

  sptr<Box> createBox(Env& env) override;

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

  sptr<Box> createBox(Env& env) override;

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

  sptr<Box> createBox(Env& env) override;

  __decl_clone(CumulativeScriptsAtom)
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

  sptr<Box> createBox(Env& env) override {
    return _box;
  }

  __decl_clone(MiddleAtom)
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

  AtomType leftType() const override {
    return _elements->leftType();
  }

  AtomType rightType() const override {
    return _elements->rightType();
  }

  void setPreviousAtom(const sptr<AtomDecor>& prev) override {
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

  void setPreviousAtom(const sptr<AtomDecor>& prev) override {
    _elements->setPreviousAtom(prev);
  }

  sptr<Box> createBox(Env& env) override;

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

  sptr<Atom> base() {
    _atom->_limitsType = _limitsType;
    return _atom;
  }

  sptr<Box> createBox(Env& env) override {
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

}  // namespace tex

#endif  // LATEX_ATOM_BASIC_H
