#ifndef LATEX_ENV_H
#define LATEX_ENV_H

#include "unimath/uni_symbol.h"
#include "unimath/uni_font.h"

namespace tex {

class Env {
private:
  // point-to-pixel conversion
  static float PIXELS_PER_POINT;

  TexStyle _style = TexStyle::display;
  std::string _textStyle;
  bool _smallCap = false;
  float _scaleFactor = 1.f;

  i32 _lastFontId = FontContext::NO_FONT;
  sptr<FontContext> _fc;

  float _textWidth = POS_INF;
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

  // region statics
  /** Set the dot per inch target */
  static void setDpi(float dpi);

  static float pixelsPerPoint();
  // endregion

  // region getters and setters
  Env& setTextWidth(UnitType unit, float width);

  Env& setLineSpace(UnitType unit, float space);

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

  inline float lineSpace() const { return _lineSpace; }
  // endregion

  /** The last used font's id, or the math font's id if no font was used */
  inline i32 lastFontId() const {
    return _lastFontId == FontContext::NO_FONT ? _fc->mathFontId() : _lastFontId;
  }

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

  /** The rule thickness, equals to the fraction rule thickness */
  float ruleThickness() const;

  /** Add a font style to this environment */
  void addFontStyle(FontStyle style);

  /**
   * Get a Char specifying the given character with scale information depending
   * on the current environment.
   *
   * @param code the alphanumeric character code
   * @param isMathMode if is in math-mode
   *
   * @return the Char, the method Char#isValid() will return false if not found
   */
  Char getChar(c32 code, bool isMathMode) const;

  /**
   * Get a Char specifying the given symbol name with scale information depending
   * on the current environment.
   *
   * @param symbolName the symbol name
   *
   * @return the Char, the method Char#isValid() will return false if not found
   */
  Char getChar(const std::string& symbolName) const;
};

}

#endif //LATEX_ENV_H
