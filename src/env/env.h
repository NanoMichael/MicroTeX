#ifndef LATEX_ENV_H
#define LATEX_ENV_H

#include "unimath/uni_symbol.h"
#include "unimath/uni_font.h"
#include "utils/nums.h"

namespace tex {

class Env {
private:
  static constexpr float FIXED_TEXT_SIZE = 1000;
  // point-to-pixel conversion
  static float PIXELS_PER_POINT;

  TexStyle _style = TexStyle::display;
  std::string _textStyle;
  bool _smallCap = false;
  float _scaleFactor = 1.f;

  i32 _lastFontId = FontContext::NO_FONT;
  sptr<FontContext> _fctx;

  float _textWidth = POS_INF;
  float _lineSpace = 1.f;
  float _fixedScale = 1.f;

  FontStyle _fontStyle = FontStyle::none;

public:
  no_copy_assign(Env);

  Env(TexStyle style, const sptr<FontContext>& fctx, float pointSize)
    : _style(style), _fctx(fctx), _fixedScale(FIXED_TEXT_SIZE / pointSize) {}

  // region statics
  /** Set the dot per inch target */
  static inline void setDpi(float dpi) {
    PIXELS_PER_POINT = dpi / 72.f;
  }

  static inline float pixelsPerPoint() {
    return PIXELS_PER_POINT;
  }

  static inline float fixedTextSize() {
    return FIXED_TEXT_SIZE;
  }
  // endregion

  // region getters and setters

  /** Get the scale factor of the current environment */
  float scale() const;

  /** Get the scale factor of the given style */
  float scale(TexStyle style) const;

  /** Set environment width with given unit */
  Env& setTextWidth(UnitType unit, float width);

  /** Set line space with given unit */
  Env& setLineSpace(UnitType unit, float space);

  /** Set scale factor, this is used for scale atoms */
  inline Env& setScaleFactor(float factor) {
    _scaleFactor = factor;
    return *this;
  }

  /** Set current style to display formulas */
  inline Env& setStyle(TexStyle style) {
    _style = style;
    return *this;
  }

  /** Set if draw formula with small capitals */
  inline Env& setSmallCap(bool smallCap) {
    _smallCap = smallCap;
    return *this;
  }

  /** Set the last font id will be used later when box is to be painted */
  inline Env& setLastFontId(i32 lastFontId) {
    _lastFontId = lastFontId == FontContext::NO_FONT ? _fctx->mathFontId() : _lastFontId;
    return *this;
  }

  /** The scale factor */
  inline float scaleFactor() const { return _scaleFactor; }

  /** If draw formula with small capitals */
  inline bool isSmallCap() const { return _smallCap; }

  /** The environment width */
  inline float textWidth() const { return _textWidth; }

  /** The style to display formulas */
  inline TexStyle style() const { return _style; }

  inline const std::string& textStyle() const { return _textStyle; }

  /** The text size, in point */
  inline float textSize() const { return FIXED_TEXT_SIZE; }

  /** The fixed scale */
  inline float fixedScale() const { return _fixedScale; }

  /** The line space to layout boxes vertically */
  inline float lineSpace() const { return _lineSpace; }

  /** The font style to display formulas */
  inline FontStyle fontStyle() const { return _fontStyle; }
  // endregion

  /** The last used font's id, or the math font's id if no font was used */
  inline i32 lastFontId() const {
    return _lastFontId == FontContext::NO_FONT ? _fctx->mathFontId() : _lastFontId;
  }

  /** The math font id */
  inline i32 mathFontId() const {
    return _fctx->mathFontId();
  }

  // region Font related

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

  /** The space size of the current environment */
  float space() const;

  /** The rule thickness, equals to the fraction rule thickness */
  float ruleThickness() const;

  /** The axis height */
  float axisHeight() const;

  /** Add a font style to this environment */
  void addFontStyle(FontStyle style);

  /** Remove a font style from this environment */
  void removeFontStyle(FontStyle style);

  /** Select math font to paint formulas from given name and style */
  void selectMathFont(const std::string& name, MathStyle style);

  // endregion

  // region Styles

  /** Test if current style is cramped */
  inline bool isCrampedStyle() const {
    return crampStyle() == _style;
  }

  /** Style to display formulas in smaller size */
  TexStyle crampStyle() const;

  /** Style to display denominator */
  TexStyle dnomStyle() const;

  /** Style to display numerator */
  TexStyle numStyle() const;

  /** Style to display radical roots */
  TexStyle rootStyle() const;

  /** Style to display subscripts */
  TexStyle subStyle() const;

  /** Style to display superscripts */
  TexStyle supStyle() const;

  // endregion

  /**
   * Get a Char specifying the given character with scale information depending
   * on the current environment.
   *
   * @param code the alphanumeric character code-point
   * @param isMathMode if is in math-mode
   * @param style the font style, will use the environment font style instead if
   *        it is FontStyle::invalid, default is FontStyle::invalid
   *
   * @return the Char, Char#isValid() will return false if not found
   */
  Char getChar(c32 code, bool isMathMode, FontStyle style = FontStyle::invalid) const;

  /**
   * Get a Char specifying the given symbol with scale information depending
   * on the current environment.
   *
   * @param sym the symbol
   *
   * @return the Char, the method Char#isValid() will return false if not found
   */
  Char getChar(const Symbol& sym) const;

  /**
   * Do something with given TexStyle. This will reset the style after function #f
   * return.
   */
  template<typename F>
  auto withStyle(const TexStyle style, F&& f) -> decltype(f(*this)) {
    auto oldStyle = _style;
    setStyle(style);
    auto result = f(*this);
    setStyle(oldStyle);
    return result;
  }

  /**
   * Do something with given FontStyle. This will reset the FontStyle after function #f
   * return.
   */
  template<typename F>
  auto withFontStyle(const FontStyle style, F&& f) -> decltype(f(*this)) {
    auto oldStyle = _fontStyle;
    _fontStyle = style;
    auto result = f(*this);
    _fontStyle = oldStyle;
    return result;
  }
};

}

#endif //LATEX_ENV_H
