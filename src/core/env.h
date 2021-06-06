#ifndef LATEX_ENV_H
#define LATEX_ENV_H

#include "unimath/uni_symbol.h"
#include "unimath/uni_font.h"
#include "atom/atom_space.h"

namespace tex {

class Env {
private:
  // point-to-pixel conversion
  static float PIXELS_PER_POINT;

  TexStyle _style = TexStyle::display;
  float _textWidth = POS_INF;
  std::string _textStyle;
  bool _smallCap = false;
  float _scaleFactor = 1.f;

  i32 _lastFontId = FontContext::NO_FONT;
  sptr<FontContext> _fc;

  UnitType _lineSpaceUnit = UnitType::ex;
  float _lineSpace = 1.f;

  /** The text size in point */
  float _textSize = 1.f;

  FontStyle _fontStyle = FontStyle::none;

  /** Get the scale factor of the given style */
  float scale(TexStyle style) const;

public:
  no_copy_assign(Env);

  Env(TexStyle style, const sptr<FontContext>& fc, float textSize)
    : _style(style), _fc(fc), _textSize(textSize) {}

  inline Env& setTextWidth(UnitType unit, float width) {
    // TODO
    return *this;
  }

  inline Env& setLineSpace(UnitType unit, float space) {
    _lineSpaceUnit = unit;
    _lineSpace = space;
    return *this;
  }

  inline Env& setScaleFactor(float factor) {
    _scaleFactor = factor;
    return *this;
  }

  inline Env& setStyle(TexStyle style) {
    _style = style;
    return *this;
  }

  inline Env& setSmallCap(bool smallCap) {
    _smallCap = smallCap;
    return *this;
  }

  inline Env& setLastFontId(i32 lastFontId) {
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
    return _lastFontId == FontContext::NO_FONT ? _fc->mathFontId() : _lastFontId;
  }

  /** Set the dot per inch target */
  static void setDpi(float dpi);

  static float pixelsPerPoint();

  /** Helper function to get math constants */
  const MathConsts& mathConsts() const;

  /** units per em, environment independent */
  float upem() const;

  /** The em size of the current environment */
  float em() const;

  /** pixels per em */
  float ppem() const;

  /** The x-height of the current environment */
  float xHeight() const;

  float ruleThickness() const;

  Char getChar(c32 code, bool isMathMode) const;

  Char getChar(const std::string& symbolName) const;
};

}

#endif //LATEX_ENV_H
