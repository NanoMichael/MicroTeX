#include "atom/atom_delim.h"

#include "atom/atom_basic.h"

#include "box/box_factory.h"
#include "box/box_group.h"
#include "box/box_single.h"
#include "env/env.h"

using namespace microtex;

sptr<Box> OverUnderBar::createBox(Env& env) {
  const auto base = (_base == nullptr ? sptrOf<EmptyAtom>() : _base)->createBox(env);
  const auto vbox = sptrOf<VBox>();
  if (_over) {
    const auto asc = env.mathConsts().overbarExtraAscender() * env.scale();
    vbox->add(sptrOf<StrutBox>(0.f, asc, 0.f, 0.f));
    const auto t = env.mathConsts().overbarRuleThickness() * env.scale();
    vbox->add(sptrOf<RuleBox>(t, base->_width, 0.f));
    const auto gap = env.mathConsts().overbarVerticalGap() * env.scale();
    vbox->add(sptrOf<StrutBox>(0.f, gap, 0.f, 0.f));
    vbox->add(base);
    const auto h = vbox->_height + vbox->_depth;
    vbox->_depth = base->_depth;
    vbox->_height = h - vbox->_depth;
  } else {
    vbox->add(base);
    const auto gap = env.mathConsts().underbarVerticalGap() * env.scale();
    vbox->add(sptrOf<StrutBox>(0.f, gap, 0.f, 0.f));
    const auto t = env.mathConsts().underbarRuleThickness() * env.scale();
    vbox->add(sptrOf<RuleBox>(t, base->_width, 0.f));
    const auto dsc = env.mathConsts().underbarExtraDescender() * env.scale();
    vbox->add(sptrOf<StrutBox>(0.f, dsc, 0.f, 0.f));
    const auto h = vbox->_height + vbox->_depth;
    vbox->_height = base->_height;
    vbox->_depth = h - vbox->_height;
  }

  return vbox;
}

sptr<Box> OverUnderDelimiter::createBox(Env& env) {
  const auto base = _base->createBox(env);
  const auto delim = microtex::createHDelim(_delim, env, base->_width);
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
