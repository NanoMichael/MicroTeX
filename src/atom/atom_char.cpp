#include "atom/atom_char.h"
#include "core/core.h"
#include "res/parser/formula_parser.h"

using namespace tex;
using namespace std;

//sptr<CharFont> FixedCharAtom::getCharFont(TeXFont& tf) {
//  return _cf;
//}

sptr<Box> FixedCharAtom::createBox(Environment& env) {
  const auto& i = env.getTeXFont();
  TeXFont& tf = *i;
  Char c = tf.getChar(*_cf, env.getStyle());
  return sptrOf<CharBox>(c);
}

SymbolAtom::SymbolAtom(const string& name, AtomType type, bool del) noexcept: _unicode(0) {
  _name = name;
  _type = type;
  if (type == AtomType::bigOperator) _limitsType = LimitsType::normal;
}

sptr<Box> SymbolAtom::createBox(Environment& env) {
  const auto& i = env.getTeXFont();
  TeXFont& tf = *i;
  TexStyle style = env.getStyle();
  Char c = tf.getChar(_name, style);
  sptr<Box> cb = sptrOf<CharBox>(c);
  if (env.getSmallCap() && _unicode != 0 && islower(_unicode)) {
    // find if exists in mapping
    auto it = Formula::_symbolTextMappings.find(toupper(_unicode));
    if (it != Formula::_symbolFormulaMappings.end()) {
      const string& name = it->second;
      try {
        auto cx = sptrOf<CharBox>(tf.getChar(name, style));
        cb = sptrOf<ScaleBox>(cx, 0.8f, 0.8f);
      } catch (ex_symbol_mapping_not_found& e) {}
    }
  }
  if (_type == AtomType::bigOperator) {
    if (style < TexStyle::text && tf.hasNextLarger(c)) c = tf.getNextLarger(c, style);
    cb = sptrOf<CharBox>(c);
    cb->_shift = -(cb->_height + cb->_depth) / 2.f - tf.getAxisHeight(style);
    float delta = c.getItalic();
    auto hb = sptrOf<HBox>(cb);
    if (delta > PREC) hb->add(sptrOf<StrutBox>(delta, 0.f, 0.f, 0.f));
    return hb;
  }
  return cb;
}

//sptr<CharFont> SymbolAtom::getCharFont(TeXFont& tf) {
//  return tf.getChar(_name, TexStyle::display).getCharFont();
//}

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

//sptr<CharFont> CharAtom::getCharFont(TeXFont& tf) {
//    std::cout << "???" << std::endl;
//  return getChar(tf, TexStyle::display, false).getCharFont();
//}

sptr<Box> CharAtom::createBox(Environment& env) {
  if (_textStyle.empty()) {
    const string& ts = env.getTextStyle();
    if (!ts.empty()) _textStyle = ts;
  }
  bool smallCap = env.getSmallCap();
  Char ch = getChar(*env.getTeXFont(), env.getStyle(), smallCap);
  sptr<Box> box = sptrOf<CharBox>(ch);
  if (smallCap && islower(_c)) {
    // we have a small capital
    box = sptrOf<ScaleBox>(box, 0.8f, 0.8f);
  }
  return box;
}

sptr<Box> BreakMarkAtom::createBox(Environment& env) {
  return sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f);
}
