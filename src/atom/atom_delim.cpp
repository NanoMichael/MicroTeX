#include "atom/atom_delim.h"
#include "box/box_factory.h"
#include "box/box_group.h"
#include "env/env.h"

using namespace tex;

sptr<Box> OverUnderDelimiter::createBox(Env& env) {
  const auto base = _base->createBox(env);
  const auto delim = tex::createHDelim(_delim, env, base->_width);
  base->_shift = (delim->_width - base->_width) / 2;

  const auto vbox = sptrOf<VBox>();
  if (_over) {
    vbox->add(delim);
    const auto kern = SpaceAtom(UnitType::ex, 0.f, -1.f, 0.f).createBox(env);
    vbox->add(kern);
    vbox->add(base);
    const auto h = vbox->_height + vbox->_depth;
    vbox->_depth = base->_depth;
    vbox->_height = h - vbox->_depth;
  } else {
    vbox->add(base);
    vbox->add(delim);
    const auto h = vbox->_height + vbox->_depth;
    vbox->_height = base->_height;
    vbox->_depth = h - vbox->_height;
  }

  return vbox;
}
