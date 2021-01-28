#include "atom/atom_basic.h"

#include <memory>
#include <sstream>

#include "atom/box.h"
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
  return sptr<Box>(new ScaleBox(_base->createBox(env), _sx, _sy));
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
  Box* b = new HorizontalRule(drt, _width, _shift, _color, false);
  auto* vb = new VerticalBox();
  vb->add(sptr<Box>(b));
  vb->_type = AtomType::hline;
  return sptr<Box>(vb);
}

SpaceAtom UnderScoreAtom::_w(UnitType::em, 0.7f, 0, 0);
SpaceAtom UnderScoreAtom::_s(UnitType::em, 0.06f, 0, 0);

sptr<Box> UnderScoreAtom::createBox(Environment& env) {
  float drt = env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
  auto* hb = new HorizontalBox(_s.createBox(env));
  hb->add(sptr<Box>(new HorizontalRule(drt, _w.createBox(env)->_width, 0)));
  return sptr<Box>(hb);
}

CumulativeScriptsAtom::CumulativeScriptsAtom(
  const sptr<Atom>& base, const sptr<Atom>& sub, const sptr<Atom>& sup) {
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
  return sptr<Atom>(new ScriptsAtom(_base, _sub, _sup));
}

sptr<Box> CumulativeScriptsAtom::createBox(Environment& env) {
  return ScriptsAtom(_base, _sub, _sup).createBox(env);
}

sptr<Box> TextRenderingAtom::createBox(Environment& env) {
  if (_infos == nullptr) {
    return sptr<Box>(new TextRenderingBox(
      _str, _type, DefaultTeXFont::getSizeFactor(env.getStyle())));
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
  return sptr<Box>(new TextRenderingBox(
    _str, type, DefaultTeXFont::getSizeFactor(env.getStyle()), font, kerning));
}

/***************************************************************************************************
 *                                     SpaceAtom implementation                                    *
 ***************************************************************************************************/

sptr<Box> SpaceAtom::createBox(Environment& env) {
  if (!_blankSpace) {
    float w = _width * getFactor(_wUnit, env);
    float h = _height * getFactor(_hUnit, env);
    float d = _depth * getFactor(_dUnit, env);
    return sptr<Box>(new StrutBox(w, h, d, 0));
  }
  if (_blankType == SpaceType::none) return sptr<Box>(new StrutBox(env.getSpace(), 0, 0, 0));
  return Glue::get(_blankType, env);
}

pair<UnitType, float> SpaceAtom::getLength(const string& lgth) {
  if (lgth.empty()) return {UnitType::pixel, 0.f};

  size_t i = 0;
  for (; i < lgth.size() && !isalpha(lgth[i]); i++);
  float f = 0;
  valueof(lgth.substr(0, i), f);

  UnitType unit = UnitType::pixel;
  string x = lgth.substr(i);
  tolower(x);
  if (i != lgth.size()) unit = getUnit(x);

  return {unit, f};
}

pair<UnitType, float> SpaceAtom::getLength(const wstring& lgth) {
  string s;
  wide2utf8(lgth.c_str(), s);
  return getLength(s);
}

sptr<Box> BreakMarkAtom::createBox(Environment& env) {
  return sptr<Box>(new StrutBox(0, 0, 0, 0));
}

/********************************** char symbol atoms implementation ******************************/

sptr<CharFont> FixedCharAtom::getCharFont(TeXFont& tf) {
  return _cf;
}

sptr<Box> FixedCharAtom::createBox(Environment& env) {
  const auto& i = env.getTeXFont();
  TeXFont& tf = *i;
  Char c = tf.getChar(*_cf, env.getStyle());
  return sptr<Box>(new CharBox(c));
}

void SymbolAtom::_init_() {
#ifdef HAVE_LOG
  __log << "elements in _symbols" << endl;
  for (auto i : _symbols)
    __log << "\t" << *(i.second) << endl;
  __log << endl;
#endif  // HAVE_LOG
}

#ifdef HAVE_LOG

ostream& tex::operator<<(ostream& os, const SymbolAtom& s) {
  os << "SymbolAtom { "
     << "name: " << s._name << ", delimiter: " << s._delimiter << " }";
  return os;
}

#endif  // HAVE_LOG

SymbolAtom::SymbolAtom(const string& name, AtomType type, bool del) : _unicode(0) {
  _name = name;
  if (type == AtomType::bigOperator) _limitsType = LimitsType::normal;
  _delimiter = del;
  this->_type = type;
}

sptr<Box> SymbolAtom::createBox(Environment& env) {
  const auto& i = env.getTeXFont();
  TeXFont& tf = *i;
  TexStyle style = env.getStyle();
  Char c = tf.getChar(_name, style);
  sptr<Box> cb(new CharBox(c));
  if (env.getSmallCap() && _unicode != 0 && islower(_unicode)) {
    // find if exists in mapping
    auto it = Formula::_symbolTextMappings.find(toupper(_unicode));
    if (it != Formula::_symbolFormulaMappings.end()) {
      const string& name = it->second;
      try {
        sptr<Box> cx(new CharBox(tf.getChar(name, style)));
        cb = sptr<Box>(new ScaleBox(cx, 0.8f, 0.8f));
      } catch (ex_symbol_mapping_not_found& e) {}
    }
  }
  if (_type == AtomType::bigOperator) {
    if (style < TexStyle::text && tf.hasNextLarger(c)) c = tf.getNextLarger(c, style);
    cb = sptr<Box>(new CharBox(c));
    cb->_shift = -(cb->_height + cb->_depth) / 2.f - tf.getAxisHeight(style);
    float delta = c.getItalic();
    sptr<HorizontalBox> hb(new HorizontalBox(cb));
    if (delta > PREC) hb->add(sptr<Box>(new StrutBox(delta, 0, 0, 0)));
    return hb;
  }
  return cb;
}

sptr<CharFont> SymbolAtom::getCharFont(TeXFont& tf) {
  return tf.getChar(_name, TexStyle::display).getCharFont();
}

void SymbolAtom::addSymbolAtom(const string& file) {
  TeXSymbolParser parser(file);
  parser.readSymbols(_symbols);
}

void SymbolAtom::addSymbolAtom(const sptr<SymbolAtom>& sym) {
  _symbols[sym->_name] = sym;
}

sptr<SymbolAtom> SymbolAtom::get(const string& name) {
  auto it = _symbols.find(name);
  if (it == _symbols.end()) throw ex_symbol_not_found(name);
  return it->second;
}

Char CharAtom::getChar(TeXFont& tf, TexStyle style, bool smallCap) {
  wchar_t chr = _c;
  if (smallCap) {
    if (islower(_c)) chr = toupper(_c);
  }
  if (_textStyle.empty()) return tf.getDefaultChar(chr, style);
  return tf.getChar(chr, _textStyle, style);
}

sptr<CharFont> CharAtom::getCharFont(TeXFont& tf) {
  return getChar(tf, TexStyle::display, false).getCharFont();
}

sptr<Box> CharAtom::createBox(Environment& env) {
  if (_textStyle.empty()) {
    const string& ts = env.getTextStyle();
    if (!ts.empty()) _textStyle = ts;
  }
  bool smallCap = env.getSmallCap();
  Char ch = getChar(*env.getTeXFont(), env.getStyle(), smallCap);
  sptr<Box> box(new CharBox(ch));
  if (smallCap && islower(_c)) {
    // we have a small capital
    box = sptr<Box>(new ScaleBox(box, 0.8f, 0.8f));
  }
  return box;
}

/************************************** row atom implementation ***********************************/

bool RowAtom::_breakEveywhere = false;
bitset<16> RowAtom::_binSet(_initBinset_());
bitset<16> RowAtom::_ligKernSet(_initLigKernSet_());

bitset<16> RowAtom::_initBinset_() {
  bitset<16> binSet;
  binSet
    .set(static_cast<i8>(AtomType::binaryOperator))
    .set(static_cast<i8>(AtomType::bigOperator))
    .set(static_cast<i8>(AtomType::relation))
    .set(static_cast<i8>(AtomType::opening))
    .set(static_cast<i8>(AtomType::punctuation));
  return binSet;
}

bitset<16> RowAtom::_initLigKernSet_() {
  bitset<16> ligKern;
  ligKern
    .set(static_cast<i8>(AtomType::ordinary))
    .set(static_cast<i8>(AtomType::bigOperator))
    .set(static_cast<i8>(AtomType::binaryOperator))
    .set(static_cast<i8>(AtomType::relation))
    .set(static_cast<i8>(AtomType::opening))
    .set(static_cast<i8>(AtomType::closing))
    .set(static_cast<i8>(AtomType::punctuation));
  return ligKern;
}

RowAtom::RowAtom(const sptr<Atom>& el)
  : _lookAtLastAtom(false), _previousAtom(nullptr), _breakable(true) {
  if (el != nullptr) {
    auto* x = dynamic_cast<RowAtom*>(el.get());
    if (x != nullptr) {
      // no need to make an mrow the only element of a mrow
      _elements.insert(_elements.end(), x->_elements.begin(), x->_elements.end());
    } else {
      _elements.push_back(el);
    }
  }
}

sptr<Atom> RowAtom::getFirstAtom() {
  if (!_elements.empty()) return _elements.front();
  return nullptr;
}

sptr<Atom> RowAtom::popLastAtom() {
  if (!_elements.empty()) {
    sptr<Atom> x = _elements.back();
    _elements.pop_back();
    return x;
  }
  return sptr<Atom>(new SpaceAtom(UnitType::point, 0.f, 0.f, 0.f));
}

sptr<Atom> RowAtom::get(size_t pos) {
  if (pos >= _elements.size()) return sptr<Atom>(new SpaceAtom(UnitType::point, 0, 0, 0));
  return _elements[pos];
}

void RowAtom::add(const sptr<Atom>& el) {
  if (el != nullptr) _elements.push_back(el);
}

void RowAtom::change2Ord(Dummy* cur, Dummy* prev, Atom* next) {
  AtomType type = cur->leftType();
  if ((type == AtomType::binaryOperator) &&
      ((prev == nullptr || _binSet[static_cast<i8>(prev->rightType())]) || next == nullptr)) {
    cur->type = AtomType::ordinary;
  } else if (next != nullptr && cur->rightType() == AtomType::binaryOperator) {
    AtomType nextType = next->leftType();
    if (nextType == AtomType::relation ||
        nextType == AtomType::closing ||
        nextType == AtomType::punctuation) {
      cur->type = AtomType::ordinary;
    }
  }
}

AtomType RowAtom::leftType() const {
  if (_elements.empty()) return AtomType::ordinary;
  return _elements.front()->leftType();
}

AtomType RowAtom::rightType() const {
  if (_elements.empty()) return AtomType::ordinary;
  return _elements.back()->rightType();
}

sptr<Box> RowAtom::createBox(Environment& env) {
  auto x = env.getTeXFont();
  TeXFont& tf = *x;
  auto* hbox = new HorizontalBox();

  // convert atoms to boxes and add to the horizontal box
  int e = _elements.size() - 1;
  for (int i = -1; i < e;) {
    auto at = _elements[++i];
    bool markAdded = false;
    auto* ba = dynamic_cast<BreakMarkAtom*>(at.get());
    while (ba != nullptr) {
      if (!markAdded) markAdded = true;
      if (i < e) {
        at = _elements[++i];
        ba = dynamic_cast<BreakMarkAtom*>(at.get());
      } else {
        break;
      }
    }

    sptr<Dummy> atom(new Dummy(at));
    // if necessary, change BIN type to ORD
    // i.e. for formula: $+ e - f$, the plus sign should be traded as an ordinary type
    sptr<Atom> nextAtom(nullptr);
    if (i < e) nextAtom = _elements[i + 1];
    change2Ord(atom.get(), _previousAtom.get(), nextAtom.get());
    // check for ligature or kerning
    float kern = 0;
    while (i < e && atom->rightType() == AtomType::ordinary && atom->isCharSymbol()) {
      auto next = _elements[++i];
      auto* c = dynamic_cast<CharSymbol*>(next.get());
      if (c != nullptr && _ligKernSet[static_cast<i8>(next->leftType())]) {
        atom->markAsTextSymbol();
        auto l = atom->getCharFont(tf);
        auto r = c->getCharFont(tf);
        auto lig = tf.getLigature(*l, *r);
        if (lig == nullptr) {
          kern = tf.getKern(*l, *r, env.getStyle());
          i--;
          break;  // iterator remains unchanged (no ligature!)
        } else {
          // fixed with ligature
          atom->changeAtom(std::make_shared<FixedCharAtom>(lig));
        }
      } else {
        i--;
        break;
      }  // iterator remains unchanged
    }

    // insert glue, unless it's the first element of the row
    // or this element or the next is a kerning
    if (i != 0 && _previousAtom != nullptr && !_previousAtom->isKern() && !atom->isKern()) {
      hbox->add(Glue::get(_previousAtom->rightType(), atom->leftType(), env));
    }
    // insert atom's box
    atom->setPreviousAtom(_previousAtom);
    auto b = atom->createBox(env);
    auto* cb = dynamic_cast<CharBox*>(b.get());
    if (cb != nullptr && atom->isCharInMathMode()) {
      // When we have a single char, we need to add italic correction
      // As an example: (TVY) looks crappy...
      cb->addItalicCorrectionToWidth();
    }

    if (_breakable) {
      if (_breakEveywhere) {
        hbox->addBreakPosition(hbox->_children.size());
      } else {
        auto ca = dynamic_cast<CharAtom*>(at.get());
        if (markAdded || (ca != nullptr && isdigit(ca->getCharacter()))) {
          hbox->addBreakPosition(hbox->_children.size());
        }
      }
    }

    hbox->add(b);

    // set last used font id (for next atom)
    env.setLastFontId(b->getLastFontId());

    // insert kerning
    if (abs(kern) > PREC) hbox->add(sptr<Box>(new StrutBox(kern, 0, 0, 0)));

    // kerning do not interfere with the normal glue-rules without kerning
    if (!atom->isKern()) _previousAtom = atom;
  }
  // reset previous atom
  _previousAtom = nullptr;
  return sptr<Box>(hbox);
}

void RowAtom::setPreviousAtom(const sptr<Dummy>& prev) {
  _previousAtom = prev;
}

/************************************ VRowAtom implementation *************************************/

VRowAtom::VRowAtom() {
  _addInterline = false;
  _valign = Alignment::center;
  _halign = Alignment::none;
  _raise = sptrOf<SpaceAtom>(UnitType::ex, 0, 0, 0);
}

VRowAtom::VRowAtom(const sptr<Atom>& el) {
  _addInterline = false;
  _valign = Alignment::center;
  _halign = Alignment::none;
  _raise = sptrOf<SpaceAtom>(UnitType::ex, 0, 0, 0);
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
  _raise = sptrOf<SpaceAtom>(unit, r, 0, 0);
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
  auto* vb = new VerticalBox();
  sptr<Box> interline(new StrutBox(0, env.getInterline(), 0, 0));

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
      auto hb = sptr<Box>(new HorizontalBox(box, maxWidth, _halign));
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
    float t = vb->getSize() == 0 ? 0 : vb->_children.front()->_height;
    vb->_height = t;
    vb->_depth = vb->_depth + vb->_height - t;
  } else if (_valign == Alignment::center) {
    float axis = env.getTeXFont()->getAxisHeight(env.getStyle());
    float h = vb->_height + vb->_depth;
    vb->_height = h / 2 + axis;
    vb->_depth = h / 2 - axis;
  } else {
    float t = vb->getSize() == 0 ? 0 : vb->_children.back()->_depth;
    vb->_height = vb->_depth + vb->_height - t;
    vb->_depth = t;
  }
  return sptr<Box>(vb);
}

/*************************************** color atom implementation ********************************/

const color ColorAtom::_default = black;

ColorAtom::ColorAtom(const sptr<Atom>& atom, color bg, color c) : _background(bg), _color(c) {
  _elements = sptrOf<RowAtom>(atom);
}

void ColorAtom::defineColor(const string& name, color c) {
  _colors[name] = c;
}

color ColorAtom::getColor(string s) {
  if (s.empty()) return _default;
  trim(s);
  // #AARRGGBB formatted color
  if (s[0] == '#') return decode(s);
  if (s.find(',') == string::npos) {
    // find from predefined colors
    auto it = _colors.find(tolower(s));
    if (it != _colors.end()) return it->second;
    // AARRGGBB formatted color
    if (s.find('.') == string::npos) return decode("#" + s);
    // gray color
    float x = 0.f;
    valueof(s, x);
    if (x != 0.f) {
      float g = min(1.f, max(x, 0.f));
      return rgb(g, g, g);
    }
    return _default;
  }

  auto en = string::npos;
  StrTokenizer toks(s, ";,");
  int n = toks.count();
  if (n == 3) {
    // RGB model
    string R = toks.next();
    string G = toks.next();
    string B = toks.next();

    float r = 0.f, g = 0.f, b = 0.f;
    valueof(trim(R), r);
    valueof(trim(G), g);
    valueof(trim(B), b);

    if (r == 0.f && g == 0.f && b == 0.f) return _default;

    if (r == (int) r && g == (int) g && b == (int) b &&
        R.find('.') == en && G.find('.') == en && B.find('.') == en) {
      int ir = (int) min(255.f, max(0.f, r));
      int ig = (int) min(255.f, max(0.f, g));
      int ib = (int) min(255.f, max(0.f, b));
      return rgb(ir, ig, ib);
    }
    r = min(1.f, max(0.f, r));
    g = min(1.f, max(0.f, g));
    b = min(1.f, max(0.f, b));
    return rgb(r, g, b);
  } else if (n == 4) {
    // CMYK model
    float c = 0.f, m = 0.f, y = 0.f, k = 0.f;
    string C = toks.next();
    string M = toks.next();
    string Y = toks.next();
    string K = toks.next();
    valueof(trim(C), c);
    valueof(trim(M), m);
    valueof(trim(Y), y);
    valueof(trim(K), k);

    if (c == 0.f && m == 0.f && y == 0.f && k == 0.f) return _default;

    c = min(1.f, max(0.f, c));
    m = min(1.f, max(0.f, m));
    y = min(1.f, max(0.f, y));
    k = min(1.f, max(0.f, k));

    return cmyk(c, m, y, k);
  }

  return _default;
}

sptr<Box> ColorAtom::createBox(Environment& env) {
  const auto box = _elements->createBox(env);
  return sptr<Box>(new ColorBox(box, _color, _background));
}

sptr<Box> RomanAtom::createBox(Environment& env) {
  if (_base == nullptr) return sptr<Box>(new StrutBox(0, 0, 0, 0));
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
  return sptr<Box>(new StrutBox(w, h, d, s));
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
  auto b =
    (_base == nullptr
     ? sptr<Box>(new StrutBox(0, 0, 0, 0))
     : _base->createBox(*(env.crampStyle()))
    );

  float u = b->_width;
  float s = 0;
  auto* sym = dynamic_cast<CharSymbol*>(_underbase.get());
  if (sym != nullptr) s = tf->getSkew(*(sym->getCharFont(*tf)), style);

  // retrieve best char form the accent symbol
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
  auto* vBox = new VerticalBox();

  // accent
  sptr<Box> y(nullptr);
  float italic = ch.getItalic();
  sptr<Box> cb(new CharBox(ch));
  if (_acc) cb = _accent->createBox(_changeSize ? *(env.subStyle()) : env);

  if (abs(italic) > PREC) {
    y = sptr<Box>(new HorizontalBox(sptr<Box>(new StrutBox(-italic, 0, 0, 0))));
    y->add(cb);
  } else {
    y = cb;
  }

  // if diff > 0, center accent, otherwise center base
  float diff = (u - y->_width) / 2;
  y->_shift = s + (diff > 0 ? diff : 0);
  if (diff < 0) b = sptr<Box>(new HorizontalBox(b, y->_width, Alignment::center));
  vBox->add(y);

  // kerning
  vBox->add(sptr<Box>(new StrutBox(0, _changeSize ? -delta : -b->_width, 0, 0)));
  // base
  vBox->add(b);

  // set height and depth vertical box
  float total = vBox->_height + vBox->_depth, d = b->_depth;
  vBox->_depth = d;
  vBox->_height = total - d;

  if (diff < 0) {
    auto* hb = new HorizontalBox(sptr<Box>(new StrutBox(diff, 0, 0, 0)));
    hb->add(sptr<Box>(vBox));
    hb->_width = u;
    return sptr<Box>(hb);
  }

  return sptr<Box>(vBox);
}

/************************************ UnderOverAtom implementation *******************************/

sptr<Box> UnderOverAtom::changeWidth(const sptr<Box>& b, float maxW) {
  if (b != nullptr && abs(maxW - b->_width) > PREC)
    return sptr<Box>(new HorizontalBox(b, maxW, Alignment::center));
  return b;
}

sptr<Box> UnderOverAtom::createBox(Environment& env) {
  // create boxes in right style and calculate maximum width
  auto b = (_base == nullptr ? sptr<Box>(new StrutBox(0, 0, 0, 0)) : _base->createBox(env));
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
  auto* vBox = new VerticalBox();

  // last font used by base (for mono-space atoms following)
  env.setLastFontId(b->getLastFontId());

  // over script + space
  if (_over != nullptr) {
    vBox->add(changeWidth(o, mx));
    vBox->add(sptr<Box>(SpaceAtom(_overUnit, 0, _overSpace, 0).createBox(env)));
  }

  // base
  auto c = changeWidth(b, mx);
  vBox->add(c);

  // calculate future height of the vertical box (to make sure that the
  // base stays on the baseline)
  float h = vBox->_height + vBox->_depth - c->_depth;

  // under script + space
  if (_under != nullptr) {
    vBox->add(SpaceAtom(_underUnit, 0, _underSpace, 0).createBox(env));
    vBox->add(changeWidth(u, mx));
  }

  // set height and depth
  vBox->_depth = vBox->_height + vBox->_depth - h;
  vBox->_height = h;
  return sptr<Box>(vBox);
}

/************************************ ScriptsAtom implementation **********************************/

SpaceAtom ScriptsAtom::SCRIPT_SPACE(UnitType::point, 0.5f, 0, 0);

sptr<Box> ScriptsAtom::createBox(Environment& env) {
  if (_base == nullptr) {
    sptr<Atom> in(new CharAtom(L'M', "mathnormal"));
    _base = sptr<Atom>(new PhantomAtom(in, false, true, true));
  }

  auto b = _base->createBox(env);
  sptr<Box> deltaSymbol(new StrutBox(0, 0, 0, 0));
  if (_sub == nullptr && _sup == nullptr) return b;

  TeXFont* tf = env.getTeXFont().get();
  const TexStyle style = env.getStyle();

  if (_base->_limitsType == LimitsType::limits ||
      (_base->_limitsType == LimitsType::normal && style == TexStyle::display)) {
    sptr<Atom> in(new UnderOverAtom(_base, _sub, UnitType::point, 0.3f, true, false));
    return UnderOverAtom(in, _sup, UnitType::point, 3.f, true, true).createBox(env);
  }

  sptr<HorizontalBox> hor(new HorizontalBox(b));

  int lastFontId = b->getLastFontId();
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
    sptr<Box> x(new CharBox(c));

    float axish = env.getTeXFont()->getAxisHeight(env.getStyle());
    x->_shift = -(x->_height + x->_depth) / 2 - axish;
    hor = sptrOf<HorizontalBox>(x);

    // include delta in width or not?
    delta = c.getItalic();
    deltaSymbol = SpaceAtom(SpaceType::medMuSkip).createBox(env);
    if (delta > PREC && _sub == nullptr) hor->add(sptr<Box>(new StrutBox(delta, 0, 0, 0)));

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
      hor->add(sptr<Box>(new StrutBox(delta, 0, 0, 0)));
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

  sptr<HorizontalBox> sup(new HorizontalBox(x, msiz, _align));
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
    sptr<HorizontalBox> sub(new HorizontalBox(y, msiz, _align));
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
    auto* vBox = new VerticalBox();
    sup->_shift = delta;
    vBox->add(sup);
    // recalculate inter-space
    interspace = shiftUp - x->_depth + shiftDown - y->_height;
    vBox->add(sptr<Box>(new StrutBox(0, interspace, 0, 0)));
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

sptr<Box> BigOperatorAtom::changeWidth(const sptr<Box>& b, float maxw) {
  if (b != nullptr && abs(maxw - b->_width) > PREC)
    return sptr<Box>(new HorizontalBox(b, maxw, Alignment::center));
  return b;
}

sptr<Box> BigOperatorAtom::createSideSets(Environment& env) {
  auto* sa = static_cast<SideSetsAtom*>(_base.get());
  auto sl = sa->_left, sr = sa->_right, sb = sa->_base;
  if (sb == nullptr) {
    sptr<Atom> in(new CharAtom(L'M', "mathnormal"));
    sb = sptr<Atom>(new PhantomAtom(in, false, true, true));
  }

  auto opbox = sb->createBox(env);
  auto pa = sptr<Atom>(new PlaceholderAtom(0, opbox->_height, opbox->_depth, opbox->_shift));
  pa->_limitsType = LimitsType::noLimits;
  pa->_type = AtomType::bigOperator;

  auto* l = dynamic_cast<ScriptsAtom*>(sl.get());
  auto* r = dynamic_cast<ScriptsAtom*>(sr.get());

  if (l != nullptr && l->_base == nullptr) {
    l->_base = pa;
    l->_align = Alignment::right;
  }
  if (r != nullptr && r->_base == nullptr) r->_base = pa;

  auto y = sptr<Box>(new HorizontalBox());
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
  auto* vbox = new VerticalBox();
  float bigop5 = tf->getBigOpSpacing5(style), kern = 0;

  if (_over != nullptr) {
    vbox->add(sptr<Box>(new StrutBox(0, bigop5, 0, 0)));
    x->_shift = limitsShift - x->_width / 2 + delta / 2;
    vbox->add(x);
    kern = max(tf->getBigOpSpacing1(style), tf->getBigOpSpacing3(style) - x->_depth);
    vbox->add(sptr<Box>(new StrutBox(0, kern, 0, 0)));
  }

  vbox->add(y);

  if (_under != nullptr) {
    float k = max(tf->getBigOpSpacing2(style), tf->getBigOpSpacing4(style) - z->_height);
    vbox->add(sptr<Box>(new StrutBox(0, k, 0, 0)));
    z->_shift = limitsShift - z->_width / 2 - delta / 2;
    vbox->add(z);
    vbox->add(sptr<Box>(new StrutBox(0, bigop5, 0, 0)));
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

  if ((_limitsSet && !_limits) ||
      (!_limitsSet && style >= TexStyle::text) ||
      (_base->_limitsType == LimitsType::noLimits) ||
      (_base->_limitsType == LimitsType::normal && style >= TexStyle::text)
    ) {
    // if explicitly set to not display as limits or if not set and
    // style is not display, then attach over and under as regular sub or
    // super script
    if (row != nullptr) {
      row->add(sptr<Atom>(new ScriptsAtom(_base, _under, _over)));
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
    auto in = _base == nullptr ? sptr<Box>(new StrutBox(0, 0, 0, 0))
                               : _base->createBox(env);
    y = sptr<Box>(new HorizontalBox(in));
  }

  // limits
  sptr<Box> x, z;
  if (_over != nullptr) x = _over->createBox(*(env.supStyle()));
  if (_under != nullptr) z = _under->createBox(*(env.subStyle()));

  // make boxes equally wide
  float maxW = max(
    max(x == nullptr ? 0 : x->_width, y->_width),
    z == nullptr ? 0 : z->_width);
  x = changeWidth(x, maxW);
  y = changeWidth(y, maxW);
  z = changeWidth(z, maxW);

  // build vertical box
  auto* vBox = new VerticalBox();

  float bigop5 = tf->getBigOpSpacing5(style), kern = 0;

  // over
  if (_over != nullptr) {
    vBox->add(sptr<Box>(new StrutBox(0, bigop5, 0, 0)));
    x->_shift = delta / 2;
    vBox->add(x);
    kern = max(tf->getBigOpSpacing1(style), tf->getBigOpSpacing3(style) - x->_depth);
    vBox->add(sptr<Box>(new StrutBox(0, kern, 0, 0)));
  }

  // base
  vBox->add(y);

  // under
  if (_under != nullptr) {
    float k = max(tf->getBigOpSpacing2(style), tf->getBigOpSpacing4(style) - z->_height);
    vBox->add(sptr<Box>(new StrutBox(0, k, 0, 0)));
    z->_shift = -delta / 2;
    vBox->add(z);
    vBox->add(sptr<Box>(new StrutBox(0, bigop5, 0, 0)));
  }

  // set height and depth of vertical box
  float h = y->_height, total = vBox->_height + vBox->_depth;
  if (x != nullptr) h += bigop5 + kern + x->_height + x->_depth;
  vBox->_height = h;
  vBox->_depth = total - h;

  if (row != nullptr) {
    auto* hb = new HorizontalBox(row->createBox(env));
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
    // create a phatom to place side-sets
    sptr<Atom> in(new CharAtom(L'M', "mathnormal"));
    _base = sptr<Atom>(new PhantomAtom(in, false, true, true));
  }

  auto bb = _base->createBox(env);
  auto pa = sptr<Atom>(new PlaceholderAtom(0, bb->_height, bb->_depth, bb->_shift));

  auto* l = dynamic_cast<ScriptsAtom*>(_left.get());
  auto* r = dynamic_cast<ScriptsAtom*>(_right.get());

  if (l != nullptr && l->_base == nullptr) {
    l->_base = pa;
    l->_align = Alignment::right;
  }
  if (r != nullptr && r->_base == nullptr) r->_base = pa;

  auto hb = new HorizontalBox();
  if (_left != nullptr) hb->add(_left->createBox(env));
  hb->add(bb);
  if (_right != nullptr) hb->add(_right->createBox(env));

  return sptr<Box>(hb);
}

/******************************** OverUnderDelimiter implementation *******************************/

float OverUnderDelimiter::getMaxWidth(const Box* b, const Box* del, const Box* script) {
  float mx = max(b->_width, del->_height + del->_depth);
  if (script != nullptr) mx = max(mx, script->_width);
  return mx;
}

sptr<Box> OverUnderDelimiter::createBox(Environment& env) {
  auto b = (_base == nullptr ? sptr<Box>(new StrutBox(0, 0, 0, 0)) : _base->createBox(env));
  sptr<Box> del = DelimiterFactory::create(_symbol->getName(), env, b->_width);

  sptr<Box> sb(nullptr);
  if (_script != nullptr)
    sb = _script->createBox((_over ? *(env.supStyle()) : *(env.subStyle())));

  // create centered horizontal box if smaller than maximum width
  float mx = getMaxWidth(b.get(), del.get(), sb.get());
  if (mx - b->_width > PREC) b = sptr<Box>(new HorizontalBox(b, mx, Alignment::center));

  del = sptr<Box>(new VerticalBox(del, mx, Alignment::center));
  if (sb != nullptr && mx - sb->_width > PREC)
    sb = sptr<Box>(new HorizontalBox(sb, mx, Alignment::center));

  return sptr<Box>(new OverUnderBox(b, del, sb, _kern.createBox(env)->_height, _over));
}
