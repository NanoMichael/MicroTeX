#ifndef LATEX_MACRO_SCRIPTS_H
#define LATEX_MACRO_SCRIPTS_H

#include "macro/macro_decl.h"
#include "atom/atom_impl.h"
#include "atom/atom_sideset.h"

namespace tex {

inline macro(frac) {
  Formula num(tp, args[1], false);
  Formula den(tp, args[2], false);
  if (num._root == nullptr || den._root == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FractionAtom>(num._root, den._root, true);
}

inline macro(over) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FractionAtom>(num, den, true);
}

inline macro(atop) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of a fraction can't be empty!");
  return sptrOf<FractionAtom>(num, den, false);
}

inline macro(mathcumsup) {
  return sptrOf<CumulativeScriptsAtom>(
    tp.popLastAtom(),
    nullptr,
    Formula(tp, args[1])._root
  );
}

inline macro(mathcumsub) {
  return sptrOf<CumulativeScriptsAtom>(
    tp.popLastAtom(),
    Formula(tp, args[1])._root,
    nullptr
  );
}

macro(prescript);

macro(sideset);

macro(sfrac);

macro(cfrac);

macro(genfrac);

macro(overwithdelims);

macro(atopwithdelims);

macro(abovewithdelims);

}

#endif //LATEX_MACRO_SCRIPTS_H
