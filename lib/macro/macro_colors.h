#ifndef TINYTEX_MACRO_COLORS_H
#define TINYTEX_MACRO_COLORS_H

#include "macro/macro_decl.h"
#include "atom/atom_misc.h"
#include "atom/atom_box.h"
#include "atom/atom_basic.h"
#include "utils/utf.h"

namespace tinytex {

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

}

#endif //TINYTEX_MACRO_COLORS_H
