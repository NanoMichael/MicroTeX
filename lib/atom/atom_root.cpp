#include "atom/atom_root.h"

#include "box/box_factory.h"
#include "box/box_group.h"
#include "env/env.h"

using namespace microtex;

sptr<Box> NthRoot::createBox(Env& env) {
  // clang-format off
  const auto base = (
    _base == nullptr
    ? StrutBox::empty()
    : env.withStyle(env.crampStyle(), [&](Env& cramp) { return _base->createBox(cramp); })
  );
  // clang-format on

  const auto& math = env.mathConsts();
  const auto gap = env.style() <= TexStyle::display1 ? math.radicalDisplayStyleVerticalGap()
                                                     : math.radicalVerticalGap();
  const auto theta = math.radicalRuleThickness() * env.scale();
  auto radical = microtex::createVDelim("sqrt", env, base->vlen() + gap + theta);

  const auto delta = (radical->vlen() - (base->vlen() + gap + theta)) / 2.f;
  const auto rule = sptrOf<RuleBox>(theta, base->_width, 0.f);

  auto vbox = sptrOf<VBox>();
  const auto asc = math.radicalExtraAscender() * env.scale();
  vbox->add(sptrOf<StrutBox>(0.f, asc, 0.f, 0.f));
  vbox->add(rule);
  vbox->add(sptrOf<StrutBox>(0.f, gap + delta, 0.f, 0.f));
  vbox->add(base);
  const auto h = vbox->vlen();
  vbox->_height = h - base->_depth;
  vbox->_depth = base->_depth;

  auto hbox = sptrOf<HBox>();
  radical->_shift = radical->_height - vbox->_height + asc;
  hbox->add(radical);
  hbox->add(vbox);

  // degree
  if (_root == nullptr) {
    return hbox;
  }

  const auto degree =
    env.withStyle(env.rootStyle(), [&](Env& root) { return _root->createBox(root); });
  const auto hd = sptrOf<HBox>();
  const auto bk = math.radicalKernBeforeDegree() * env.scale();
  hd->add(StrutBox::create(bk));
  hd->add(degree);
  const auto br = math.radicalDegreeBottomRaisePercent() / 100.f * hbox->vlen();
  hd->_shift = hbox->_depth - hd->_depth - br + asc + theta;

  hbox->add(0, hd);
  const auto ak = math.radicalKernAfterDegree() * env.scale();
  hbox->add(1, StrutBox::create(ak));

  return hbox;
}
