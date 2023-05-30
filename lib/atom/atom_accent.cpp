#include "atom/atom_accent.h"

#include "box/box_group.h"
#include "box/box_single.h"
#include "env/env.h"
#include "env/units.h"
#include "utils/exceptions.h"

using namespace microtex;
using namespace std;

void AccentedAtom::setupBase(const sptr<Atom>& base) {
  auto a = dynamic_cast<AccentedAtom*>(base.get());
  _base = a != nullptr ? a->_base : base;
}

AccentedAtom::AccentedAtom(const sptr<Atom>& base, const string& name, bool fitSize, bool fake) {
  _accenter = SymbolAtom::get(name);
  if (_accenter == nullptr) {
    throw ex_parse("'" + name + "' is not defined as a symbol");
  }
  _fitSize = fitSize;
  _fakeAccent = fake;
  if (_accenter->_type == AtomType::accent) {
    _accentee = base;
    setupBase(base);
    // the symbol is a real accent but specified as fake accent
    _fakeAccent = false;
  } else if (fake) {
    _accentee = base;
    setupBase(base);
  } else {
    throw ex_parse(
      "The symbol with the name '" + name + "' is not defined as an accent (type='acc')!"
    );
  }
}

sptr<Box> AccentedAtom::createBox(Env& env) {
  // create accentee box in cramped style
  // clang-format off
  auto accentee = (
    _accentee == nullptr
    ? StrutBox::empty()
    : env.withStyle(env.crampStyle(), [&](Env& cramp) { return _accentee->createBox(cramp); })
  );
  // clang-format on

  float topAccent = Otf::undefinedMathValue;
  if (auto sym = dynamic_cast<CharSymbol*>(_base.get()); sym != nullptr) {
    topAccent = sym->getChar(env).topAccentAttachment();
  }
  topAccent = topAccent == Otf::undefinedMathValue ? accentee->_width / 2.f : topAccent;

  // function to retrieve best char from the accent symbol to match the accentee box's width
  const auto& getChar = [&]() {
    const auto& chr = ((SymbolAtom*)_accenter.get())->getChar(env);
    if (!_fitSize) return chr;
    int i = 1;
    for (; i < chr.hLargerCount(); i++) {
      const auto& larger = chr.hLarger(i);
      if (larger.width() > accentee->_width) break;
    }
    return chr.hLarger(i - 1);
  };

  // accenter
  sptr<Box> accenter;
  if (_fakeAccent) {
    accenter = env.withStyle(env.subStyle(), [&](Env& sub) { return _accenter->createBox(sub); });
    accenter->_shift = topAccent - accenter->_width / 2.f;
  } else {
    const auto& chr = getChar();
    accenter = sptrOf<CharBox>(chr);
    const auto pos = chr.topAccentAttachment();
    const auto shift = pos == Otf::undefinedMathValue ? accenter->_width / 2.f : pos;
    accenter->_shift = topAccent - shift;
  }

  // add to vertical box
  auto vbox = new VBox();
  // kerning
  auto delta = 0.f;
  auto sigma = 0.f;
  if (_fakeAccent) {
    delta = Units::fsize(UnitType::mu, 1, env);
  } else if (accenter->_depth <= 0) {
    delta = -min(accentee->_height, (float)env.mathConsts().accentBaseHeight());
  } else {
    // if accent has depth (e.g. \not), we need to align the accentee and accenter
    // by its baseline
    if (accentee->_height > accenter->_height) {
      auto t = accenter;
      accenter = accentee;
      accentee = t;
    }
    delta = -(accentee->_height + accenter->_depth);
    sigma = max(accenter->_depth - accentee->_depth, 0.f);
  }

  vbox->add(accenter);
  vbox->add(sptrOf<StrutBox>(0.f, delta, 0.f, 0.f));
  vbox->add(accentee);

  // set height and depth of the vertical box
  vbox->_height = vbox->vlen() - accentee->_depth;
  vbox->_depth = accentee->_depth;

  // add extra space to make sure the box is tall enough to wrap all children
  if (sigma > PREC) {
    vbox->add(sptrOf<StrutBox>(0.f, sigma, 0.f, 0.f));
  }

  return sptr<Box>(vbox);
}
