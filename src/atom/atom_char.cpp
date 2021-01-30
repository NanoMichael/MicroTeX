#include "atom/atom_char.h"
#include "core/core.h"
#include "res/parser/formula_parser.h"

using namespace tex;
using namespace std;

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

SymbolAtom::SymbolAtom(const string& name, AtomType type, bool del) noexcept: _unicode(0) {
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

sptr<Box> BreakMarkAtom::createBox(Environment& env) {
  return sptr<Box>(new StrutBox(0, 0, 0, 0));
}
