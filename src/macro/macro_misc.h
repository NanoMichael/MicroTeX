#ifndef LATEX_MACRO_MISC_H
#define LATEX_MACRO_MISC_H

#include <memory>

#include "atom/atom_basic.h"
#include "atom/atom_misc.h"
#include "atom/atom_sideset.h"
#include "core/split.h"
#include "core/formula.h"
#include "core/parser.h"
#include "macro/macro.h"
#include "macro/macro_decl.h"
#include "graphic/graphic.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

namespace tex {

inline macro(fatalIfCmdConflict) {
  NewCommandMacro::_errIfConflict = args[1] == "true";
  return nullptr;
}

inline macro(breakEverywhere) {
  RowAtom::_breakEveywhere = args[1] == "true";
  return nullptr;
}

inline macro(longdiv) {
  long dividend = 0;
  valueof(args[1], dividend);
  long divisor = 0;
  valueof(args[2], divisor);
  if (divisor == 0) throw ex_parse("Divisor must not be 0.");
  return sptrOf<LongDivAtom>(divisor, dividend);
}

inline macro(st) {
  auto base = Formula(tp, args[1], false)._root;
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

inline sptr<Atom> _cancel(
  int cancelType,
  TeXParser& tp, std::vector<std::string>& args) {
  auto base = Formula(tp, args[1], false)._root;
  if (base == nullptr)
    throw ex_parse("Cancel content must not be empty!");
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

inline macro(cedilla) {
  return sptrOf<CedillaAtom>(Formula(tp, args[1])._root);
}

inline macro(ogonek) {
  return sptrOf<OgonekAtom>(Formula(tp, args[1])._root);
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

inline macro(cr) {
  if (tp.isArrayMode()) {
    tp.addRow();
  } else {
    ArrayFormula arr;
    arr.add(tp._formula->_root);
    arr.addRow();
    TeXParser parser(
      tp.isPartial(),
      tp.forwardBalancedGroup(),
      &arr,
      false,
      tp.isMathMode()
    );
    parser.parse();
    arr.checkDimensions();
    tp._formula->_root = arr.getAsVRow();
  }

  return nullptr;
}

inline macro(backslashcr) {
  return macro_cr(tp, args);
}

inline macro(shoveright) {
  auto a = Formula(tp, args[1])._root;
  a->_alignment = Alignment::right;
  return a;
}

inline macro(shoveleft) {
  auto a = Formula(tp, args[1])._root;
  a->_alignment = Alignment::left;
  return a;
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
  if (!args[4].empty()) valueof(args[4], opt);

  NewEnvironmentMacro::addNewEnvironment(args[1], args[2], args[3], opt);
  return nullptr;
}

inline macro(renewenvironment) {
  int opt = 0;
  if (!args[4].empty()) valueof(args[4], opt);

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
  return sptr<Atom>(
    new PhantomAtom(Formula(tp, args[1], false)._root, true, true, true));
}

inline macro(ctext) {
  // TODO text
  return sptrOf<EmptyAtom>();
}

inline macro(char) {
  // TODO
  std::string x = args[1];
  int radix = 10;
  if (startswith(x, "0x") || startswith(x, "0X")) {
    x = x.substr(2);
    radix = 16;
  } else if (startswith(x, "x") || startswith(x, "X")) {
    x = x.substr(1);
    radix = 16;
  } else if (startswith(x, "0")) {
    x = x.substr(1);
    radix = 8;
  }
  int n = 0;
  str2int(x, n, radix);
  return tp.getCharAtom(n);
}

inline macro(T) {
  return sptrOf<RotateAtom>(Formula(tp, args[1])._root, 180.f, "origin=cc");
}

inline macro(textcircled) {
  // TODO
  // return sptrOf<TextCircledAtom>(sptrOf<RomanAtom>(Formula(tp, args[1])._root));
  return nullptr;
}

inline macro(textsc) {
  return sptrOf<SmallCapAtom>(Formula(tp, args[1], false)._root);
}

inline macro(sc) {
  return sptrOf<SmallCapAtom>(Formula(tp, tp.getOverArgument(), false, tp.isMathMode())._root);
}

inline macro(surd) {
  return sptrOf<VCenteredAtom>(SymbolAtom::get("surdsign"));
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

inline sptr<Atom> _limits_type(TeXParser& tp, Args& args, LimitsType type) {
  auto atom = tp.popLastAtom();
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

macro(kern);

macro(hvspace);

macro(rule);

macro(newcommand);

macro(renewcommand);

macro(raisebox);

macro(romannumeral);

macro(setmathfont);

macro(zstack);

#ifdef GRAPHICS_DEBUG

macro(debug);

macro(undebug);

#endif  // GRAPHICS_DEBUG

/**************************************** not implemented *****************************************/

inline macro(includegraphics) {
  return nullptr;
}

inline macro(fcscore) {
  return nullptr;
}

inline macro(dynamic) {
  return nullptr;
}

}  // namespace tex

#endif  // LATEX_MACRO_MISC_H
