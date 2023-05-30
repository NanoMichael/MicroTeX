#include "atom/atom_char.h"

#include "box/box_single.h"
#include "core/split.h"
#include "env/env.h"
#include "utils/utf.h"

using namespace microtex;
using namespace std;

std::string FixedCharAtom::name() const {
  // TODO name?
  return "";
}

sptr<Box> FixedCharAtom::createBox(Env& env) {
  return sptrOf<CharBox>(_chr);
}

sptr<SymbolAtom> SymbolAtom::get(const std::string& name) {
  const auto symbol = Symbol::get(name.c_str());
  return symbol == nullptr ? nullptr : sptrOf<SymbolAtom>(symbol);
}

SymbolAtom::SymbolAtom(const Symbol* symbol) noexcept : _symbol(symbol) {
  if (_symbol == nullptr) {
    _type = AtomType::none;
  } else {
    _type = _symbol->type();
    _limitsType = _symbol->limitsType();
  }
}

c32 SymbolAtom::unicode() const {
  return _symbol == nullptr ? 0 : _symbol->unicode;
}

string SymbolAtom::name() const {
  return _symbol == nullptr ? "" : _symbol->name;
}

bool SymbolAtom::isValid() const {
  return _symbol != nullptr;
}

Char SymbolAtom::getChar(Env& env) const {
  if (_symbol == nullptr) return {};
  if (env.style() >= TexStyle::script) {
    const auto& chr = env.getChar(*_symbol);
    return chr.script(0);
  }
  if (_type == AtomType::bigOperator) {
    const auto& chr = env.getChar(*_symbol);
    return env.style() < TexStyle::text ? chr.vLarger(1) : chr;
  }
  return env.getChar(*_symbol);
}

sptr<Box> SymbolAtom::createBox(Env& env) {
  const auto& chr = getChar(env);
  if (_type == AtomType::bigOperator) {
    auto box = sptrOf<CharBox>(chr);
    const auto axis = env.axisHeight();
    box->_shift = (box->_height - box->_depth) / 2 - axis;
    return sptrOf<HBox>(box);
  } else {
    return sptrOf<CharBox>(chr);
  }
}

Char CharAtom::getChar(Env& env) const {
  if (_mathMode && env.style() >= TexStyle::script) {
    const auto& chr = env.getChar(_unicode, true, _fontStyle);
    return chr.isValid() ? chr.script(0) : chr;
  }
  return env.getChar(unicode(), isMathMode(), _fontStyle);
}

std::string CharAtom::name() const {
  std::string str;
  microtex::appendToUtf8(str, _unicode);
  return str;
}

sptr<Box> CharAtom::createBox(Env& env) {
  const auto& chr = getChar(env);
  if (chr.isValid()) return sptrOf<CharBox>(chr);
  FontStyle fontStyle;
  if (_fontStyle != FontStyle::invalid) {
    fontStyle = _fontStyle;
  } else {
    fontStyle = _mathMode ? env.mathFontStyle() : env.textFontStyle();
  }
  return sptrOf<TextBox>(name(), fontStyle, Env::fixedTextSize() * env.scale());
}

sptr<Box> BreakMarkAtom::createBox(Env& env) {
  return StrutBox::empty();
}
