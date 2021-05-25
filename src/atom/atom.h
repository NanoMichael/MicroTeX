#ifndef ATOM_H_INCLUDED
#define ATOM_H_INCLUDED

#undef DEBUG

#include <list>

#include "box/box.h"

namespace tex {

/**
 * An abstract superclass for all logical mathematical constructions that can be
 * a part of a Formula. All subclasses must implement the abstract
 * Atom#createBox(Environment) method that transforms this logical unit
 * into a concrete box (that can be painted). They also must define their type,
 * used for determining what glue to use between adjacent atoms in a
 * "row construction". That can be one single type by assigning one of the type
 * constants to the #_type field. But they can also be defined as having
 * two types: a "left type" and a "right type". This can be done by implementing
 * the methods Atom#leftType() and Atom#rightType(). The left type
 * will then be used for determining the glue between this atom and the previous
 * one (in a row, if any) and the right type for the glue between this atom and
 * the following one (in a row, if any).
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
   * Get the type of the leftermost child atom. Most atoms have no child
   * atoms, so the "left type" and the "right type" are the same: the atom's
   * type. This also is the default implementation. But Some atoms are
   * composed of child atoms put one after another in a horizontal row. These
   * atoms must override this method.
   *
   * @return the type of the leftermost child atom
   */
  virtual AtomType leftType() const { return _type; }

  /**
   * Get the type of the rightermost child atom. Most atoms have no child
   * atoms, so the "left type" and the "right type" are the same: the atom's
   * type. This also is the default implementation. But Some atoms are
   * composed of child atoms put one after another in a horizontal row. These
   * atoms must override this method.
   *
   * @return the type of the rightermost child atom
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
  virtual sptr<Box> createBox(Environment& env) = 0;

  /** Shallow clone a atom from this atom. */
  virtual sptr<Atom> clone() const = 0;

  virtual ~Atom() = default;

#ifndef __decl_clone
#define __decl_clone(type) \
  virtual sptr<Atom> clone() const override { return sptr<Atom>(new type(*this)); }
#endif
};

}  // namespace tex

#endif  // ATOM_H_INCLUDED
