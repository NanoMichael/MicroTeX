#ifndef LATEX_ATOM_SCRIPTS_H
#define LATEX_ATOM_SCRIPTS_H

#include "atom/atom.h"
#include "atom/atom_space.h"
#include "box/box_single.h"
#include "utils/enums.h"
#include "utils/utils.h"

namespace tex {

/** An atom representing scripts to be attached to another atom */
class ScriptsAtom : public Atom {
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

  ScriptsAtom(const sptr<Atom>& base, const sptr<Atom>& sub, const sptr<Atom>& sup, bool alignLeft)
    : _base(base), _sub(sub), _sup(sup), _align(alignLeft ? Alignment::left : Alignment::right) {}

  AtomType leftType() const override {
    return _base == nullptr ? _type : _base->leftType();
  }

  AtomType rightType() const override {
    return _base == nullptr ? _type : _base->rightType();
  }

  sptr<Box> createBox(Env& env) override;

  __decl_clone(ScriptsAtom)
};

}

#endif //LATEX_ATOM_SCRIPTS_H
