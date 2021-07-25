#ifndef LATEX_MACRO_COLORS_H
#define LATEX_MACRO_COLORS_H

#include "macro/macro_decl.h"
#include "atom/atom_impl.h"
#include "utils/utf.h"

namespace tex {

inline macro(fgcolor) {
  auto a = Formula(tp, args[2])._root;
  std::string x = wide2utf8(args[1]);
  return sptrOf<ColorAtom>(a, TRANSPARENT, ColorAtom::getColor(x));
}

inline macro(bgcolor) {
  auto a = Formula(tp, args[2])._root;
  std::string x = wide2utf8(args[1]);
  return sptrOf<ColorAtom>(a, ColorAtom::getColor(x), TRANSPARENT);
}

inline macro(textcolor) {
  auto a = Formula(tp, args[2], false, false)._root;
  std::string x = wide2utf8(args[1]);
  return sptrOf<ColorAtom>(a, TRANSPARENT, ColorAtom::getColor(x));
}

inline macro(colorbox) {
  std::string x = wide2utf8(args[1]);
  color c = ColorAtom::getColor(x);
  return sptrOf<FBoxAtom>(Formula(tp, args[2])._root, c, c);
}

inline macro(fcolorbox) {
  std::string x = wide2utf8(args[2]);
  color f = ColorAtom::getColor(x);
  std::string y = wide2utf8(args[1]);
  color b = ColorAtom::getColor(y);
  return sptrOf<FBoxAtom>(Formula(tp, args[3])._root, f, b);
}

macro(definecolor);

}

#endif //LATEX_MACRO_COLORS_H
