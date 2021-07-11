#include "atom/atom_accent.h"
#include "env/env.h"
#include "env/units.h"
#include "box/box_single.h"
#include "box/box_group.h"

using namespace tex;
using namespace std;

void AccentedAtom::setupBase(const sptr<Atom>& base) {
  auto a = dynamic_cast<AccentedAtom*>(base.get());
  if (a != nullptr) _base = a->_base;
  else _base = base;
}

AccentedAtom::AccentedAtom(const sptr<Atom>& base, const string& name, bool fitSize, bool fake) {
  _accenter = SymbolAtom::get(name);
  if (_accenter == nullptr) {
    throw ex_symbol_not_found(name);
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
    throw ex_invalid_symbol_type(
      "The symbol with the name '"
      + name + "' is not defined as an accent (type='acc')!"
    );
  }
}

sptr<Box> AccentedAtom::createBox(Env& env) {
  // create accentee box in cramped style
  auto accentee = (
    _accentee == nullptr
    ? StrutBox::empty()
    : env.withStyle(env.crampStyle(), [&](Env& cramp) { return _accentee->createBox(cramp); })
  );

  float topAccent = Otf::undefinedMathValue;
  if (auto sym = dynamic_cast<CharSymbol*>(_base.get()); sym != nullptr) {
    topAccent = sym->getChar(env).topAccentAttachment();
  }
  topAccent = topAccent == Otf::undefinedMathValue ? accentee->_width / 2.f : topAccent;

  // function to retrieve best char from the accent symbol to match the accentee box's width
  const auto& getChar = [&]() {
    const auto& chr = ((SymbolAtom*) _accenter.get())->getChar(env);
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
  vbox->add(accenter);
  // kerning
  const auto delta = (
    _fakeAccent
    ? Units::fsize(UnitType::mu, 1, env)
    : -min(accentee->_height, env.xHeight())
  );
  vbox->add(sptrOf<StrutBox>(0.f, delta, 0.f, 0.f));
  // accentee
  vbox->add(accentee);

  // set height and depth of the vertical box
  const auto total = vbox->_height + vbox->_depth;
  vbox->_depth = accentee->_depth;
  vbox->_height = total - accentee->_depth;

  return sptr<Box>(vbox);
}
