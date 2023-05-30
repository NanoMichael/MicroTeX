#ifndef MICROTEX_ATOM_ROOT_H
#define MICROTEX_ATOM_ROOT_H

#include "atom/atom.h"
#include "atom/atom_basic.h"

namespace microtex {

/** An atom representing an nth-root construction */
class NthRoot : public Atom {
private:
  // base atom to be put under the root sign
  sptr<Atom> _base;
  // root atom to be put in the upper left corner above the root sign
  sptr<Atom> _root;

public:
  NthRoot() = delete;

  NthRoot(const sptr<Atom>& base, const sptr<Atom>& root) : _base(base), _root(root) {}

  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_ROOT_H
