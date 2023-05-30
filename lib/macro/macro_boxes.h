#ifndef MICROTEX_MACRO_BOXES_H
#define MICROTEX_MACRO_BOXES_H

#include "atom/atom_box.h"
#include "atom/atom_misc.h"
#include "macro/macro_decl.h"
#include "utils/string_utils.h"

namespace microtex {

inline macro(rotatebox) {
  float angle = 0;
  if (!args[1].empty()) valueOf(args[1], angle);
  return sptrOf<RotateAtom>(Formula(tp, args[2])._root, angle, args[3]);
}

inline macro(reflectbox) {
  return sptrOf<ReflectAtom>(Formula(tp, args[1])._root);
}

inline macro(scalebox) {
  float sx = 1, sy = 1;
  valueOf(args[1], sx);

  if (args[3].empty())
    sy = sx;
  else
    valueOf(args[3], sy);

  if (sx == 0) sx = 1;
  if (sy == 0) sy = 1;
  return sptrOf<ScaleAtom>(Formula(tp, args[2], false, tp.isMathMode())._root, sx, sy);
}

inline macro(resizebox) {
  const std::string& ws = args[1];
  const std::string& hs = args[2];
  return sptrOf<ResizeAtom>(Formula(tp, args[3])._root, ws, hs, ws == "!" || hs == "!");
}

inline macro(shadowbox) {
  return sptrOf<ShadowAtom>(Formula(tp, args[1])._root);
}

inline macro(ovalbox) {
  return sptrOf<OvalAtom>(Formula(tp, args[1])._root);
}

inline macro(cornersize) {
  float size = 0.5f;
  valueOf(args[1], size);
  if (size <= 0 || size > 0.5f) size = 0.5f;
  OvalAtom::_multiplier = size;
  OvalAtom::_diameter = 0;
  return nullptr;
}

inline macro(doublebox) {
  return sptrOf<DoubleFramedAtom>(Formula(tp, args[1])._root);
}

inline macro(fbox) {
  return sptrOf<FBoxAtom>(Formula(tp, args[1], false)._root);
}

}  // namespace microtex

#endif  // MICROTEX_MACRO_BOXES_H
