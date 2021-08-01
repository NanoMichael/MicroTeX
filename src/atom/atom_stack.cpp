#include "atom/atom_stack.h"
#include "atom/atom_space.h"
#include "atom/atom_char.h"
#include "atom/atom_basic.h"
#include "box/box_single.h"
#include "box/box_group.h"

using namespace tex;
using namespace std;

sptr<Box> StackAtom::createBox(Env& env) {
  const auto&[box, _] = createStack(env);
  return box;
}

StackResult StackAtom::createStack(Env& env) {
  // over and under
  sptr<Box> o, u;
  if (_over.isPresent()) {
    o = (
      _over.isScript
      ? env.withStyle(env.supStyle(), [&](Env& sub) { return _over.atom->createBox(sub); })
      : _over.atom->createBox(env)
    );
    _maxWidth = std::max(_maxWidth, o->_width);
  }
  if (_under.isPresent()) {
    u = (
      _under.isScript
      ? env.withStyle(env.subStyle(), [&](Env& sub) { return _under.atom->createBox(sub); })
      : _under.atom->createBox(env)
    );
    _maxWidth = std::max(_maxWidth, u->_width);
  }

  sptr<Box> b = _base == nullptr ? StrutBox::empty() : _base->createBox(env);
  _maxWidth = std::max(_maxWidth, b->_width);
  auto delta = 0.f;
  if (auto cs = dynamic_cast<CharSymbol*>(_base.get()); cs != nullptr) {
    delta = cs->getChar(env).italic();
  } else if (auto pa = dynamic_cast<PlaceholderAtom*>(_base.get()); pa != nullptr) {
    delta = pa->italic();
  }

  const auto wrap = [&](const sptr<Box>& box, float bias) -> sptr<Box> {
    box->_shift = (_maxWidth - box->_width) / 2 + bias;
    return box;
  };

  // vertical box
  auto vbox = sptrOf<VBox>();

  // last font used by base (for mono-space atoms following)
  env.setLastFontId(b->lastFontId());

  // params to layout limits
  const auto& math = env.mathConsts();

  // over script + space
  if (o != nullptr && !o->isSpace()) {
    auto ob = wrap(o, delta / 2);
    vbox->add(ob);
    float space = 0.f;
    if (_over.isAutoSpace) {
      const auto gapMin = math.upperLimitGapMin() * env.scale();
      const auto baselineRiseMin = math.upperLimitBaselineRiseMin() * env.scale();
      space = std::max(baselineRiseMin - o->_depth, gapMin);
    } else {
      space = Units::fsize(_over.spaceUnit, _over.space, env);
    }
    const auto kern = sptrOf<StrutBox>(0.f, space, 0.f, 0.f);
    kern->_shift = delta / 2;
    vbox->add(kern);
  }

  // base
  auto center = wrap(b, 0.f);
  vbox->add(center);

  // calculate future height of the vertical box, to make sure that the
  // base stays on the baseline
  const auto h = vbox->_height + vbox->_depth - center->_depth;

  // under script + space
  if (u != nullptr && !u->isSpace()) {
    float space = 0.f;
    if (_under.isAutoSpace) {
      const auto gapMin = math.lowerLimitGapMin() * env.scale();
      const auto baselineDropMin = math.lowerLimitBaselineDropMin() * env.scale();
      space = std::max(baselineDropMin - u->_height, gapMin);
    } else {
      space = Units::fsize(_under.spaceUnit, _under.space, env);
    }
    const auto kern = sptrOf<StrutBox>(0.f, space, 0.f, 0.f);
    kern->_shift = delta / 2;
    vbox->add(kern);
    auto ub = wrap(u, -delta / 2);
    vbox->add(ub);
  }

  // calculate height and depth
  vbox->_depth = vbox->_height + vbox->_depth - h;
  vbox->_height = h;

  return {vbox, center->_shift - vbox->leftMostPos()};
}
