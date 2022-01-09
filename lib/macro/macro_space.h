#ifndef LATEX_MACRO_SPACE_H
#define LATEX_MACRO_SPACE_H

#include "macro/macro_decl.h"
#include "atom/atom_space.h"

namespace tex {

inline macro(quad) {
  return sptrOf<SpaceAtom>(UnitType::em, 1.f, 0.f, 0.f);
}

macro(muskips);

}

#endif //LATEX_MACRO_SPACE_H
