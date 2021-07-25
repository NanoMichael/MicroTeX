#include "macro/macro_space.h"

namespace tex {

macro(muskips) {
  SpaceType type = SpaceType::none;
  if (args[0] == L",")
    type = SpaceType::thinMuSkip;
  else if (args[0] == L":")
    type = SpaceType::medMuSkip;
  else if (args[0] == L";")
    type = SpaceType::thickMuSkip;
  else if (args[0] == L"thinspace")
    type = SpaceType::thinMuSkip;
  else if (args[0] == L"medspace")
    type = SpaceType::medMuSkip;
  else if (args[0] == L"thickspace")
    type = SpaceType::thickMuSkip;
  else if (args[0] == L"!")
    type = SpaceType::negThinMuSkip;
  else if (args[0] == L"negthinspace")
    type = SpaceType::negThinMuSkip;
  else if (args[0] == L"negmedspace")
    type = SpaceType::negMedMuSkip;
  else if (args[0] == L"negthickspace")
    type = SpaceType::negThickMuSkip;

  return sptrOf<SpaceAtom>(type);
}

}
