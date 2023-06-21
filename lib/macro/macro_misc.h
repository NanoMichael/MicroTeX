#ifndef MICROTEX_MACRO_MISC_H
#define MICROTEX_MACRO_MISC_H

#include <memory>

#include "atom/atom_basic.h"
#include "atom/atom_misc.h"
#include "atom/atom_sideset.h"
#include "core/formula.h"
#include "core/parser.h"
#include "core/split.h"
#include "graphic/graphic.h"
#include "macro/macro.h"
#include "macro/macro_decl.h"
#include "utils/exceptions.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

namespace microtex {

inline macro(fatalIfCmdConflict) {
  NewCommandMacro::_errIfConflict = args[1] == "true";
  return nullptr;
}

inline macro(breakEverywhere) {
  RowAtom::_breakEverywhere = args[1] == "true";
  return nullptr;
}

inline macro(st) {
  auto base = Formula(tp, args[1], false, tp.isMathMode())._root;
  return sptrOf<StrikeThroughAtom>(base);
}

inline macro(spATbreve) {
  auto* vra = new VRowAtom(Formula("\\displaystyle\\!\\breve{}")._root);
  vra->setRaise(UnitType::ex, 0.6f);
  return sptrOf<SmashedAtom>(sptr<Atom>(vra), "");
}

inline macro(clrlap) {
  return sptrOf<LapedAtom>(Formula(tp, args[1])._root, args[0][0]);
}

inline macro(mathclrlap) {
  return sptrOf<LapedAtom>(Formula(tp, args[1])._root, args[0][4]);
}

inline sptr<Atom> _cancel(int cancelType, Parser& tp, std::vector<std::string>& args) {
  auto base = Formula(tp, args[1], false)._root;
  if (base == nullptr) throw ex_parse("Cancel content must not be empty!");
  return sptrOf<CancelAtom>(base, cancelType);
}

inline macro(cancel) {
  return _cancel(CancelAtom::SLASH, tp, args);
}

inline macro(bcancel) {
  return _cancel(CancelAtom::BACKSLASH, tp, args);
}

inline macro(xcancel) {
  return _cancel(CancelAtom::CROSS, tp, args);
}

inline macro(underscore) {
  return SymbolAtom::get("_");
}

inline macro(nbsp) {
  return sptrOf<SpaceAtom>();
}

inline macro(joinrel) {
  return sptrOf<TypedAtom>(
    AtomType::relation,
    AtomType::relation,
    sptrOf<SpaceAtom>(UnitType::mu, -2.6f, 0.f, 0.f)
  );
}

inline macro(smash) {
  return sptrOf<SmashedAtom>(Formula(tp, args[1], false)._root, args[2]);
}

inline macro(makeatletter) {
  tp.makeAtLetter();
  return nullptr;
}

inline macro(makeatother) {
  tp.makeAtOther();
  return nullptr;
}

inline macro(newenvironment) {
  int opt = 0;
  if (!args[4].empty()) valueOf(args[4], opt);

  NewEnvironmentMacro::addNewEnvironment(args[1], args[2], args[3], opt);
  return nullptr;
}

inline macro(renewenvironment) {
  int opt = 0;
  if (!args[4].empty()) valueOf(args[4], opt);

  NewEnvironmentMacro::addRenewEnvironment(args[1], args[2], args[3], opt);
  return nullptr;
}

inline macro(hphantom) {
  return sptrOf<PhantomAtom>(Formula(tp, args[1], false)._root, true, false, false);
}

inline macro(vphantom) {
  return sptrOf<PhantomAtom>(Formula(tp, args[1], false)._root, false, true, true);
}

inline macro(phantom) {
  return sptr<Atom>(new PhantomAtom(Formula(tp, args[1], false)._root, true, true, true));
}

inline macro(surd) {
  return sptrOf<VCenterAtom>(SymbolAtom::get("surdsign"));
}

inline macro(lmoustache) {
  auto* s = new SymbolAtom(*(SymbolAtom::get("lmoustache")));
  auto b = sptrOf<BigSymbolAtom>(sptr<SymbolAtom>(s), 1);
  b->_type = AtomType::opening;
  return b;
}

inline macro(rmoustache) {
  auto* s = new SymbolAtom(*(SymbolAtom::get("rmoustache")));
  auto b = sptrOf<BigSymbolAtom>(sptr<SymbolAtom>(s), 1);
  b->_type = AtomType::closing;
  return b;
}

inline macro(breakmark) {
  return sptrOf<BreakMarkAtom>();
}

inline macro(nokern) {
  return sptrOf<NokernAtom>();
}

/**************************************** limits macros *******************************************/

inline sptr<Atom> _limits_type(Parser& tp, Args& args, LimitsType type) {
  auto atom = tp.popBack();
  if (atom != nullptr)
    atom->_limitsType = type;
  return atom;
}

inline macro(nolimits) {
  return _limits_type(tp, args, LimitsType::noLimits);
}

inline macro(limits) {
  return _limits_type(tp, args, LimitsType::limits);
}

inline macro(normal) {
  return _limits_type(tp, args, LimitsType::normal);
}

/***************************************** implement at .cpp **************************************/

macro(longdiv);

macro(char);

macro(cr);

macro(kern);

macro(hvspace);

macro(rule);

macro(newcommand);

macro(renewcommand);

macro(raisebox);

macro(romannumeral);

macro(zstack);

#ifdef GRAPHICS_DEBUG

macro(debug);

macro(undebug);

#endif  // GRAPHICS_DEBUG

inline macro(backslashcr) {
  return macro_cr(tp, args);
}

/**************************************** not implemented *****************************************/

inline macro(includegraphics) {
  return nullptr;
}

}  // namespace microtex

#endif  // MICROTEX_MACRO_MISC_H
