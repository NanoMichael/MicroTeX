#include "env/env.h"

#include "env/units.h"

using namespace std;
using namespace microtex;

float Env::PIXELS_PER_POINT = 1.f;

float Env::scale(TexStyle style) const {
  if (style < TexStyle::script) return 1;
  auto math = _fctx->mathFont().otf().mathConsts();
  i16 percent = style < TexStyle::scriptScript ? math->scriptPercentScaleDown()
                                               : math->scriptScriptPercentScaleDown();
  return percent / 100.f;
}

float Env::scale() const {
  return scale(_style);
}

Env& Env::setTextWidth(const Dimen& dimen) {
  _textWidth = Units::fsize(dimen, *this);
  return *this;
}

Env& Env::setLineSpace(const Dimen& dimen) {
  _lineSpace = Units::fsize(dimen, *this);
  return *this;
}

const MathConsts& Env::mathConsts() const {
  return *_fctx->mathFont().otf().mathConsts();
}

float Env::upem() const {
  return _fctx->mathFont().otf().em();
}

float Env::em() const {
  return upem() * scale();
}

float Env::ppem() const {
  return PIXELS_PER_POINT * textSize();
}

float Env::xHeight() const {
  return _fctx->getFont(lastFontId())->otf().xHeight() * scale();
}

float Env::space(bool isMathMode) const {
  if (isMathMode) {
    return _fctx->mathFont().otf().space() * scale();
  }
  return _fctx->mainSpace() * scale();
}

float Env::ruleThickness() const {
  return mathConsts().fractionRuleThickness() * scale();
}

float Env::axisHeight() const {
  return mathConsts().axisHeight() * scale();
}

void Env::addFontStyle(FontStyle style, FontStyle& target) {
  auto dummy = static_cast<u16>(target) | static_cast<u16>(style);
  target = static_cast<FontStyle>(dummy);
}

void Env::removeFontStyle(FontStyle style, FontStyle& target) {
  auto dummy = static_cast<u16>(target) & (~static_cast<u16>(style));
  target = static_cast<FontStyle>(dummy);
}

void Env::addMathFontStyle(FontStyle style) {
  addFontStyle(style, _mathFontStyle);
}

void Env::removeMathFontStyle(FontStyle style) {
  removeFontStyle(style, _mathFontStyle);
}

void Env::addTextFontStyle(FontStyle style) {
  addFontStyle(style, _textFontStyle);
}

void Env::removeTextFontStyle(FontStyle style) {
  removeFontStyle(style, _textFontStyle);
}

void Env::selectMathFont(const std::string& name, MathStyle style) {
  _fctx->selectMathFont(name);
  MathVersion::setMathStyle(style);
}

TexStyle Env::crampStyle() const {
  const i8 style = static_cast<i8>(_style);
  return static_cast<TexStyle>(style % 2 == 1 ? style : style + 1);
}

TexStyle Env::dnomStyle() const {
  const i8 style = static_cast<i8>(_style);
  return static_cast<TexStyle>(2 * (style / 2) + 1 + 2 - 2 * (style / 6));
}

TexStyle Env::numStyle() const {
  const i8 style = static_cast<i8>(_style);
  return static_cast<TexStyle>(style + 2 - 2 * (style / 6));
}

TexStyle Env::rootStyle() const {
  return TexStyle::scriptScript;
}

TexStyle Env::subStyle() const {
  const i8 style = static_cast<i8>(_style);
  return static_cast<TexStyle>(2 * (style / 4) + 4 + 1);
}

TexStyle Env::supStyle() const {
  const i8 style = static_cast<i8>(_style);
  return static_cast<TexStyle>(2 * (style / 4) + 4 + (style % 2));
}

Char Env::getChar(c32 code, bool isMathMode, FontStyle style) const {
  auto targetStyle = style;
  if (style == FontStyle::invalid) {
    targetStyle = isMathMode ? _mathFontStyle : _textFontStyle;
  }
  auto chr = _fctx->getChar(code, targetStyle, isMathMode);
  chr.scale = scale();
  return chr;
}

Char Env::getChar(const Symbol& sym) const {
  auto chr = _fctx->getChar(sym, _mathFontStyle);
  chr.scale = scale();
  return chr;
}
