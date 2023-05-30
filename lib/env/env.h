#ifndef MICROTEX_ENV_H
#define MICROTEX_ENV_H

#include "unimath/uni_font.h"
#include "unimath/uni_symbol.h"
#include "utils/nums.h"

namespace microtex {

struct Dimen;

class Env {
private:
  static constexpr float FIXED_TEXT_SIZE = 1000;
  // point-to-pixel conversion
  static float PIXELS_PER_POINT;

  TexStyle _style = TexStyle::display;

  i32 _lastFontId = FontContext::NO_FONT;
  sptr<FontContext> _fctx;

  float _textWidth = POS_INF;
  float _lineSpace = 1.f;
  float _fixedScale = 1.f;

  FontStyle _textFontStyle = FontStyle::none;
  FontStyle _mathFontStyle = FontStyle::none;

  void addFontStyle(FontStyle style, FontStyle& target);

  void removeFontStyle(FontStyle style, FontStyle& target);

public:
  no_copy_assign(Env);

  Env(TexStyle style, const sptr<FontContext>& fctx, float pointSize)
      : _style(style), _fctx(fctx), _fixedScale(FIXED_TEXT_SIZE / pointSize) {}

  // region statics
  /** Set the dot per inch target */
  static inline void setDpi(float dpi) { PIXELS_PER_POINT = dpi / 72.f; }

  static inline float pixelsPerPoint() { return PIXELS_PER_POINT; }

  static inline float fixedTextSize() { return FIXED_TEXT_SIZE; }
  // endregion

  // region getters and setters
  /** Get the scale factor of the current environment */
  float scale() const;

  /** Get the scale factor of the given style */
  float scale(TexStyle style) const;

  /** Set environment width with given dimension. */
  Env& setTextWidth(const Dimen& dimen);

  /** Set line space with given dimension. */
  Env& setLineSpace(const Dimen& dimen);

  /** Set current style to display formulas */
  inline Env& setStyle(TexStyle style) {
    _style = style;
    return *this;
  }

  /** Set the last font id will be used later when box is to be painted */
  inline Env& setLastFontId(i32 lastFontId) {
    _lastFontId = lastFontId == FontContext::NO_FONT ? _fctx->mathFontId() : _lastFontId;
    return *this;
  }

  /** The environment width */
  inline float textWidth() const { return _textWidth; }

  /** The style to display formulas */
  inline TexStyle style() const { return _style; }

  /** The text size, in point */
  inline float textSize() const { return FIXED_TEXT_SIZE; }

  /** The fixed scale */
  inline float fixedScale() const { return _fixedScale; }

  /** The line space to layout boxes vertically */
  inline float lineSpace() const { return _lineSpace; }

  /** The font style to display formulas */
  inline FontStyle mathFontStyle() const { return _mathFontStyle; }

  /** The font style to display text */
  inline FontStyle textFontStyle() const { return _textFontStyle; }

  /** The last used font's id, or the math font's id if no font was used */
  inline i32 lastFontId() const {
    return _lastFontId == FontContext::NO_FONT ? _fctx->mathFontId() : _lastFontId;
  }

  /** The math font id */
  inline i32 mathFontId() const { return _fctx->mathFontId(); }
  // endregion

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
  float space(bool isMathMode) const;

  /** The rule thickness, equals to the fraction rule thickness */
  float ruleThickness() const;

  /** The axis height */
  float axisHeight() const;

  /** Add a math font style to this environment */
  void addMathFontStyle(FontStyle style);

  /** Remove a math font style from this environment */
  void removeMathFontStyle(FontStyle style);

  /** Add a text font style to this environment */
  void addTextFontStyle(FontStyle style);

  /** Remove a text font style from this environment */
  void removeTextFontStyle(FontStyle style);

  /** Select math font to paint formulas from given name and style */
  void selectMathFont(const std::string& name, MathStyle style);
  // endregion

  // region Styles
  /** Test if current style is cramped */
  inline bool isCrampedStyle() const { return crampStyle() == _style; }

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
   * Get a Char describe the given character with scale information depending
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
   * Get a Char describe the given symbol with scale information depending
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
  template <typename F>
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
  template <typename F>
  auto withFontStyle(const FontStyle style, bool isMathMode, F&& f) -> decltype(f(*this)) {
    auto oldStyle = isMathMode ? _mathFontStyle : _textFontStyle;
    auto& target = isMathMode ? _mathFontStyle : _textFontStyle;
    target = style;
    auto result = f(*this);
    target = oldStyle;
    return result;
  }
};

}  // namespace microtex

#endif  // MICROTEX_ENV_H
