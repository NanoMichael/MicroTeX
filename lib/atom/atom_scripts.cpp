#include "atom/atom_scripts.h"

#include "atom/atom_accent.h"
#include "atom/atom_basic.h"
#include "atom/atom_char.h"
#include "atom/atom_stack.h"

using namespace microtex;
using namespace std;

sptr<Box> ScriptsAtom::createBox(Env& env) {
  // if no base was given, use a phantom 'M' to place scripts
  if (_base == nullptr) {
    auto in = sptrOf<CharAtom>('M', FontStyle::rm, false);
    _base = sptrOf<PhantomAtom>(in, false, true, true);
  }

  // no scripts
  if (_sub == nullptr && _sup == nullptr) {
    return _base->createBox(env);
  }

  // display as limits
  // clang-format off
  if (_base->_limitsType == LimitsType::limits ||
      (_base->_limitsType == LimitsType::normal && env.style() == TexStyle::display)
    ) {
    // clang-format on
    const auto& over = StackArgs::autoSpace(_sup);
    const auto& under = StackArgs::autoSpace(_sub);
    return StackAtom(_base, over, under).createBox(env);
  }

  const auto& [base, scripts, space, kern, reduce, italic] = createScripts(env);
  const auto hbox = sptrOf<HBox>();
  if (_onRight) {
    hbox->add(base);
    if (std::abs(reduce + kern) > PREC) hbox->add(StrutBox::create(reduce + kern));
    hbox->add(scripts);
    hbox->add(StrutBox::create(space));
  } else {
    hbox->add(StrutBox::create(space));
    hbox->add(scripts);
    if (std::abs(reduce + kern) > PREC) hbox->add(StrutBox::create(reduce + kern));
    hbox->add(base);
  }

  return hbox;
}

ScriptResult ScriptsAtom::createScripts(Env& env) {
  // params to place scripts
  sptr<Box> kernel, base;
  float delta = 0.f;
  bool isText = false, isOperator = false;
  const MathKernRecord* kernRecord = nullptr;

  const auto checkAtom = [&](const sptr<Atom>& atom, Env& targetEnv) {
    if (auto cs = dynamic_cast<CharSymbol*>(atom.get()); cs != nullptr) {
      const auto& chr = cs->getChar(targetEnv);
      kernRecord = &(chr.glyph()->math().kernRecord());
      if (!cs->isText()) delta = chr.italic();
      isText = atom->_type != AtomType::bigOperator;
      isOperator = atom->_type == AtomType::bigOperator;
    }
  };
  const auto getMathKern = [&](float height, bool isTop) {
    if (kernRecord == nullptr) return 0.f;
    const auto& mathKern = _onRight ? (isTop ? kernRecord->topRight() : kernRecord->bottomRight())
                                    : (isTop ? kernRecord->topLeft() : kernRecord->bottomLeft());
    const auto i = mathKern.indexOf(height);
    return mathKern.value(i) * env.scale();
  };

  if (auto acc = dynamic_cast<AccentedAtom*>(_base.get()); acc != nullptr) {
    // 1. special case: accent
    // the scripts are placed around the accentee
    // clang-format off
    kernel = env.withStyle(
      env.crampStyle(),
      [&](Env& cramp) {
        checkAtom(acc->_base, cramp);
        return (acc->_base == nullptr ? sptrOf<EmptyAtom>() : acc->_base)->createBox(cramp);
      }
    );
    // clang-format on
    base = _base->createBox(env);
  } else {
    // 2. char symbol or boxed
    checkAtom(_base, env);
    kernel = base = _base->createBox(env);
  }

  const auto& math = env.mathConsts();

  auto u = 0.f, v = 0.f;
  if (!isText) {
    u = kernel->_height - math.superscriptBaselineDropMax() * env.scale();
    v = kernel->_depth + math.subscriptBaselineDropMin() * env.scale();
  }

  const auto compose = [&](const sptr<Box>& box, float extra = 0) -> ScriptResult {
    const auto scriptSpace = math.spaceAfterScript() * env.scale();
    const auto kern = kernel->_width - base->_width + extra;
    const auto widthReduce = isOperator && delta > PREC ? -delta : 0.f;
    return {base, box, scriptSpace, kern, widthReduce, delta};
  };

  if (_sup == nullptr) {
    // case 1. only subscript
    auto x = env.withStyle(env.subStyle(), [&](Env& sub) { return _sub->createBox(sub); });
    x->_shift = maxOf<float>(
      v,
      math.subscriptShiftDown() * env.scale(),
      x->_height - math.subscriptTopMax() * env.scale()
    );
    const auto kern = getMathKern(-x->_shift, false);
    return compose(x, kern);
  }

  auto x = env.withStyle(env.supStyle(), [&](Env& sup) { return _sup->createBox(sup); });
  // clang-format off
  u = maxOf<float>(
    u,
    (float) (
      env.isCrampedStyle()
      ? math.superscriptShiftUpCramped()
      : math.superscriptShiftUp()
    ) * env.scale(),
    x->_depth + math.superscriptBottomMin() * env.scale()
  );
  // clang-format on

  if (_sub == nullptr) {
    // case 2. only superscript
    x->_shift = -u;
    const auto kern = getMathKern(-x->_shift, true);
    return compose(x, kern + delta);
  }

  // case 3. both super & sub scripts
  auto y = env.withStyle(env.subStyle(), [&](Env& sub) { return _sub->createBox(sub); });
  v = maxOf<float>(v, math.subscriptShiftDown() * env.scale());

  const auto theta = math.subSuperscriptGapMin() * env.scale();
  auto sigma = (u - x->_depth) - (y->_height - v);
  if (sigma < theta) {
    v = theta + y->_height + x->_depth - u;
    const auto psi = math.superscriptBottomMaxWithSubscript() * env.scale() - (u - x->_depth);
    if (psi > 0 && (v - psi + y->_depth) > kernel->_depth) {
      u += psi;
      v -= psi;
    }
    sigma = theta;
  }

  // clang-format off
  const auto topKern =
    (
      _onRight
      ? 0
      : std::max(0.f, y->_width - x->_width)
    ) + getMathKern(u, true) + delta;
  const auto bottomKern =
    (
      _onRight
      ? 0
      : std::max(0.f, x->_width - y->_width)
    ) + getMathKern(-v, false);
  // clang-format on
  const auto kern = _onRight ? std::min(topKern, bottomKern) : 0;

  auto vbox = sptrOf<VBox>();
  x->_shift = topKern - kern;
  vbox->add(x);
  y->_shift = bottomKern - kern;
  vbox->add(y, sigma);
  vbox->_height = x->_height + u;
  vbox->_depth = y->_depth + v;

  return compose(vbox, kern);
}
