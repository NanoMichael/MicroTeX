#include "atom/atom_operator.h"

#include "atom/atom_basic.h"
#include "atom/atom_row.h"
#include "atom/atom_scripts.h"
#include "atom/atom_sideset.h"
#include "atom/atom_stack.h"
#include "env/env.h"

using namespace microtex;

sptr<Box> OperatorAtom::createBox(Env& env) {
  // case 1: side sets
  if (auto ss = dynamic_cast<SideSetsAtom*>(_base.get()); ss != nullptr) {
    ss->_under = _under;
    ss->_over = _over;
    return ss->createBox(env);
  }

  // case 2: limits are shown as scripts
  // clang-format off
  if ((_limitsType == LimitsType::noLimits)
      || (_limitsType == LimitsType::normal && env.style() >= TexStyle::text)
    ) {
    // clang-format on
    RowAtom* row = nullptr;
    auto base = _base;
    // special case: the base atom is a row that composed by other atoms
    // we should take out the last atom in the row to place scripts
    if (auto typed = dynamic_cast<TypedAtom*>(_base.get()); typed != nullptr) {
      const auto& atom = typed->base();
      const auto ra = dynamic_cast<RowAtom*>(atom.get());
      if (ra != nullptr && ra->_lookAtLastAtom) {
        base = ra->popBack();
        row = ra;
      } else {
        base = atom;
      }
    }
    if (row != nullptr) {
      row->add(sptrOf<ScriptsAtom>(base, _under, _over));
      return row->createBox(env);
    }
    return ScriptsAtom(base, _under, _over).createBox(env);
  }

  // case 3: limits are put over/under the base
  const StackArgs& over = StackArgs::autoSpace(_over);
  const StackArgs& under = StackArgs::autoSpace(_under);
  return StackAtom(_base, over, under).createBox(env);
}
