#include "macro/macro_delims.h"

#include "atom/atom_stack.h"
#include "utils/utf.h"

namespace microtex {

using namespace std;

macro(xarrow) {
  const auto& name = args[0].substr(1);
  const auto& over =
    StackArgs::autoSpace(Formula(tp, args[1], false, tp.isMathMode())._root, false);
  const auto& under =
    StackArgs::autoSpace(Formula(tp, args[2], false, tp.isMathMode())._root, false);
  const auto stack = new StackAtom(nullptr, over, under);
  const auto& arrow = sptrOf<ExtensibleAtom>(
    name,
    // capture raw pointer to avoid cycle reference
    [stack](const Env& env) -> float {
      return stack->getMaxWidth() + Units::fsize(UnitType::ex, 1.f, env);
    },
    false
  );
  arrow->_type = AtomType::relation;
  stack->setBaseAtom(arrow);
  return sptr<StackAtom>(stack);
}

macro(left) {
  const string& grep = tp.getGroup("\\left", "\\right");

  auto left = Formula(tp, args[1], false)._root;
  auto* big = dynamic_cast<BigSymbolAtom*>(left.get());
  if (big != nullptr) left = big->_delim;

  auto right = tp.getArgument();
  big = dynamic_cast<BigSymbolAtom*>(right.get());
  if (big != nullptr) right = big->_delim;

  auto sl = dynamic_pointer_cast<CharSymbol>(left);
  auto sr = dynamic_pointer_cast<CharSymbol>(right);
  if (sl != nullptr && sr != nullptr) {
    Formula tf(tp, grep, false);
    return sptrOf<FencedAtom>(tf._root, sl->name(), sr->name(), tf.middle());
  }

  auto* ra = new RowAtom();
  ra->add(left);
  ra->add(Formula(tp, grep, false)._root);
  ra->add(right);

  return sptr<Atom>(ra);
}

}  // namespace microtex
