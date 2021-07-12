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
  float delta = 0.f;
  bool isSymbol = false;

  auto checkSym = [&delta, &isSymbol](const sptr<Atom>& atom, Env& targetEnv) {
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
  // functions calculate the raw shift
  const auto calSupShift = [isSymbol, &math, &kernel, &env](const sptr<Box>& box) {
    float shift = 0.f;
    if (isSymbol) {
      shift = -(
        env.isCrampedStyle()
        ? math.superscriptShiftUpCramped() * env.scale()
        : math.superscriptShiftUp() * env.scale()
      );
    } else {
      shift = -kernel->_height + math.superscriptBaselineDropMax() * env.scale();
    }
    const auto gapMin = math.superscriptBottomMin() * env.scale();
    return (
      shift + box->_depth > gapMin
      ? -box->_depth - gapMin
      : shift
    );
  };
  const auto calSubShift = [isSymbol, &math, &kernel, &env](const sptr<Box>& box) {
    const auto shift = (
      isSymbol
      ? math.subscriptShiftDown() * env.scale()
      : math.subscriptBaselineDropMin() * env.scale() + kernel->_depth
    );
    const auto gapMax = math.subscriptTopMax() * env.scale();
    return (
      box->_height - shift > gapMax
      ? box->_height - gapMax
      : shift
    );
  };

  auto scriptSpace = SpaceAtom(UnitType::mu, 0.5f, 0.f, 0.f).createBox(env);
  auto createBox = [&](const sptr<Atom>& atom, TexStyle style) {
    auto box = env.withStyle(style, [&](Env& e) { return atom->createBox(e); });
    auto h = sptrOf<HBox>(box);
    h->add(scriptSpace);
    return h;
  };

  if (_sup != nullptr && _sub != nullptr) {
    // 1. both super and sub scripts
    auto sup = env.withStyle(env.supStyle(), [&](Env& sup) { return _sup->createBox(sup); });
    auto sub = env.withStyle(env.subStyle(), [&](Env& sub) { return _sub->createBox(sub); });
    // calculate shift
    const auto shiftUp = calSupShift(sup);
    const auto shiftDown = calSubShift(sub);
    // calculate gap between super & sub script
    const auto supBottomPos = shiftUp + sup->_depth;
    const auto subTopPos = shiftDown - sub->_height;
    const auto gap = subTopPos - supBottomPos;
    // add super & sub script to vertical box
    auto v = sptrOf<VBox>();
    sup->_shift = delta;
    v->add(sup);
    const auto gapMin = math.subSuperscriptGapMin() * env.scale();
    v->add(sptrOf<StrutBox>(0.f, std::max(gap, gapMin), 0.f, 0.f));
    v->add(sub);
    // shift vertical box
    const auto height = v->_height + v->_depth;
    if (gap > gapMin) {
      v->_depth = sub->_depth;
      v->_height = height - sub->_depth;
      v->_shift = shiftDown;
    } else {
      v->_height = sup->_height;
      v->_depth = height - sup->_height;
      v->_shift = -math.superscriptBottomMaxWithSubscript() * env.scale() - sup->_depth;
    }
    // add to horizontal box
    auto h = sptrOf<HBox>(base);
    const auto kern = kernel->_width - base->_width;
    if (std::abs(kern) > PREC) h->add(StrutBox::create(kern));
    h->add(v);
    h->add(scriptSpace);
    return h;
  } else if (_sup == nullptr) {
    // 2. only subscript
    auto sub = createBox(_sub, env.subStyle());
    sub->_shift = calSubShift(sub);
    auto h = sptrOf<HBox>(base);
    // add kern if kernel box and base box are different
    const auto kern = kernel->_width - base->_width;
    if (std::abs(kern) > PREC) h->add(StrutBox::create(kern));
    h->add(sub);
    return h;
  } else if (_sub == nullptr) {
    // 3. only superscript
    auto sup = createBox(_sup, env.supStyle());
    sup->_shift = calSupShift(sup);
    // add to horizontal box
    auto h = sptrOf<HBox>(base);
    const auto kern = kernel->_width - base->_width + delta;
    if (std::abs(kern) > PREC) h->add(StrutBox::create(kern));
    h->add(sup);
    return h;
  }

  return StrutBox::empty();
}
