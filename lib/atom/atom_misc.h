#ifndef MICROTEX_ATOM_MISC_H
#define MICROTEX_ATOM_MISC_H

#include "atom/atom.h"
#include "atom/atom_basic.h"
#include "atom/atom_char.h"
#include "atom/atom_vrow.h"
#include "box/box_factory.h"
#include "box/box_group.h"
#include "core/formula.h"
#include "core/split.h"
#include "env/env.h"
#include "env/units.h"
#include "graphic/graphic.h"

namespace microtex {

/** An atom representing a big symbol atom (i.e. sigma) */
class BigSymbolAtom : public Atom {
private:
  int _size;

public:
  const sptr<SymbolAtom> _delim;

  BigSymbolAtom() = delete;

  BigSymbolAtom(const sptr<SymbolAtom>& delim, int size) : _delim(delim), _size(size) {}

  AtomType leftType() const override { return _delim->leftType(); }

  AtomType rightType() const override { return _delim->rightType(); }

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing a lapped atom (i.e. with no width) */
class LapedAtom : public Atom {
private:
  sptr<Atom> _at;
  char _type;

public:
  LapedAtom() = delete;

  LapedAtom(const sptr<Atom>& a, char type);

  sptr<Box> createBox(Env& env) override;
};

class RaiseAtom : public Atom {
private:
  sptr<Atom> _base;
  Dimen _raise, _height, _depth;

public:
  RaiseAtom() = delete;

  RaiseAtom(const sptr<Atom>& base, Dimen raise, Dimen height, Dimen depth)
      : _base(base), _raise(raise), _height(height), _depth(depth) {}

  AtomType leftType() const override { return _base->leftType(); }

  AtomType rightType() const override { return _base->rightType(); }

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing a reflected atom */
class ReflectAtom : public Atom {
private:
  sptr<Atom> _base;

public:
  ReflectAtom() = delete;

  explicit ReflectAtom(const sptr<Atom>& base) : _base(base) { _type = _base->_type; }

  sptr<Box> createBox(Env& env) override { return sptrOf<ReflectBox>(_base->createBox(env)); }
};

/** An atom representing a resize operation on given atom */
class ResizeAtom : public Atom {
private:
  sptr<Atom> _base;
  Dimen _width, _height;
  bool _keepAspectRatio;

public:
  ResizeAtom() = delete;

  ResizeAtom(
    const sptr<Atom>& base,
    const std::string& ws,
    const std::string& hs,
    bool keepAspectRatio
  ) {
    const sptr<Atom>& xbase = base == nullptr ? sptrOf<EmptyAtom>() : base;
    _type = xbase->_type;
    _base = xbase;
    _keepAspectRatio = keepAspectRatio;
    _width = Units::getDimen(ws);
    _height = Units::getDimen(hs);
  }

  AtomType leftType() const override { return _base->leftType(); }

  AtomType rightType() const override { return _base->rightType(); }

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing a rotated atom */
class RotateAtom : public Atom {
private:
  sptr<Atom> _base;
  float _angle;
  Rotation _option;
  Dimen _x, _y;

public:
  RotateAtom() = delete;

  RotateAtom(const sptr<Atom>& base, const std::string& angle, const std::string& option);

  RotateAtom(const sptr<Atom>& base, float angle, const std::string& option);

  sptr<Box> createBox(Env& env) override;
};

class RuleAtom : public Atom {
private:
  Dimen _w, _h, _r;

public:
  RuleAtom() = delete;

  RuleAtom(const Dimen& w, const Dimen& h, const Dimen& r) : _w(w), _h(h), _r(r) {}

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing a strike through atom */
class StrikeThroughAtom : public Atom {
private:
  sptr<Atom> _at;

public:
  explicit StrikeThroughAtom(const sptr<Atom>& a) {
    _at = a == nullptr ? sptrOf<EmptyAtom>() : a;
  }

  sptr<Box> createBox(Env& env) override;
};

/**
 * An atom representing another atom vertically centered with respect to
 * the math axis
 */
class VCenterAtom : public WrapAtom {
public:
  VCenterAtom() = delete;

  explicit VCenterAtom(const sptr<Atom>& a) : WrapAtom(a) {}

  sptr<Box> createBox(Env& env) override;
};

/** An atom representing long division */
class LongDivAtom : public VRowAtom {
private:
  long _divisor, _dividend;

  void calculate(std::vector<std::string>& results) const;

public:
  LongDivAtom() = delete;

  LongDivAtom(long divisor, long dividend);
};

/** An atom representing an atom with lines covered */
class CancelAtom : public Atom {
private:
  sptr<Atom> _base;
  int _cancelType;

public:
  enum CancelType { SLASH, BACKSLASH, CROSS };

  CancelAtom() = delete;

  CancelAtom(const sptr<Atom>& base, int cancelType) : _base(base), _cancelType(cancelType) {}

  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_MISC_H
