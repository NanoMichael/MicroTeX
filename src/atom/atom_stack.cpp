#include "atom/atom_stack.h"
#include "atom/atom_space.h"
#include "box/box_single.h"
#include "box/box_group.h"

using namespace tex;
using namespace std;

sptr<Box> StackAtom::changeWidth(const sptr<Box>& b, float maxWidth) {
  if (b != nullptr && abs(maxWidth - b->_width) > PREC) {
    return sptrOf<HBox>(b, maxWidth, Alignment::center);
  }
  return b;
}

sptr<Box> StackAtom::createBox(Env& env) {
  // create boxes in right style and calculate max width
  auto b = _base == nullptr ? StrutBox::empty() : _base->createBox(env);
  // over and under
  sptr<Box> o, u;
  float maxWidth = b->_width;
  if (_over.isPresent()) {
    o = (
      _over.isSmall
      ? env.withStyle(env.subStyle(), [&](Env& sub) { return _over.atom->createBox(sub); })
      : _over.atom->createBox(env)
    );
    maxWidth = std::max(maxWidth, o->_width);
  }
  if (_under.isPresent()) {
    u = (
      _under.isSmall
      ? env.withStyle(env.subStyle(), [&](Env& sub) { return _under.atom->createBox(sub); })
      : _under.atom->createBox(env)
    );
    maxWidth = std::max(maxWidth, u->_width);
  }

  // vertical box
  auto vbox = new VBox();

  // last font used by base (for mono-space atoms following)
  env.setLastFontId(b->lastFontId());

  // params to layout limits
  const auto& math = env.mathConsts();

  // over script + space
  if (o != nullptr && !o->isSpace()) {
    vbox->add(changeWidth(o, maxWidth));
    if (_over.isAutoSpace) {
      const auto gapMin = math.upperLimitGapMin() * env.scale();
      const auto baselineRiseMin = math.upperLimitBaselineRiseMin() * env.scale();
      const auto diff = baselineRiseMin - o->_depth;
      const auto space = std::max(diff, gapMin);
      vbox->add(sptrOf<StrutBox>(0.f, space, 0.f, 0.f));
    } else {
      vbox->add(SpaceAtom(_over.spaceUnit, 0.f, _over.space, 0.f).createBox(env));
    }
  }

  // base
  auto center = changeWidth(b, maxWidth);
  vbox->add(center);

  // calculate future height of the vertical box, to make sure that the
  // base stays on the baseline
  const auto h = vbox->_height + vbox->_depth - center->_depth;

  // under script + space
  if (u != nullptr && !u->isSpace()) {
    if (_under.isAutoSpace) {
      const auto gapMin = math.lowerLimitGapMin() * env.scale();
      const auto baselineDropMin = math.lowerLimitBaselineDropMin() * env.scale();
      const auto diff = baselineDropMin - u->_height;
      const auto space = std::max(diff, gapMin);
      vbox->add(sptrOf<StrutBox>(0.f, space, 0.f, 0.f));
    } else {
      vbox->add(SpaceAtom(_under.spaceUnit, 0.f, _under.space, 0.f).createBox(env));
    }
    vbox->add(changeWidth(u, maxWidth));
  }

  // calculate height and depth
  vbox->_depth = vbox->_height + vbox->_depth - h;
  vbox->_height = h;
  return sptr<Box>(vbox);
}
