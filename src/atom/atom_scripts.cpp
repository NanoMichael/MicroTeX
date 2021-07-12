#include "atom/atom_scripts.h"
#include "atom/atom_stack.h"
#include "atom/atom_char.h"
#include "atom/atom_basic.h"

using namespace tex;
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
  if (_base->_limitsType == LimitsType::limits ||
      (_base->_limitsType == LimitsType::normal && env.style() == TexStyle::display)
    ) {
    const auto& over = StackArgs::autoSpace(_sup);
    const auto& under = StackArgs::autoSpace(_sub);
    return StackAtom(_base, over, under).createBox(env);
  }

  // params to place scripts
  sptr<Box> kernel, base;
  float delta = 0;
  bool isSymbol = false;

  auto checkSym = [&](const sptr<Atom>& atom, Env& targetEnv) {
    if (auto cs = dynamic_cast<CharSymbol*>(atom.get()); cs != nullptr) {
      if (!cs->isMarkedAsTextSymbol()) {
        delta = cs->getChar(targetEnv).italic();
      }
      isSymbol = true;
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
  } else if (auto sym = dynamic_cast<SymbolAtom*>(_base.get());
    sym != nullptr && sym->_type == AtomType::bigOperator) {
    // 2. single big operator symbol
    const auto& chr = (
      env.style() < TexStyle::text
      ? sym->getChar(env).vLarger(1)
      : sym->getChar(env)
    );
    delta = chr.italic();
    kernel = base = sptrOf<CharBox>(chr);
    isSymbol = true;
  } else {
    // 3. char symbol or boxed
    checkSym(_base, env);
    kernel = base = _base->createBox(env);
  }

  const auto& math = env.mathConsts();
  // calculate the raw shift
  const auto shiftUp = [&]() {
    if (isSymbol) {
      return -(
        env.isCrampedStyle()
        ? math.superscriptShiftUpCramped() * env.scale()
        : math.superscriptShiftUp() * env.scale()
      );
    } else {
      return -kernel->_height + math.superscriptBaselineDropMax() * env.scale();
    }
  }();
  const auto shiftDown = (
    isSymbol
    ? math.subscriptShiftDown() * env.scale()
    : math.subscriptBaselineDropMin() * env.scale() + kernel->_depth
  );

  auto scriptSpace = SpaceAtom(UnitType::mu, 0.5f, 0.f, 0.f).createBox(env);
  if (_sup != nullptr && _sub != nullptr) {
    // 1. both super and sub scripts
  } else if (_sup == nullptr) {
    // 2. only subscript
    auto sub = env.withStyle(env.subStyle(), [&](Env& sub) { return _sub->createBox(sub); });
    auto hsub = sptrOf<HBox>(sub);
    hsub->add(scriptSpace);
    // calculate shift
    const auto gapMax = math.subscriptTopMax() * env.scale();
    hsub->_shift = (
      hsub->_height - shiftDown > gapMax
      ? hsub->_height - gapMax
      : shiftDown
    );
    // add to horizontal box
    auto h = sptrOf<HBox>(base);
    // add kern if kernel box and base box are different
    const auto kern = kernel->_width - base->_width;
    if (std::abs(kern) > PREC) h->add(StrutBox::create(kern));
    h->add(hsub);
    return h;
  } else if (_sub == nullptr) {
    // 3. only superscript
    auto sup = env.withStyle(env.supStyle(), [&](Env& sup) { return _sup->createBox(sup); });
    auto hsup = sptrOf<HBox>(sup);
    hsup->add(scriptSpace);
    // calculate shift
    const auto gapMin = math.superscriptBottomMin() * env.scale();
    hsup->_shift = (
      shiftUp + hsup->_depth > gapMin
      ? -hsup->_depth - gapMin
      : shiftUp
    );
    // add to horizontal box
    auto h = sptrOf<HBox>(base);
    const auto kern = kernel->_width - base->_width + delta;
    if (std::abs(kern) > PREC) h->add(StrutBox::create(kern));
    h->add(hsup);
    return h;
  }

  return StrutBox::empty();
}


























