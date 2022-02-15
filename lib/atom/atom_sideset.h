#ifndef LATEX_ATOM_SIDESET_H
#define LATEX_ATOM_SIDESET_H

#include "atom/atom.h"

namespace tinytex {

/** An atom representing scripts around of a base atom */
class SideSetsAtom : public Atom {
public:
  // _base is guaranteed to be valid
  sptr<Atom> _left, _right, _base;
  sptr<Atom> _under, _over;

  SideSetsAtom() = delete;

  SideSetsAtom(const sptr<Atom>& base, const sptr<Atom>& left, const sptr<Atom>& right)
    : _base(base), _left(left), _right(right) {
    _type = AtomType::bigOperator;
    _limitsType = LimitsType::noLimits;
  }

  sptr<Box> createBox(Env& env) override;
};

}

#endif //LATEX_ATOM_SIDESET_H
