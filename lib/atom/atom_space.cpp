#include "atom/atom_space.h"

#include "box/box_single.h"
#include "core/glue.h"
#include "env/env.h"
#include "env/units.h"

using namespace std;
using namespace microtex;

sptr<Box> SpaceAtom::createBox(Env& env) {
  if (!_blankSpace) {
    float w = Units::fsize(_unit, _width, env);
    float h = Units::fsize(_unit, _height, env);
    float d = Units::fsize(_unit, _depth, env);
    return sptrOf<StrutBox>(w, h, d, 0.f);
  }
  if (_blankType == SpaceType::none) {
    return sptrOf<StrutBox>(env.space(_isMathMode), 0.f, 0.f, 0.f);
  }
  return Glue::get(_blankType, env);
}
