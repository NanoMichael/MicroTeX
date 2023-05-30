#ifndef MICROTEX_MACRO_COLORS_H
#define MICROTEX_MACRO_COLORS_H

#include "atom/atom_basic.h"
#include "atom/atom_box.h"
#include "atom/atom_misc.h"
#include "macro/macro_decl.h"
#include "utils/utf.h"

namespace microtex {

inline macro(fgcolor) {
  auto a = Formula(tp, args[2])._root;
  return sptrOf<ColorAtom>(a, TRANSPARENT, ColorAtom::getColor(args[1]));
}

inline macro(bgcolor) {
  auto a = Formula(tp, args[2])._root;
  return sptrOf<ColorAtom>(a, ColorAtom::getColor(args[1]), TRANSPARENT);
}

inline macro(textcolor) {
  auto a = Formula(tp, args[2], false, false)._root;
  return sptrOf<ColorAtom>(a, TRANSPARENT, ColorAtom::getColor(args[1]));
}

inline macro(colorbox) {
  color c = ColorAtom::getColor(args[1]);
  return sptrOf<FBoxAtom>(Formula(tp, args[2])._root, c, c);
}

inline macro(fcolorbox) {
  color f = ColorAtom::getColor(args[2]);
  color b = ColorAtom::getColor(args[1]);
  return sptrOf<FBoxAtom>(Formula(tp, args[3])._root, f, b);
}

macro(definecolor);

}  // namespace microtex

#endif  // MICROTEX_MACRO_COLORS_H
