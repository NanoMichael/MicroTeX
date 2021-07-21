#include "atom/atom_sideset.h"
#include "box/box_single.h"

using namespace tex;

sptr<Box> SideSetsAtom::createBox(Env& env) {
  _base->_limitsType = LimitsType::noLimits;
  return StrutBox::empty();
}
