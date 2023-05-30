#include "macro/macro_accent.h"

namespace microtex {

macro(accentbiss) {
  std::string acc;
  switch (args[0][0]) {
    case '~': acc = "tilde"; break;
    case '\'': acc = "acute"; break;
    case '^': acc = "hat"; break;
    case '\"': acc = "ddot"; break;
    case '`': acc = "grave"; break;
    case '=': acc = "bar"; break;
    case '.': acc = "dot"; break;
    case 'u': acc = "breve"; break;
    case 'v': acc = "check"; break;
    case 't': acc = "tie"; break;
    case 'r': acc = "mathring"; break;
  }

  return sptrOf<AccentedAtom>(Formula(tp, args[1], false)._root, acc);
}

macro(accents) {
  const auto& name = args[0];
  const auto& [acc, fit] = [&]() {
    if (name == "widehat") return std::make_pair<std::string>("hat", true);
    if (name == "widetilde") return std::make_pair<std::string>("tilde", true);
    return std::make_pair(name, false);
  }();
  return sptrOf<AccentedAtom>(Formula(tp, args[1], false)._root, acc, fit);
}

macro(undertilde) {
  auto stack = new StackAtom({StackElement::base, StackElement::under});
  auto tilde = sptrOf<ExtensibleAtom>(
    "tilde",
    // capture raw pointer to avoid cycle reference
    [stack](const Env& env) { return stack->getMaxWidth(); },
    false
  );
  auto a = Formula(tp, args[1], false)._root;
  const StackArgs under{tilde, UnitType::mu, 0.5f, true};
  stack->setAdjustBottom(true);
  stack->setBaseAtom(a);
  stack->setUnder(under);
  return sptr<StackAtom>(stack);
}

}  // namespace microtex
