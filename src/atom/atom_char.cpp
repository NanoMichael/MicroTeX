#include "atom/atom_char.h"
#include "core/core.h"
#include "env/env.h"
#include "res/parser/formula_parser.h"

using namespace tex;
using namespace std;

sptr<Box> FixedCharAtom::createBox(Env& env) {
  return sptrOf<CharBox>(_chr);
}

SymbolAtom::SymbolAtom(const string&& name) noexcept {
  _symbol = Symbol::get(name.c_str());
  _type = _symbol == nullptr ? AtomType::none : _symbol->type;
  if (_type == AtomType::bigOperator) {
    _limitsType = LimitsType::normal;
  }
}

inline c32 SymbolAtom::unicode() const {
  return _symbol == nullptr ? 0 : _symbol->unicode;
}

inline string SymbolAtom::name() const {
  return _symbol == nullptr ? "" : _symbol->name;
}

inline bool SymbolAtom::isValid() const {
  return _symbol != nullptr;
}

Char SymbolAtom::getChar(Env& env) const {
  if (_symbol == nullptr) return {};
  if (env.isSmallCap() && unicode() != 0 && isUnicodeLower(unicode())) {
    const auto upper = toUnicodeUppper(unicode());
    if (upper == unicode()) return env.getChar(*_symbol);
    const auto chr = env.getChar(upper, true); // TODO math mode?
    return chr.isValid() ? chr : env.getChar(*_symbol);
  }
  return getChar(env);
}

sptr<Box> SymbolAtom::createBox(Env& env) {
  const auto& chr = getChar(env);
  if (_type == AtomType::bigOperator) {
    const auto& larger = env.style() > TexStyle::text ? chr.vLarger(1) : chr;
    auto box = sptrOf<CharBox>(larger);
    box->_shift = -(box->_height + box->_depth) / 2.f - env.axisHeight();
    const float delta = larger.italic();
    auto hb = sptrOf<HBox>(box);
    if (delta > PREC) hb->add(sptrOf<StrutBox>(delta, 0.f, 0.f, 0.f));
    return hb;
  } else {
    const bool doScale = unicode() != chr._code;
    auto box = sptrOf<CharBox>(chr);
    if (doScale) return sptrOf<ScaleBox>(box, 0.8f, 0.8f);
    return box;
  }
}

Char CharAtom::getChar(Env& env) const {
  c32 code = unicode();
  if (env.isSmallCap() && isUnicodeLower(code)) {
    code = toUnicodeUppper(code);
  }
  return env.getChar(code, isMathMode(), _fontStyle);
}

sptr<Box> CharAtom::createBox(Env& env) {
  const auto& chr = getChar(env);
  sptr<Box> box = sptrOf<CharBox>(chr);
  if (env.isSmallCap() && unicode() != chr._code) {
    box = sptrOf<ScaleBox>(box, 0.8f, 0.8f);
  }
  return box;
}

sptr<Box> BreakMarkAtom::createBox(Env& env) {
  return sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f);
}
