#include "atom/atom_char.h"
#include "core/split.h"
#include "env/env.h"
#include "box/box_single.h"

using namespace tex;
using namespace std;

sptr<Box> FixedCharAtom::createBox(Env& env) {
  return sptrOf<CharBox>(_chr);
}

sptr<SymbolAtom> SymbolAtom::get(const std::string& name) {
  const auto symbol = Symbol::get(name.c_str());
  return symbol == nullptr ? nullptr : sptrOf<SymbolAtom>(symbol);
}

SymbolAtom::SymbolAtom(const Symbol* symbol) noexcept: _symbol(symbol) {
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
  if (_type == AtomType::bigOperator) {
    const auto& chr = env.getChar(*_symbol);
    return env.style() < TexStyle::text ? chr.vLarger(1) : chr;
  }
  if (env.isSmallCap() && unicode() != 0 && isUnicodeLower(unicode())) {
    const auto upper = toUnicodeUpper(unicode());
    if (upper == unicode()) return env.getChar(*_symbol);
    const auto chr = env.getChar(upper, true); // TODO math mode?
    return chr.isValid() ? chr : env.getChar(*_symbol);
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
    const bool doScale = unicode() != chr.code;
    auto box = sptrOf<CharBox>(chr);
    if (doScale) return sptrOf<ScaleBox>(box, 0.8f, 0.8f);
    return box;
  }
}

Char CharAtom::getChar(Env& env) const {
  const auto code = (
    env.isSmallCap() && isUnicodeLower(unicode())
    ? toUnicodeUpper(unicode())
    : unicode()
  );
  return env.getChar(code, isMathMode(), _fontStyle);
}

sptr<Box> CharAtom::createBox(Env& env) {
  const auto& chr = getChar(env);
  sptr<Box> box = sptrOf<CharBox>(chr);
  if (env.isSmallCap() && unicode() != chr.code) {
    box = sptrOf<ScaleBox>(box, 0.8f, 0.8f);
  }
  return box;
}

sptr<Box> BreakMarkAtom::createBox(Env& env) {
  return sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f);
}
