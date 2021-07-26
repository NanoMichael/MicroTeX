#ifndef LATEX_ATOM_STACK_H
#define LATEX_ATOM_STACK_H

#include <utility>

#include "atom/atom.h"
#include "env/units.h"

namespace tex {

/** Arguments to place stacked atoms */
struct StackArgs {
  sptr<Atom> atom = nullptr;
  UnitType spaceUnit = UnitType::none;
  float space = 0.f;
  bool isSmall = false;
  bool isAutoSpace = false;

  inline bool isPresent() const {
    return atom != nullptr;
  }

  static inline StackArgs autoSpace(const sptr<Atom>& atom, bool isSmall = true) {
    return {atom, UnitType::none, 0.f, isSmall, true};
  }
};

struct StackResult {
  sptr<Box> box;
  float kernelShift;
};

/**
 * An atom representing another atom with an atom above it (if not null)
 * separated by a kern and in a smaller size depending on "overScriptSize"
 * and/or an atom under it (if not null) separated by a kern and in a smaller
 * size depending on "underScriptSize"
 */
class StackAtom : public Atom {
private:
  sptr<Atom> _base;
  StackArgs _over;
  StackArgs _under;

public:
  StackAtom() = delete;

  StackAtom(const sptr<Atom>& base, const StackArgs& args, bool isOver) {
    _base = base;
    if (isOver) {
      _over = args;
    } else {
      _under = args;
    }
  }

  StackAtom(
    const sptr<Atom>& base,
    StackArgs overArgs,
    StackArgs underArgs
  ) : _base(base), _over(std::move(overArgs)), _under(std::move(underArgs)) {}

  AtomType leftType() const override {
    return _base->leftType();
  }

  AtomType rightType() const override {
    return _base->rightType();
  }

  sptr<Box> createBox(Env& env) override;

  StackResult createStack(Env& env);

  __decl_clone(StackAtom)
};

}

#endif //LATEX_ATOM_STACK_H
