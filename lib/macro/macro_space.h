#ifndef MICROTEX_MACRO_SPACE_H
#define MICROTEX_MACRO_SPACE_H

#include "macro/macro_decl.h"
#include "atom/atom_space.h"

namespace microtex {

inline macro(quad) {
  return sptrOf<SpaceAtom>(UnitType::em, 1.f, 0.f, 0.f);
}

macro(muskips);

}

#endif //MICROTEX_MACRO_SPACE_H
