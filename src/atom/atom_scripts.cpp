#include "atom/atom_scripts.h"
#include "atom/atom_stack.h"
#include "atom/atom_char.h"
#include "atom/atom_basic.h"

using namespace tex;
using namespace std;

// TODO math kern
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
  if (_base->_limitsType == LimitsType::limits ||
      (_base->_limitsType == LimitsType::normal && env.style() == TexStyle::display)
    ) {
    const auto& over = StackArgs::autoSpace(_sup);
    const auto& under = StackArgs::autoSpace(_sub);
    return StackAtom(_base, over, under).createBox(env);
  }

  // params to place scripts
  sptr<Box> kernel, base;
  float delta = 0.f;
  bool isText = false;

  auto checkSym = [&delta, &isText](const sptr<Atom>& atom, Env& targetEnv) {
    if (auto cs = dynamic_cast<CharSymbol*>(atom.get()); cs != nullptr) {
      if (!cs->isText()) {
        delta = cs->getChar(targetEnv).italic();
      }
      isText = atom->_type != AtomType::bigOperator;
    }
  };

  if (auto acc = dynamic_cast<AccentedAtom*>(_base.get()); acc != nullptr) {
    // 1. special case: accent
    // the scripts are placed around the accentee
    kernel = env.withStyle(
      env.crampStyle(),
      [&](Env& cramp) {
        checkSym(acc->_base, cramp);
        return acc->_base->createBox(cramp);
      }
    );
    base = _base->createBox(env);
  } else {
    // 2. char symbol or boxed
    checkSym(_base, env);
    kernel = base = _base->createBox(env);
  }

  const auto& math = env.mathConsts();

  auto u = 0.f, v = 0.f;
  if (!isText) {
    u = kernel->_height - math.superscriptBaselineDropMax() * env.scale();
    v = kernel->_depth + math.subscriptBaselineDropMin() * env.scale();
  }

  auto hbox = sptrOf<HBox>(base);
  const auto scriptSpace = StrutBox::create(math.spaceAfterScript() * env.scale());
  const auto compose = [&](const sptr<Box>& box, float extra = 0) {
    const auto kern = kernel->_width - base->_width + extra;
    if (std::abs(kern) > PREC) hbox->add(StrutBox::create(kern));
    hbox->add(box);
    hbox->add(scriptSpace);
    return hbox;
  };

  if (_sup == nullptr) {
    // case 1. only subscript
    auto x = env.withStyle(env.subStyle(), [&](Env& sub) { return _sub->createBox(sub); });
    x->_shift = maxOf<float>(
      v,
      math.subscriptShiftDown() * env.scale(),
      x->_height - math.subscriptTopMax() * env.scale()
    );
    return compose(x);
  }

  auto x = env.withStyle(env.supStyle(), [&](Env& sup) { return _sup->createBox(sup); });
  u = maxOf<float>(
    u,
    (float) (
      env.isCrampedStyle()
      ? math.superscriptShiftUpCramped()
      : math.superscriptShiftUp()
    ) * env.scale(),
    x->_depth + math.superscriptBottomMin() * env.scale()
  );

  if (_sub == nullptr) {
    // case 2. only superscript
    x->_shift = -u;
    return compose(x, delta);
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

  auto vbox = sptrOf<VBox>();
  x->_shift = delta;
  vbox->add(x);
  vbox->add(y, sigma);
  vbox->_height = x->_height + u;
  vbox->_depth = y->_depth + v;

  return compose(vbox);
}
