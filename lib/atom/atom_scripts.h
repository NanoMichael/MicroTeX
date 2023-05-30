#ifndef MICROTEX_ATOM_SCRIPTS_H
#define MICROTEX_ATOM_SCRIPTS_H

#include "atom/atom.h"
#include "atom/atom_space.h"
#include "box/box_group.h"
#include "box/box_single.h"
#include "utils/utils.h"

namespace microtex {

struct ScriptResult {
  sptr<Box> base = nullptr;
  sptr<Box> scripts = nullptr;
  float space = 0.f;
  float kern = 0.f;
  float widthReduce = 0.f;
  float italic = 0.f;
};

/** An atom representing scripts to be attached to another atom */
class ScriptsAtom : public Atom {
public:
  // base atom
  sptr<Atom> _base;
  // subscript and superscript to be attached to the base
  sptr<Atom> _sub;
  sptr<Atom> _sup;
  bool _onRight = true;

  ScriptsAtom() = delete;

  ScriptsAtom(
    const sptr<Atom>& base,
    const sptr<Atom>& sub,
    const sptr<Atom>& sup,
    bool onRight = true
  )
      : _base(base), _sub(sub), _sup(sup), _onRight(onRight) {}

  AtomType leftType() const override { return _base == nullptr ? _type : _base->leftType(); }

  AtomType rightType() const override { return _base == nullptr ? _type : _base->rightType(); }

  sptr<Box> createBox(Env& env) override;

  ScriptResult createScripts(Env& env);
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_SCRIPTS_H
