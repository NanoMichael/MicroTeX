#include "atom/atom_basic.h"

#include <memory>

#include "box/box_group.h"
#include "box/box_factory.h"
#include "core/core.h"
#include "core/formula.h"
#include "graphic/graphic.h"
#include "env/env.h"
#include "env/units.h"

using namespace std;
using namespace tex;

/***************************************************************************************************
 *                                     basic atom implementation                                   *
 ***************************************************************************************************/

sptr<Box> MathFontAtom::createBox(Env& env) {
  env.selectMathFont(_fontName, _mathStyle);
  return StrutBox::empty();
}

sptr<Box> ScaleAtom::createBox(Env& env) {
  return sptrOf<ScaleBox>(_base->createBox(env), _sx, _sy);
}

sptr<Box> MathAtom::createBox(Env& env) {
  const auto fontStyle = env.fontStyle();
  env.removeFontStyle(FontStyle::rm);
  const auto style = env.style();
  // if parent style greater than "this style", that means the parent uses smaller font size,
  // then uses parent style instead
  if (_style > style) {
    env.setStyle(_style);
  }
  auto box = _base->createBox(env);
  env.addFontStyle(fontStyle);
  env.setStyle(style);
  return box;
}

sptr<Box> HlineAtom::createBox(Env& env) {
  const auto drt = env.ruleThickness();
  auto b = new RuleBox(drt, _width, _shift, _color, false);
  auto vb = new VBox();
  vb->add(sptr<Box>(b));
  vb->_type = AtomType::hline;
  return sptr<Box>(vb);
}

CumulativeScriptsAtom::CumulativeScriptsAtom(
  const sptr<Atom>& base, const sptr<Atom>& sub, const sptr<Atom>& sup
) {
  if (auto ca = dynamic_cast<CumulativeScriptsAtom*>(base.get()); ca != nullptr) {
    _base = ca->_base;
    ca->_sup->add(sup);
    ca->_sub->add(sub);
    _sup = ca->_sup;
    _sub = ca->_sub;
  } else if (auto sa = dynamic_cast<ScriptsAtom*>(base.get()); sa != nullptr) {
    _base = sa->_base;
    _sup = sptrOf<RowAtom>(sa->_sup);
    _sub = sptrOf<RowAtom>(sa->_sub);
    _sup->add(sup);
    _sub->add(sub);
  } else {
    _base = base;
    _sup = sptrOf<RowAtom>(sup);
    _sub = sptrOf<RowAtom>(sub);
  }
}

void CumulativeScriptsAtom::addSuperscript(const sptr<Atom>& sup) {
  _sup->add(sup);
}

void CumulativeScriptsAtom::addSubscript(const sptr<Atom>& sub) {
  _sub->add(sub);
}

sptr<Atom> CumulativeScriptsAtom::getScriptsAtom() const {
  return sptrOf<ScriptsAtom>(_base, _sub, _sup);
}

sptr<Box> CumulativeScriptsAtom::createBox(Env& env) {
  return ScriptsAtom(_base, _sub, _sup).createBox(env);
}

/************************************ VRowAtom implementation *************************************/

VRowAtom::VRowAtom() {
  _addInterline = false;
  _valign = Alignment::center;
  _halign = Alignment::none;
  _raise = sptrOf<SpaceAtom>(UnitType::ex, 0.f, 0.f, 0.f);
}

VRowAtom::VRowAtom(const sptr<Atom>& base) {
  _addInterline = false;
  _valign = Alignment::center;
  _halign = Alignment::none;
  _raise = sptrOf<SpaceAtom>(UnitType::ex, 0.f, 0.f, 0.f);
  if (base != nullptr) {
    auto* a = dynamic_cast<VRowAtom*>(base.get());
    if (a != nullptr) {
      _elements.insert(_elements.end(), a->_elements.begin(), a->_elements.end());
    } else {
      _elements.push_back(base);
    }
  }
}

void VRowAtom::setRaise(UnitType unit, float r) {
  _raise = sptrOf<SpaceAtom>(unit, r, 0.f, 0.f);
}

sptr<Atom> VRowAtom::popLastAtom() {
  auto x = _elements.back();
  _elements.pop_back();
  return x;
}

void VRowAtom::prepend(const sptr<Atom>& el) {
  if (el != nullptr) _elements.insert(_elements.begin(), el);
}

void VRowAtom::append(const sptr<Atom>& el) {
  if (el != nullptr) _elements.push_back(el);
}

sptr<Box> VRowAtom::createBox(Env& env) {
  auto vb = new VBox();
  auto lineSpace = sptrOf<StrutBox>(0.f, env.lineSpace(), 0.f, 0.f);

  if (_halign != Alignment::none) {
    float maxWidth = F_MIN;
    vector<sptr<Box>> boxes;
    const size_t size = _elements.size();
    // find the width of the widest box
    for (auto& atom : _elements) {
      auto box = atom->createBox(env);
      boxes.push_back(box);
      maxWidth = std::max(maxWidth, box->_width);
    }
    // align the boxes and add it to the vertical box
    for (int i = 0; i < size; i++) {
      auto box = boxes[i];
      auto hb = sptrOf<HBox>(box, maxWidth, _halign);
      vb->add(hb);
      if (_addInterline && i < size - 1) vb->add(lineSpace);
    }
  } else {
    // convert atoms to boxes and add to the vertical box
    const size_t size = _elements.size();
    for (int i = 0; i < size; i++) {
      vb->add(_elements[i]->createBox(env));
      if (_addInterline && i < size - 1) vb->add(lineSpace);
    }
  }

  vb->_shift = -_raise->createBox(env)->_width;
  if (_valign == Alignment::top) {
    float t = vb->size() == 0 ? 0 : vb->_children.front()->_height;
    vb->_height = t;
    vb->_depth = vb->_depth + vb->_height - t;
  } else if (_valign == Alignment::center) {
    const float axis = env.axisHeight();
    const float h = vb->_height + vb->_depth;
    vb->_height = h / 2 + axis;
    vb->_depth = h / 2 - axis;
  } else {
    float t = vb->size() == 0 ? 0 : vb->_children.back()->_depth;
    vb->_height = vb->_depth + vb->_height - t;
    vb->_depth = t;
  }
  return sptr<Box>(vb);
}

/*************************************** color atom implementation ********************************/

const color ColorAtom::_default = black;

ColorAtom::ColorAtom(const sptr<Atom>& atom, color bg, color c)
  : _background(bg), _color(c) {
  _elements = sptrOf<RowAtom>(atom);
}

void ColorAtom::defineColor(const string& name, color c) {
  _colors[name] = c;
}

sptr<Box> ColorAtom::createBox(Env& env) {
  const auto box = _elements->createBox(env);
  return sptrOf<ColorBox>(box, _color, _background);
}

sptr<Box> RomanAtom::createBox(Env& env) {
  if (_base == nullptr) return StrutBox::empty();
  // TODO nested style?
  return env.withFontStyle(
    FontStyle::rm,
    [&](auto& v) { return _base->createBox(v); }
  );
}

PhantomAtom::PhantomAtom(const sptr<Atom>& el) {
  if (el == nullptr) _elements = sptrOf<RowAtom>();
  else _elements = sptrOf<RowAtom>(el);
  _w = _h = _d = true;
}

PhantomAtom::PhantomAtom(const sptr<Atom>& el, bool w, bool h, bool d) {
  if (el == nullptr) _elements = sptrOf<RowAtom>();
  else _elements = sptrOf<RowAtom>(el);
  _w = w, _h = h, _d = d;
}

sptr<Box> PhantomAtom::createBox(Env& env) {
  auto res = _elements->createBox(env);
  float w = (_w ? res->_width : 0);
  float h = (_h ? res->_height : 0);
  float d = (_d ? res->_depth : 0);
  float s = res->_shift;
  return sptrOf<StrutBox>(w, h, d, s);
}

/************************************ AccentedAtom implementation *********************************/

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

/************************************ UnderOverAtom implementation *******************************/

sptr<Box> UnderOverAtom::changeWidth(const sptr<Box>& b, float maxWidth) {
  if (b != nullptr && abs(maxWidth - b->_width) > PREC)
    return sptrOf<HBox>(b, maxWidth, Alignment::center);
  return b;
}

sptr<Box> UnderOverAtom::createBox(Env& env) {
  return StrutBox::empty();
}

/************************************ ScriptsAtom implementation **********************************/

SpaceAtom ScriptsAtom::SCRIPT_SPACE(UnitType::point, 0.5f, 0.f, 0.f);

sptr<Box> ScriptsAtom::createBox(Env& env) {
  return StrutBox::empty();
}

/************************************ BigOperatorAtom implementation ******************************/

void BigOperatorAtom::init(const sptr<Atom>& base, const sptr<Atom>& under, const sptr<Atom>& over) {
  _base = base;
  _under = under;
  _over = over;
  _limits = false;
  _limitsSet = false;
  _type = AtomType::bigOperator;
}

sptr<Box> BigOperatorAtom::changeWidth(const sptr<Box>& b, float maxWidth) {
  if (b != nullptr && abs(maxWidth - b->_width) > PREC)
    return sptrOf<HBox>(b, maxWidth, Alignment::center);
  return b;
}

sptr<Box> BigOperatorAtom::createSideSets(Env& env) {
  return StrutBox::empty();
}

sptr<Box> BigOperatorAtom::createBox(Env& env) {
  return StrutBox::empty();
}

/*********************************** SideSetsAtom implementation **********************************/

sptr<Box> SideSetsAtom::createBox(Env& env) {
  if (_base == nullptr) {
    // create a phantom to place side-sets
    auto in = sptrOf<CharAtom>(L'M', "mathnormal");
    _base = sptrOf<PhantomAtom>(in, false, true, true);
  }

  auto bb = _base->createBox(env);
  auto pa = sptrOf<PlaceholderAtom>(0.f, bb->_height, bb->_depth, bb->_shift);

  auto* l = dynamic_cast<ScriptsAtom*>(_left.get());
  auto* r = dynamic_cast<ScriptsAtom*>(_right.get());

  if (l != nullptr && l->_base == nullptr) {
    l->_base = pa;
    l->_align = Alignment::right;
  }
  if (r != nullptr && r->_base == nullptr) r->_base = pa;

  auto hb = new HBox();
  if (_left != nullptr) hb->add(_left->createBox(env));
  hb->add(bb);
  if (_right != nullptr) hb->add(_right->createBox(env));

  return sptr<Box>(hb);
}

/******************************** OverUnderDelimiter implementation *******************************/

float OverUnderDelimiter::getMaxWidth(const Box* b, const Box* del, const Box* script) {
  // TODO
  // float mx = max(b->_width, del->_height + del->_depth);
  float mx = max(b->_width, del->_width);
  if (script != nullptr) mx = max(mx, script->_width);
  return mx;
}

sptr<Box> OverUnderDelimiter::createBox(Env& env) {
  return StrutBox::empty();
}
