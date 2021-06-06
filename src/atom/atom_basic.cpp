#include "atom/atom_basic.h"

#include <memory>

#include "box/box_group.h"
#include "box/box_factory.h"
#include "core/core.h"
#include "core/formula.h"
#include "fonts/fonts.h"
#include "graphic/graphic.h"
#include "res/parser/formula_parser.h"

using namespace std;
using namespace tex;

/***************************************************************************************************
 *                                     basic atom implementation                                   *
 ***************************************************************************************************/

sptr<Box> ScaleAtom::createBox(Environment& env) {
  return sptrOf<ScaleBox>(_base->createBox(env), _sx, _sy);
}

sptr<Box> MathAtom::createBox(Environment& env) {
  Environment& e = *(env.copy(env.getTeXFont()->copy()));
  e.getTeXFont()->setRoman(false);
  TexStyle style = e.getStyle();
  // if parent style greater than "this style",
  // that means the parent uses smaller font size,
  // then uses parent style instead
  if (_style > style) {
    e.setStyle(_style);
  }
  auto box = _base->createBox(e);
  e.setStyle(style);
  return box;
}

sptr<Box> HlineAtom::createBox(Environment& env) {
  float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
  Box* b = new RuleBox(drt, _width, _shift, _color, false);
  auto* vb = new VBox();
  vb->add(sptr<Box>(b));
  vb->_type = AtomType::hline;
  return sptr<Box>(vb);
}

CumulativeScriptsAtom::CumulativeScriptsAtom(
  const sptr<Atom>& base, const sptr<Atom>& sub, const sptr<Atom>& sup
) {
  auto* ca = dynamic_cast<CumulativeScriptsAtom*>(base.get());
  ScriptsAtom* sa = nullptr;
  if (ca != nullptr) {
    _base = ca->_base;
    ca->_sup->add(sup);
    ca->_sub->add(sub);
    _sup = ca->_sup;
    _sub = ca->_sub;
  } else if ((sa = dynamic_cast<ScriptsAtom*>(base.get()))) {
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

sptr<Box> CumulativeScriptsAtom::createBox(Environment& env) {
  return ScriptsAtom(_base, _sub, _sup).createBox(env);
}

sptr<Box> TextRenderingAtom::createBox(Environment& env) {
  if (_infos == nullptr) {
    return sptrOf<TextRenderingBox>(
      _str, _type, DefaultTeXFont::getSizeFactor(env.getStyle()));
  }
  auto* tf = (DefaultTeXFont*) (env.getTeXFont().get());
  int type = tf->_isIt ? ITALIC : PLAIN;
  type = type | (tf->_isBold ? BOLD : 0);
  bool kerning = tf->_isRoman;
  sptr<Font> font;
  const FontInfos& infos = *_infos;
  if (tf->_isSs) {
    if (infos._sansserif.empty()) {
      font = Font::_create(infos._serif, PLAIN, 10);
    } else {
      font = Font::_create(infos._sansserif, PLAIN, 10);
    }
  } else {
    if (infos._serif.empty()) {
      font = Font::_create(infos._sansserif, PLAIN, 10);
    } else {
      font = Font::_create(infos._serif, PLAIN, 10);
    }
  }
  return sptrOf<TextRenderingBox>(_str, type, DefaultTeXFont::getSizeFactor(env.getStyle()), font, kerning);
}

SpaceAtom UnderScoreAtom::_w(UnitType::em, 0.7f, 0.f, 0.f);
SpaceAtom UnderScoreAtom::_s(UnitType::em, 0.06f, 0.f, 0.f);

sptr<Box> UnderScoreAtom::createBox(Environment& env) {
  float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
  auto* hb = new HBox(_s.createBox(env));
  hb->add(sptrOf<RuleBox>(drt, _w.createBox(env)->_width, 0.f));
  return sptr<Box>(hb);
}

/************************************ VRowAtom implementation *************************************/

VRowAtom::VRowAtom() {
  _addInterline = false;
  _valign = Alignment::center;
  _halign = Alignment::none;
  _raise = sptrOf<SpaceAtom>(UnitType::ex, 0.f, 0.f, 0.f);
}

VRowAtom::VRowAtom(const sptr<Atom>& el) {
  _addInterline = false;
  _valign = Alignment::center;
  _halign = Alignment::none;
  _raise = sptrOf<SpaceAtom>(UnitType::ex, 0.f, 0.f, 0.f);
  if (el != nullptr) {
    auto* a = dynamic_cast<VRowAtom*>(el.get());
    if (a != nullptr) {
      _elements.insert(_elements.end(), a->_elements.begin(), a->_elements.end());
    } else {
      _elements.push_back(el);
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

void VRowAtom::add(const sptr<Atom>& el) {
  if (el != nullptr) _elements.insert(_elements.begin(), el);
}

void VRowAtom::append(const sptr<Atom>& el) {
  if (el != nullptr) _elements.push_back(el);
}

sptr<Box> VRowAtom::createBox(Environment& env) {
  auto* vb = new VBox();
  auto interline = sptrOf<StrutBox>(0.f, env.getInterline(), 0.f, 0.f);

  if (_halign != Alignment::none) {
    float maxWidth = F_MIN;
    vector<sptr<Box>> boxes;
    const int s = _elements.size();
    // find the width of the widest box
    for (int i = 0; i < s; i++) {
      sptr<Box> box = _elements[i]->createBox(env);
      boxes.push_back(box);
      if (maxWidth < box->_width) maxWidth = box->_width;
    }
    // align the boxes and add it to the vertical box
    for (int i = 0; i < s; i++) {
      auto box = boxes[i];
      auto hb = sptrOf<HBox>(box, maxWidth, _halign);
      vb->add(hb);
      if (_addInterline && i < s - 1) vb->add(interline);
    }
  } else {
    // convert atoms to boxes and add to the vertical box
    const int s = _elements.size();
    for (int i = 0; i < s; i++) {
      vb->add(_elements[i]->createBox(env));
      if (_addInterline && i < s - 1) vb->add(interline);
    }
  }

  vb->_shift = -_raise->createBox(env)->_width;
  if (_valign == Alignment::top) {
    float t = vb->size() == 0 ? 0 : vb->_children.front()->_height;
    vb->_height = t;
    vb->_depth = vb->_depth + vb->_height - t;
  } else if (_valign == Alignment::center) {
    float axis = env.getTeXFont()->getAxisHeight(env.getStyle());
    float h = vb->_height + vb->_depth;
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

sptr<Box> ColorAtom::createBox(Environment& env) {
  const auto box = _elements->createBox(env);
  return sptrOf<ColorBox>(box, _color, _background);
}

sptr<Box> RomanAtom::createBox(Environment& env) {
  if (_base == nullptr) return sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f);
  Environment& c = *(env.copy(env.getTeXFont()->copy()));
  c.getTeXFont()->setRoman(true);
  return _base->createBox(c);
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

sptr<Box> PhantomAtom::createBox(Environment& env) {
  auto res = _elements->createBox(env);
  float w = (_w ? res->_width : 0);
  float h = (_h ? res->_height : 0);
  float d = (_d ? res->_depth : 0);
  float s = res->_shift;
  return sptrOf<StrutBox>(w, h, d, s);
}

/************************************ AccentedAtom implementation *********************************/

void AccentedAtom::init(const sptr<Atom>& base, const sptr<Atom>& accent) {
  _base = base;
  auto* a = dynamic_cast<AccentedAtom*>(base.get());
  if (a != nullptr) _underbase = a->_underbase;
  else _underbase = base;

  _accent = dynamic_pointer_cast<SymbolAtom>(accent);
  if (_accent == nullptr) throw ex_invalid_symbol_type("Invalid accent!");

  _acc = true;
  _changeSize = true;
}

AccentedAtom::AccentedAtom(const sptr<Atom>& base, const string& name) {
  _accent = SymbolAtom::get(name);
  if (_accent->_type == AtomType::accent) {
    _base = base;
    auto* a = dynamic_cast<AccentedAtom*>(base.get());
    if (a != nullptr) _underbase = a->_underbase;
    else _underbase = base;
  } else {
    throw ex_invalid_symbol_type(
      "The symbol with the name '"
      + name + "' is not defined as an accent ("
      + TeXSymbolParser::TYPE_ATTR + "='acc') in '"
      + TeXSymbolParser::RESOURCE_NAME + "'!"
    );
  }
  _changeSize = true;
  _acc = false;
}

AccentedAtom::AccentedAtom(const sptr<Atom>& base, const sptr<Formula>& acc) {
  if (acc == nullptr) throw ex_invalid_formula("the accent Formula can't be null!");
  _changeSize = true;
  _acc = false;
  auto root = acc->_root;
  _accent = dynamic_pointer_cast<SymbolAtom>(root);
  if (_accent == nullptr)
    throw ex_invalid_formula("The accent Formula does not represet a single symbol!");
  if (_accent->_type == AtomType::accent) {
    _base = base;
  } else {
    throw ex_invalid_symbol_type(
      "The accent Formula represents a single symbol with the name '"
      + _accent->getName() + "', but this symbol is not defined as accent ("
      + TeXSymbolParser::TYPE_ATTR + "='acc') in '"
      + TeXSymbolParser::RESOURCE_NAME + "'!"
    );
  }
}

sptr<Box> AccentedAtom::createBox(Environment& env) {
  TeXFont* tf = env.getTeXFont().get();
  const TexStyle style = env.getStyle();

  // set base in cramped style
  auto b = (
    _base == nullptr
    ? sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f)
    : _base->createBox(*(env.crampStyle()))
  );

  float u = b->_width;
  float s = 0;
  auto* sym = dynamic_cast<CharSymbol*>(_underbase.get());
  if (sym != nullptr) s = tf->getSkew(*(sym->getCharFont(*tf)), style);

  // retrieve best char from the accent symbol
  auto* acc = (SymbolAtom*) _accent.get();
  Char ch = tf->getChar(acc->getName(), style);
  while (tf->hasNextLarger(ch)) {
    Char larger = tf->getNextLarger(ch, style);
    if (larger.getWidth() <= u) ch = larger;
    else break;
  }

  // calculate delta
  float ec = -SpaceAtom::getFactor(UnitType::mu, env);
  float delta = _acc ? ec : min(b->_height, tf->getXHeight(style, ch.getFontCode()));

  // create vertical box
  auto* vBox = new VBox();

  // accent
  sptr<Box> y(nullptr);
  float italic = ch.getItalic();
  sptr<Box> cb = sptrOf<CharBox>(ch);
  if (_acc) cb = _accent->createBox(_changeSize ? *(env.subStyle()) : env);

  if (abs(italic) > PREC) {
    auto hbox = sptrOf<HBox>(sptrOf<StrutBox>(-italic, 0.f, 0.f, 0.f));
    hbox->add(cb);
    y = hbox;
  } else {
    y = cb;
  }

  // if diff > 0, center accent, otherwise center base
  float diff = (u - y->_width) / 2;
  y->_shift = s + (diff > 0 ? diff : 0);
  if (diff < 0) b = sptrOf<HBox>(b, y->_width, Alignment::center);
  vBox->add(y);

  // kerning
  vBox->add(sptrOf<StrutBox>(0.f, _changeSize ? -delta : -b->_width, 0.f, 0.f));
  // base
  vBox->add(b);

  // set height and depth vertical box
  float total = vBox->_height + vBox->_depth, d = b->_depth;
  vBox->_depth = d;
  vBox->_height = total - d;

  if (diff < 0) {
    auto* hb = new HBox(sptrOf<StrutBox>(diff, 0.f, 0.f, 0.f));
    hb->add(sptr<Box>(vBox));
    hb->_width = u;
    return sptr<Box>(hb);
  }

  return sptr<Box>(vBox);
}

/************************************ UnderOverAtom implementation *******************************/

sptr<Box> UnderOverAtom::changeWidth(const sptr<Box>& b, float maxWidth) {
  if (b != nullptr && abs(maxWidth - b->_width) > PREC)
    return sptrOf<HBox>(b, maxWidth, Alignment::center);
  return b;
}

sptr<Box> UnderOverAtom::createBox(Environment& env) {
  // create boxes in right style and calculate maximum width
  auto b = (_base == nullptr ? sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f) : _base->createBox(env));
  sptr<Box> o(nullptr);
  sptr<Box> u(nullptr);
  float mx = b->_width;
  if (_over != nullptr) {
    o = _over->createBox(_overSmall ? *(env.subStyle()) : env);
    mx = max(mx, o->_width);
  }
  if (_under != nullptr) {
    u = _under->createBox(_underSmall ? *(env.subStyle()) : env);
    mx = max(mx, u->_width);
  }

  // create vertical box
  auto* vBox = new VBox();

  // last font used by base (for mono-space atoms following)
  env.setLastFontId(b->lastFontId());

  // over script + space
  if (_over != nullptr) {
    vBox->add(changeWidth(o, mx));
    vBox->add(sptr<Box>(SpaceAtom(_overUnit, 0.f, _overSpace, 0).createBox(env)));
  }

  // base
  auto c = changeWidth(b, mx);
  vBox->add(c);

  // calculate future height of the vertical box (to make sure that the
  // base stays on the baseline)
  float h = vBox->_height + vBox->_depth - c->_depth;

  // under script + space
  if (_under != nullptr) {
    vBox->add(SpaceAtom(_underUnit, 0.f, _underSpace, 0.f).createBox(env));
    vBox->add(changeWidth(u, mx));
  }

  // set height and depth
  vBox->_depth = vBox->_height + vBox->_depth - h;
  vBox->_height = h;
  return sptr<Box>(vBox);
}

/************************************ ScriptsAtom implementation **********************************/

SpaceAtom ScriptsAtom::SCRIPT_SPACE(UnitType::point, 0.5f, 0.f, 0.f);

sptr<Box> ScriptsAtom::createBox(Environment& env) {
  if (_base == nullptr) {
    auto in = sptrOf<CharAtom>(L'M', "mathnormal");
    _base = sptrOf<PhantomAtom>(in, false, true, true);
  }

  auto b = _base->createBox(env);
  sptr<Box> deltaSymbol = sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f);
  if (_sub == nullptr && _sup == nullptr) return b;

  TeXFont* tf = env.getTeXFont().get();
  const TexStyle style = env.getStyle();

  if (_base->_limitsType == LimitsType::limits ||
      (_base->_limitsType == LimitsType::normal && style == TexStyle::display)) {
    auto in = sptrOf<UnderOverAtom>(_base, _sub, UnitType::point, 0.3f, true, false);
    return UnderOverAtom(in, _sup, UnitType::point, 3.f, true, true).createBox(env);
  }

  auto hor = sptrOf<HBox>(b);

  int lastFontId = b->lastFontId();
  // if no last font found (whitespace box), use default "mu font"
  if (lastFontId == TeXFont::NO_FONT) lastFontId = tf->getMuFontId();

  Environment subStyle = *(env.subStyle()), supStyle = *(env.supStyle());

  // set delta and preliminary shift-up and shift-down values
  float delta = 0, shiftUp = 0, shiftDown = 0;

  auto* acc = dynamic_cast<AccentedAtom*>(_base.get());
  auto* sym = dynamic_cast<SymbolAtom*>(_base.get());
  auto* cs = dynamic_cast<CharSymbol*>(_base.get());
  if (acc != nullptr) {
    // special case: accent
    auto box = acc->_base->createBox(*(env.crampStyle()));
    shiftUp = box->_height - tf->getSupDrop(supStyle.getStyle());
    shiftDown = box->_depth + tf->getSubDrop(subStyle.getStyle());
  } else if (sym != nullptr && _base->_type == AtomType::bigOperator) {
    // single big operator symbol
    Char c = tf->getChar(sym->getName(), style);
    // display style
    if (style < TexStyle::text && tf->hasNextLarger(c)) c = tf->getNextLarger(c, style);
    auto x = sptrOf<CharBox>(c);

    float axish = env.getTeXFont()->getAxisHeight(env.getStyle());
    x->_shift = -(x->_height + x->_depth) / 2 - axish;
    hor = sptrOf<HBox>(x);

    // include delta in width or not?
    delta = c.getItalic();
    deltaSymbol = SpaceAtom(SpaceType::medMuSkip).createBox(env);
    if (delta > PREC && _sub == nullptr) hor->add(sptrOf<StrutBox>(delta, 0.f, 0.f, 0.f));

    shiftUp = hor->_height - tf->getSupDrop(supStyle.getStyle());
    shiftDown = hor->_depth + tf->getSubDrop(subStyle.getStyle());
  } else if (cs != nullptr) {
    shiftUp = shiftDown = 0;
    sptr<CharFont> pcf = cs->getCharFont(*tf);
    CharFont& cf = *pcf;
    if (!cs->isMarkedAsTextSymbol() || !tf->hasSpace(cf.fontId)) {
      delta = tf->getChar(cf, style).getItalic();
    }
    if (delta > PREC && _sub == nullptr) {
      hor->add(sptrOf<StrutBox>(delta, 0.f, 0.f, 0.f));
      delta = 0;
    }
  } else {
    shiftUp = b->_height - tf->getSupDrop(supStyle.getStyle());
    shiftDown = b->_depth + tf->getSubDrop(subStyle.getStyle());
  }

  if (_sup == nullptr) {
    // only sub script
    auto x = _sub->createBox(subStyle);
    // calculate and set shift amount
    x->_shift = max(
      max(shiftDown, tf->getSub1(style)),
      x->_height - 0.8f * abs(tf->getXHeight(style, lastFontId))
    );
    hor->add(x);
    hor->add(deltaSymbol);

    return hor;
  }

  auto x = _sup->createBox(supStyle);
  float msiz = x->_width;
  if (_sub != nullptr && _align == Alignment::right) {
    msiz = max(msiz, _sub->createBox(subStyle)->_width);
  }

  auto sup = sptrOf<HBox>(x, msiz, _align);
  // add space
  sup->add(SCRIPT_SPACE.createBox(env));
  // adjust shift-up
  float p;
  if (style == TexStyle::display) p = tf->getSup1(style);
  else if (env.crampStyle()->getStyle() == style) p = tf->getSup3(style);
  else p = tf->getSup2(style);
  shiftUp = max(max(shiftUp, p), x->_depth + abs(tf->getXHeight(style, lastFontId)) / 4);

  if (_sub == nullptr) {
    // only super script
    sup->_shift = -shiftUp;
    hor->add(sup);
  } else {
    // both super and sub script
    sptr<Box> y(_sub->createBox(subStyle));
    auto sub = sptrOf<HBox>(y, msiz, _align);
    // add space
    sub->add(SCRIPT_SPACE.createBox(env));
    // adjust shift down
    shiftDown = max(shiftDown, tf->getSub2(style));
    // position both sub and super script
    float drt = tf->getDefaultRuleThickness(style);
    float interspace = shiftUp - x->_depth + shiftDown - y->_height;
    // space between sub and super script
    if (interspace < 4 * drt) {
      // too small
      shiftUp += 4 * drt - interspace;
      // set bottom super script at least 4/5 of X-height above baseline
      float psi = 0.8f * abs(tf->getXHeight(style, lastFontId)) - (shiftUp - x->_depth);

      if (psi > 0) {
        shiftUp += psi;
        shiftDown -= psi;
      }
    }

    // create total box
    auto* vBox = new VBox();
    sup->_shift = delta;
    vBox->add(sup);
    // recalculate inter-space
    interspace = shiftUp - x->_depth + shiftDown - y->_height;
    vBox->add(sptrOf<StrutBox>(0.f, interspace, 0.f, 0.f));
    vBox->add(sub);
    vBox->_height = shiftUp + x->_height;
    vBox->_depth = shiftDown + y->_depth;
    hor->add(sptr<Box>(vBox));
  }
  hor->add(deltaSymbol);
  return hor;
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

sptr<Box> BigOperatorAtom::createSideSets(Environment& env) {
  auto* sa = static_cast<SideSetsAtom*>(_base.get());
  auto sl = sa->_left, sr = sa->_right, sb = sa->_base;
  if (sb == nullptr) {
    auto in = sptrOf<CharAtom>(L'M', "mathnormal");
    sb = sptrOf<PhantomAtom>(in, false, true, true);
  }

  auto opbox = sb->createBox(env);
  auto pa = sptrOf<PlaceholderAtom>(0.f, opbox->_height, opbox->_depth, opbox->_shift);
  pa->_limitsType = LimitsType::noLimits;
  pa->_type = AtomType::bigOperator;

  auto* l = dynamic_cast<ScriptsAtom*>(sl.get());
  auto* r = dynamic_cast<ScriptsAtom*>(sr.get());

  if (l != nullptr && l->_base == nullptr) {
    l->_base = pa;
    l->_align = Alignment::right;
  }
  if (r != nullptr && r->_base == nullptr) r->_base = pa;

  auto y = sptrOf<HBox>();
  float limitsShift = 0;
  if (sl != nullptr) {
    auto lb = sl->createBox(env);
    y->add(lb);
    limitsShift = lb->_width + opbox->_width / 2;
  }
  y->add(opbox);
  if (sr != nullptr) y->add(sr->createBox(env));

  TeXFont* tf = env.getTeXFont().get();
  const TexStyle style = env.getStyle();

  float delta = 0;
  if (sb->_type == AtomType::bigOperator) {
    auto* sym = dynamic_cast<SymbolAtom*>(sb.get());
    if (sym != nullptr) {
      Char c = tf->getChar(sym->getName(), style);
      delta = c.getItalic();
    }
  }

  // under and over
  sptr<Box> x, z;
  if (_over != nullptr) x = _over->createBox(*(env.supStyle()));
  if (_under != nullptr) z = _under->createBox(*(env.subStyle()));

  // build vertical box
  auto* vbox = new VBox();
  float bigop5 = tf->getBigOpSpacing5(style), kern = 0;

  if (_over != nullptr) {
    vbox->add(sptrOf<StrutBox>(0.f, bigop5, 0.f, 0.f));
    x->_shift = limitsShift - x->_width / 2 + delta / 2;
    vbox->add(x);
    kern = max(tf->getBigOpSpacing1(style), tf->getBigOpSpacing3(style) - x->_depth);
    vbox->add(sptrOf<StrutBox>(0.f, kern, 0.f, 0.f));
  }

  vbox->add(y);

  if (_under != nullptr) {
    float k = max(tf->getBigOpSpacing2(style), tf->getBigOpSpacing4(style) - z->_height);
    vbox->add(sptrOf<StrutBox>(0.f, k, 0.f, 0.f));
    z->_shift = limitsShift - z->_width / 2 - delta / 2;
    vbox->add(z);
    vbox->add(sptrOf<StrutBox>(0.f, bigop5, 0.f, 0.f));
  }

  float h = y->_height, total = vbox->_height + vbox->_depth;
  if (x != nullptr) h += bigop5 + kern + x->_height + x->_depth;
  vbox->_height = h;
  vbox->_depth = total - h;

  return sptr<Box>(vbox);
}

sptr<Box> BigOperatorAtom::createBox(Environment& env) {
  if (dynamic_cast<SideSetsAtom*>(_base.get())) return createSideSets(env);

  TeXFont* tf = env.getTeXFont().get();
  const TexStyle style = env.getStyle();

  RowAtom* row = nullptr;
  auto Base = _base;

  auto* ta = dynamic_cast<TypedAtom*>(_base.get());
  if (ta != nullptr) {
    auto atom = ta->getBase();
    auto* ra = dynamic_cast<RowAtom*>(atom.get());
    if (ra != nullptr && ra->_lookAtLastAtom && _base->_limitsType != LimitsType::limits) {
      _base = ra->popLastAtom();
      row = ra;
    } else {
      _base = atom;
    }
  }

  if ((_limitsSet && !_limits)
      || (!_limitsSet && style >= TexStyle::text)
      || (_base->_limitsType == LimitsType::noLimits)
      || (_base->_limitsType == LimitsType::normal && style >= TexStyle::text)
    ) {
    // if explicitly set to not display as limits or if not set and
    // style is not display, then attach over and under as regular sub or
    // super script
    if (row != nullptr) {
      row->add(sptrOf<ScriptsAtom>(_base, _under, _over));
      auto b = row->createBox(env);
      row->popLastAtom();
      row->add(_base);
      _base = Base;
      return b;
    }
    return ScriptsAtom(_base, _under, _over).createBox(env);
  }

  sptr<Box> y(nullptr);
  float delta;

  auto* sym = dynamic_cast<SymbolAtom*>(_base.get());
  if (sym != nullptr && _base->_type == AtomType::bigOperator) {
    // single big operator symbol
    Char c = tf->getChar(sym->getName(), style);
    y = _base->createBox(env);
    // include delta in width
    delta = c.getItalic();
  } else {
    delta = 0;
    auto in = (
      _base == nullptr
      ? sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f)
      : _base->createBox(env)
    );
    y = sptrOf<HBox>(in);
  }

  // limits
  sptr<Box> x, z;
  if (_over != nullptr) x = _over->createBox(*(env.supStyle()));
  if (_under != nullptr) z = _under->createBox(*(env.subStyle()));

  // make boxes equally wide
  float maxW = max(
    max(x == nullptr ? 0 : x->_width, y->_width),
    z == nullptr ? 0 : z->_width
  );
  x = changeWidth(x, maxW);
  y = changeWidth(y, maxW);
  z = changeWidth(z, maxW);

  // build vertical box
  auto* vBox = new VBox();

  float bigop5 = tf->getBigOpSpacing5(style), kern = 0;

  // over
  if (_over != nullptr) {
    vBox->add(sptrOf<StrutBox>(0.f, bigop5, 0.f, 0.f));
    x->_shift = delta / 2;
    vBox->add(x);
    kern = max(tf->getBigOpSpacing1(style), tf->getBigOpSpacing3(style) - x->_depth);
    vBox->add(sptrOf<StrutBox>(0.f, kern, 0.f, 0.f));
  }

  // base
  vBox->add(y);

  // under
  if (_under != nullptr) {
    float k = max(tf->getBigOpSpacing2(style), tf->getBigOpSpacing4(style) - z->_height);
    vBox->add(sptrOf<StrutBox>(0.f, k, 0.f, 0.f));
    z->_shift = -delta / 2;
    vBox->add(z);
    vBox->add(sptrOf<StrutBox>(0.f, bigop5, 0.f, 0.f));
  }

  // set height and depth of vertical box
  float h = y->_height, total = vBox->_height + vBox->_depth;
  if (x != nullptr) h += bigop5 + kern + x->_height + x->_depth;
  vBox->_height = h;
  vBox->_depth = total - h;

  if (row != nullptr) {
    auto* hb = new HBox(row->createBox(env));
    row->add(_base);
    hb->add(sptr<Box>(vBox));
    _base = Base;
    return sptr<Box>(hb);
  }
  return sptr<Box>(vBox);
}

/*********************************** SideSetsAtom implementation **********************************/

sptr<Box> SideSetsAtom::createBox(Environment& env) {
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

sptr<Box> OverUnderDelimiter::createBox(Environment& env) {
  auto base = (_base == nullptr ? sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f) : _base->createBox(env));
  sptr<Box> del = DelimiterFactory::create(_symbol->getName(), env, base->_width);
  // TODO
  // no rotation needed
  del = sptrOf<RotateBox>(del, -90.f, Rotation::cc);

  sptr<Box> sb(nullptr);
  if (_script != nullptr) {
    sb = _script->createBox((_over ? *(env.supStyle()) : *(env.subStyle())));
  }

  // create centered horizontal box if smaller than maximum width
  float mx = getMaxWidth(base.get(), del.get(), sb.get());
  if (mx - base->_width > PREC) base = sptrOf<HBox>(base, mx, Alignment::center);

  del = sptrOf<HBox>(del, mx, Alignment::center);
  if (sb != nullptr && mx - sb->_width > PREC) {
    sb = sptrOf<HBox>(sb, mx, Alignment::center);
  }

  const auto kb = _kern.createBox(env);
  auto vbox = new VBox();
  if (_over) {
    if (sb != nullptr) {
      vbox->add(sb);
      if (kb->_height > PREC) vbox->add(kb);
    }
    vbox->add(del);
    vbox->add(base);
    const float total = vbox->_height + vbox->_depth;
    vbox->_height = total - base->_depth;
    vbox->_depth = base->_depth;
  } else {
    vbox->add(base);
    vbox->add(del);
    if (sb != nullptr) {
      if (kb->_height > PREC) vbox->add(kb);
      vbox->add(sb);
    }
    const float total = vbox->_height + vbox->_depth;
    vbox->_height = base->_height;
    vbox->_depth = total - base->_height;
  }
  return sptr<Box>(vbox);
}
