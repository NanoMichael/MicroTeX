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

inline macro(sideset) {
  auto l = Formula(tp, args[1])._root;
  auto r = Formula(tp, args[2])._root;
  auto op = Formula(tp, args[3])._root;
  if (op == nullptr) {
    auto in = sptrOf<CharAtom>(L'M', "mathnormal");
    op = sptrOf<PhantomAtom>(in, false, true, true);
  }
  auto cl = dynamic_cast<CumulativeScriptsAtom*>(l.get());
  auto cr = dynamic_cast<CumulativeScriptsAtom*>(r.get());
  if (cl != nullptr) l = cl->getScriptsAtom();
  if (cr != nullptr) r = cr->getScriptsAtom();
  return sptrOf<SideSetsAtom>(op, l, r);
}

inline macro(prescript) {
  auto base = Formula(tp, args[3])._root;
  auto p = sptrOf<PhantomAtom>(base, false, true, true);
  auto s = sptrOf<ScriptsAtom>(p, Formula(tp, args[2])._root, Formula(tp, args[1])._root, false);
  tp.addAtom(s);
  tp.addAtom(sptrOf<SpaceAtom>(UnitType::mu, -0.3f, 0.f, 0.f));
  return sptrOf<TypedAtom>(AtomType::ordinary, AtomType::ordinary, base);
}

macro(sfrac);

macro(cfrac);

macro(genfrac);

macro(overwithdelims);

macro(atopwithdelims);

macro(abovewithdelims);

}

#endif //LATEX_MACRO_SCRIPTS_H
