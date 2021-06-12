#include "env/env.h"
#include "env/units.h"

using namespace std;
using namespace tex;

float Env::PIXELS_PER_POINT = 1.f;

void Env::setDpi(float dpi) {
  PIXELS_PER_POINT = dpi / 72.f;
}

float Env::pixelsPerPoint() {
  return PIXELS_PER_POINT;
}

float Env::scale(TexStyle style) const {
  if (style < TexStyle::script) return 1;
  auto math = _fc->mathFont().otf().mathConsts();
  i16 percent = (
    style < TexStyle::scriptScript
    ? math->scriptPercentScaleDown()
    : math->scriptScriptPercentScaleDown()
  );
  return percent / 100.f;
}

inline Env& Env::setTextWidth(UnitType unit, float width) {
  _textWidth = Units::fsize(unit, width, *this);
  return *this;
}

inline Env& Env::setLineSpace(UnitType unit, float space) {
  _lineSpace = Units::fsize(unit, space, *this);
  return *this;
}

inline const MathConsts& Env::mathConsts() const {
  return *_fc->mathFont().otf().mathConsts();
}

float Env::upem() const {
  return _fc->mathFont().otf().em();
}

float Env::em() const {
  return upem() * scale(_style);
}

float Env::ppem() const {
  return PIXELS_PER_POINT * _textSize;
}

float Env::xHeight() const {
  return _fc->getFont(lastFontId())->otf().xHeight() * scale(_style);
}

float Env::ruleThickness() const {
  return mathConsts().fractionRuleThickness() * scale(_style);
}

void Env::addFontStyle(FontStyle style) {
  auto dummy = static_cast<u16>(_fontStyle) | static_cast<u16>(style);
  _fontStyle = static_cast<FontStyle>(dummy);
}

Char Env::getChar(c32 code, bool isMathMode) const {
  auto chr = _fc->getChar(code, _fontStyle, isMathMode);
  chr._scale = scale(_style);
  return chr;
}

Char Env::getChar(const std::string& symbolName) const {
  auto chr = _fc->getChar(symbolName);
  chr._scale = scale(_style);
  return chr;
}
