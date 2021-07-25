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

}

#endif //LATEX_MACRO_SIZES_H
