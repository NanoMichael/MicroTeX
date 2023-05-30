#ifndef MICROTEX_MACRO_SIZES_H
#define MICROTEX_MACRO_SIZES_H

#include "macro/macro_decl.h"

namespace microtex {

inline macro(declaremathsizes) {
  float a, b, c, d;
  valueOf(args[1], a), valueOf(args[2], b), valueOf(args[3], c), valueOf(args[4], d);
  // TODO setMathSizes(a, b, c, d);
  return nullptr;
}

inline macro(magnification) {
  float x;
  valueOf(args[1], x);
  // TODO setMagnification(x);
  return nullptr;
}

inline macro(sizes) {
  float f = 1;
  if (args[0] == "tiny")
    f = 0.5f;
  else if (args[0] == "scriptsize")
    f = 0.7f;
  else if (args[0] == "footnotesize")
    f = 0.8f;
  else if (args[0] == "small")
    f = 0.9f;
  else if (args[0] == "normalsize")
    f = 1.f;
  else if (args[0] == "large")
    f = 1.2f;
  else if (args[0] == "Large")
    f = 1.4f;
  else if (args[0] == "LARGE")
    f = 1.8f;
  else if (args[0] == "huge")
    f = 2.f;
  else if (args[0] == "Huge")
    f = 2.5f;

  auto a = Formula(tp, tp.getOverArgument(), false, tp.isMathMode())._root;
  a = a == nullptr ? sptrOf<EmptyAtom>() : a;
  return sptrOf<ScaleAtom>(a, f);
}

inline sptr<Atom>
_big(Parser& tp, std::vector<std::string>& args, int size, AtomType type = AtomType::none) {
  auto a = Formula(tp, args[1], false)._root;
  auto s = std::dynamic_pointer_cast<SymbolAtom>(a);
  if (s == nullptr) return a;
  auto t = sptrOf<BigSymbolAtom>(s, size);
  if (type != AtomType::none) t->_type = type;
  return t;
}

inline macro(big) {
  return _big(tp, args, 1);
}

inline macro(Big) {
  return _big(tp, args, 2);
}

inline macro(bigg) {
  return _big(tp, args, 3);
}

inline macro(Bigg) {
  return _big(tp, args, 4);
}

inline macro(bigl) {
  return _big(tp, args, 1, AtomType::opening);
}

inline macro(Bigl) {
  return _big(tp, args, 2, AtomType::opening);
}

inline macro(biggl) {
  return _big(tp, args, 3, AtomType::opening);
}

inline macro(Biggl) {
  return _big(tp, args, 4, AtomType::opening);
}

inline macro(bigr) {
  return _big(tp, args, 1, AtomType::closing);
}

inline macro(Bigr) {
  return _big(tp, args, 2, AtomType::closing);
}

inline macro(biggr) {
  return _big(tp, args, 3, AtomType::closing);
}

inline macro(Biggr) {
  return _big(tp, args, 4, AtomType::closing);
}

}  // namespace microtex

#endif  // MICROTEX_MACRO_SIZES_H
