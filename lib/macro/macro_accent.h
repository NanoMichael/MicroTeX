#ifndef MICROTEX_MACRO_ACCENT_H
#define MICROTEX_MACRO_ACCENT_H

#include "atom/atom_accent.h"
#include "atom/atom_basic.h"
#include "atom/atom_stack.h"
#include "macro/macro_decl.h"
#include "utils/utf.h"

namespace microtex {

inline macro(accentset) {
  if (args[1] == "")
    return Formula(tp, args[2], false)._root;
  return sptrOf<AccentedAtom>(Formula(tp, args[2], false)._root, args[1].substr(1), false, true);
}

inline macro(stack) {
  const auto& over = StackArgs::autoSpace(Formula(tp, args[1], false)._root);
  const auto& under = StackArgs::autoSpace(Formula(tp, args[3], false)._root);
  return sptrOf<StackAtom>(Formula(tp, args[2], false)._root, over, under);
}

inline macro(stackrel) {
  const auto& stack = macro_stack(tp, args);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, stack);
}

inline macro(stackbin) {
  const auto& stack = macro_stack(tp, args);
  return sptrOf<TypedAtom>(AtomType::binaryOperator, AtomType::binaryOperator, stack);
}

inline macro(overset) {
  const auto& over = StackArgs::autoSpace(Formula(tp, args[1], false)._root);
  sptr<Atom> a = sptrOf<StackAtom>(Formula(tp, args[2], false)._root, over, true);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, a);
}

inline macro(underset) {
  const auto& under = StackArgs::autoSpace(Formula(tp, args[1], false)._root);
  auto a = sptrOf<StackAtom>(Formula(tp, args[2], false)._root, under, false);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, a);
}

inline macro(underaccent) {
  const StackArgs under{Formula(tp, args[1], false)._root, UnitType::mu, 1.f, true};
  auto a = sptrOf<StackAtom>(Formula(tp, args[2], false)._root, under, false);
  a->setAdjustBottom(true);
  return a;
}

macro(accentbiss);

macro(accents);

macro(undertilde);

}  // namespace microtex

#endif  // MICROTEX_MACRO_ACCENT_H
