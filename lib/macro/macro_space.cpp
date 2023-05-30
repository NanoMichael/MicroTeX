#include "macro/macro_space.h"

namespace microtex {

macro(muskips) {
  SpaceType type = SpaceType::none;
  if (args[0] == ",")
    type = SpaceType::thinMuSkip;
  else if (args[0] == ":")
    type = SpaceType::medMuSkip;
  else if (args[0] == ";")
    type = SpaceType::thickMuSkip;
  else if (args[0] == "thinspace")
    type = SpaceType::thinMuSkip;
  else if (args[0] == "medspace")
    type = SpaceType::medMuSkip;
  else if (args[0] == "thickspace")
    type = SpaceType::thickMuSkip;
  else if (args[0] == "!")
    type = SpaceType::negThinMuSkip;
  else if (args[0] == "negthinspace")
    type = SpaceType::negThinMuSkip;
  else if (args[0] == "negmedspace")
    type = SpaceType::negMedMuSkip;
  else if (args[0] == "negthickspace")
    type = SpaceType::negThickMuSkip;

  return sptrOf<SpaceAtom>(type);
}

}  // namespace microtex
