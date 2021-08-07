#ifndef LATEX_MACRO_SIZES_H
#define LATEX_MACRO_SIZES_H

#include "macro/macro_decl.h"

namespace tex {

inline macro(declaremathsizes) {
  float a, b, c, d;
  valueof(args[1], a), valueof(args[2], b), valueof(args[3], c), valueof(args[4], d);
  // DefaultTeXFont::setMathSizes(a, b, c, c);
  return nullptr;
}

inline macro(magnification) {
  float x;
  valueof(args[1], x);
  // DefaultTeXFont::setMagnification(x);
  return nullptr;
}

inline macro(sizes) {
  float f = 1;
  if (args[0] == L"tiny")
    f = 0.5f;
  else if (args[0] == L"scriptsize")
    f = 0.7f;
  else if (args[0] == L"footnotesize")
    f = 0.8f;
  else if (args[0] == L"small")
    f = 0.9f;
  else if (args[0] == L"normalsize")
    f = 1.f;
  else if (args[0] == L"large")
    f = 1.2f;
  else if (args[0] == L"Large")
    f = 1.4f;
  else if (args[0] == L"LARGE")
    f = 1.8f;
  else if (args[0] == L"huge")
    f = 2.f;
  else if (args[0] == L"Huge")
    f = 2.5f;

  auto a = Formula(tp, tp.getOverArgument(), false, tp.isMathMode())._root;
  return sptrOf<MonoScaleAtom>(a, f);
}

inline sptr <Atom> _big(
  TeXParser& tp,
  std::vector<std::wstring>& args,
  int size,
  AtomType type = AtomType::none
) {
  auto a = Formula(tp, args[1], false)._root;
  auto s = std::dynamic_pointer_cast<SymbolAtom>(a);
  if (s == nullptr) return a;
  auto t = sptrOf<BigSymbolAtom>(s, size);
  if (type != AtomType::none) t->_type = type;
  return t;
}

inline macro(big) { return _big(tp, args, 1); }

inline macro(Big) { return _big(tp, args, 2); }

inline macro(bigg) { return _big(tp, args, 3); }

inline macro(Bigg) { return _big(tp, args, 4); }

inline macro(bigl) { return _big(tp, args, 1, AtomType::opening); }

inline macro(Bigl) { return _big(tp, args, 2, AtomType::opening); }

inline macro(biggl) { return _big(tp, args, 3, AtomType::opening); }

inline macro(Biggl) { return _big(tp, args, 4, AtomType::opening); }

inline macro(bigr) { return _big(tp, args, 1, AtomType::closing); }

inline macro(Bigr) { return _big(tp, args, 2, AtomType::closing); }

inline macro(biggr) { return _big(tp, args, 3, AtomType::closing); }

inline macro(Biggr) { return _big(tp, args, 4, AtomType::closing); }


}

#endif //LATEX_MACRO_SIZES_H
