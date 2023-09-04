#ifndef ATOM_H_INCLUDED
#define ATOM_H_INCLUDED

#include "box/box.h"

namespace microtex {

class Env;

/**
 * An abstract superclass for all logical mathematical constructions that can be a part
 * of a Formula. The abstract method [createBox(Env)] transforms this logical unit into
 * a concrete box (that can be painted).
 *
 * It also defines its type, used for determining what glue to use between adjacent atoms
 * in a "row construction". That can be one single type by assigning one of the type
 * constants to the [_type] field. But they can also be defined as having two types: a
 * "left type" and a "right type". This can be done by implementing the methods
 * [leftType()] and [rightType()]. The left type will then be used for determining the
 * glue between this atom and the previous one (in a row, if any) and the right type for
 * the glue between this atom and the following one (in a row, if any).
 *
 * And the [_limitsType] used for determining what variant to use when constructing scripts,
 * defaults to [LimitsType::noLimits].
 */
class Atom {
public:
  /** The type of the atom (default value: ordinary atom) */
  AtomType _type = AtomType::ordinary;
  /** The limits type of the atom (default value: nolimits) */
  LimitsType _limitsType = LimitsType::noLimits;
  /** The alignment type of the atom (default value: none) */
  Alignment _alignment = Alignment::none;

  Atom() = default;

  /**
   * Get the type of the leftmost child atom. Most atoms have no child
   * atoms, so the "left type" and the "right type" are the same: the atom's
   * type. This also is the default implementation. But Some atoms are
   * composed of child atoms put one after another in a horizontal row. These
   * atoms must override this method.
   *
   * @return the type of the leftmost child atom
   */
  virtual AtomType leftType() const { return _type; }

  /**
   * Get the type of the rightmost child atom. Most atoms have no child
   * atoms, so the "left type" and the "right type" are the same: the atom's
   * type. This also is the default implementation. But Some atoms are
   * composed of child atoms put one after another in a horizontal row. These
   * atoms must override this method.
   *
   * @return the type of the rightmost child atom
   */
  virtual AtomType rightType() const { return _type; }

  /**
   * Convert this atom into a Box, using properties set by "parent"
   * atoms, like the TeX style, the last used font, color settings, ...
   *
   * @param env the current environment settings
   *
   * @return the resulting box.
   */
  virtual sptr<Box> createBox(Env& env) = 0;

  /** Test if this atom is a single character */
  virtual bool isChar() const { return false; }

  virtual ~Atom() = default;
};

/** Atom decor contains another atom */
class WrapAtom : public Atom {
protected:
  sptr<Atom> _base;

public:
  explicit WrapAtom(const sptr<Atom>& base);

  inline sptr<Atom> base() const { return _base; }

  AtomType leftType() const override {
    return _base == nullptr ? Atom::leftType() : _base->leftType();
  }

  AtomType rightType() const override {
    return _base == nullptr ? Atom::rightType() : _base->rightType();
  }
};

}  // namespace microtex

#endif  // ATOM_H_INCLUDED
