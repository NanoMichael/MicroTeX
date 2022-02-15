#ifndef LATEX_ATOM_ZSTACK_H
#define LATEX_ATOM_ZSTACK_H

#include "atom/atom.h"
#include "utils/utils.h"
#include "env/units.h"

namespace tinytex {

struct ZStackArgs {
  Alignment align;
  Dimen offset;
};

class ZStackAtom : public Atom {
private:
  ZStackArgs _hargs, _vargs;
  sptr<Atom> _atom, _anchor;

public:
  ZStackAtom(
    const ZStackArgs& hargs, const ZStackArgs& vargs,
    const sptr<Atom>& atom, const sptr<Atom>& anchor
  ) : _hargs(hargs), _vargs(vargs), _atom(atom), _anchor(anchor) {}

  AtomType leftType() const override {
    return _anchor->leftType();
  }

  AtomType rightType() const override {
    return _anchor->rightType();
  }

  sptr<Box> createBox(Env& env) override;
};

}

#endif //LATEX_ATOM_ZSTACK_H
