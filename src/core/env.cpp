#include "core/env.h"

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
  return mathConsts().radicalRuleThickness() * scale(_style);
}

Char Env::getChar(c32 code, bool isMathMode) const {
  auto chr = _fc->getChar(code, _fontStyle, isMathMode);
  const i16 percent = scale(_style);
  return chr;
}
