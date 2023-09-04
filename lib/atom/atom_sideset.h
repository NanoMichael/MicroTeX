#ifndef MICROTEX_ATOM_SIDESET_H
#define MICROTEX_ATOM_SIDESET_H

#include "atom/atom.h"

namespace microtex {

/** An atom representing scripts around of a base atom */
class SideSetsAtom : public Atom {
public:
  // _base is guaranteed to be valid
  sptr<Atom> _left, _right, _base;
  sptr<Atom> _under, _over;

  SideSetsAtom() = delete;

  SideSetsAtom(const sptr<Atom>& base, const sptr<Atom>& left, const sptr<Atom>& right);

  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_SIDESET_H
