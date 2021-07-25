#ifndef LATEX_MACRO_ACCENT_H
#define LATEX_MACRO_ACCENT_H

#include "macro/macro_decl.h"
#include "utils/utf.h"

namespace tex {

inline macro(accents) {
  const auto name = wide2utf8(args[0]);
  const auto&[acc, fit] = [&]() {
    if (name == "widehat") return std::make_pair<std::string>("hat", true);
    if (name == "widetilde") return std::make_pair<std::string>("tilde", true);
    return std::make_pair(name, false);
  }();
  return sptrOf<AccentedAtom>(Formula(tp, args[1], false)._root, acc, fit);
}

inline macro(accentset) {
  return sptrOf<AccentedAtom>(
    Formula(tp, args[2], false)._root,
    wide2utf8(args[1]).substr(1), false, true
  );
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
  sptr<Atom> a = sptrOf<StackAtom>(Formula(tp, args[2], false)._root, under, false);
  return sptrOf<TypedAtom>(AtomType::relation, AtomType::relation, a);
}

inline macro(underaccent) {
  const StackArgs under{Formula(tp, args[1], false)._root, UnitType::mu, 1.f, true};
  return sptrOf<StackAtom>(Formula(tp, args[2], false)._root, under, false);
}

inline macro(undertilde) {
  // TODO
  auto a = Formula(tp, args[1], false)._root;
  auto p = sptrOf<PhantomAtom>(a, true, false, false);
  auto acc = sptrOf<AccentedAtom>(p, "widetilde");
  const StackArgs under{acc, UnitType::mu, 0.5f, true};
  return sptrOf<StackAtom>(a, under, false);
}

macro(accentbiss);

}

#endif //LATEX_MACRO_ACCENT_H
