#ifndef LATEX_ENV_H
#define LATEX_ENV_H

#include "unimath/uni_symbol.h"
#include "unimath/uni_font.h"
#include "atom/atom_space.h"

namespace tex {

class Environment2 {
private:
  TexStyle _style = TexStyle::display;
  float _textWidth = POS_INF;
  std::string _textStyle;
  bool _smallCap = false;
  float _scaleFactor = 1;

  i32 _lastFontId = FontContext::NO_FONT;
  sptr<FontContext> _tf;

  UnitType _lineSpaceUnit = UnitType::ex;
  float _lineSpace = 1.f;

  float _textSize = 1.f;

public:
  Environment2(TexStyle style, const sptr<FontContext>& fc, float textSize)
    : _style(style), _tf(fc), _textSize(textSize) {}

  inline Environment2& setTextWidth(UnitType unit, float width) {
    // TODO
    return *this;
  }

  inline Environment2& setLineSpace(UnitType unit, float space) {
    _lineSpaceUnit = unit;
    _lineSpace = space;
    return *this;
  }

  inline Environment2& setScaleFactor(float factor) {
    _scaleFactor = factor;
    return *this;
  }

  inline Environment2& setStyle(TexStyle style) {
    _style = style;
    return *this;
  }

  inline Environment2& setSmallCap(bool smallCap) {
    _smallCap = smallCap;
    return *this;
  }

  inline Environment2& setLastFontId(i32 lastFontId) {
    _lastFontId = lastFontId;
    return *this;
  }

  inline float scaleFactor() const { return _scaleFactor; }

  inline bool isSmallCap() const { return _smallCap; }

  inline float textWidth() const { return _textWidth; }

  inline TexStyle style() const { return _style; }

  inline const std::string& textStyle() const { return _textStyle; }

  inline float textSize() const { return _textSize; }

  inline float lineSpace() const {
    // TODO
  }

  inline i32 lastFontId() const {
    return _lastFontId == FontContext::NO_FONT ? _tf->mathFontId() : _lastFontId;
  }

};

}

#endif //LATEX_ENV_H
