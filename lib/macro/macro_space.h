#ifndef MICROTEX_MACRO_SPACE_H
#define MICROTEX_MACRO_SPACE_H

#include "atom/atom_space.h"
#include "macro/macro_decl.h"

namespace microtex {

inline macro(quad) {
  return sptrOf<SpaceAtom>(UnitType::em, 1.f, 0.f, 0.f);
}

macro(muskips);

}  // namespace microtex

#endif  // MICROTEX_MACRO_SPACE_H
