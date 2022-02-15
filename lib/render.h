#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include <functional>

#include "config.h"
#include "box/box.h"
#include "graphic/graphic.h"

namespace tinytex {

class Formula;

class Box;

class Atom;

using BoxFilter = std::function<bool(const sptr<Box>&)>;

/**
 * Object to paint formula, is baseline aligned.
 * <p>
 * You can change the size via method #setWidth and #setHeight, but only the new
 * size is larger will be handled.
 */
class CLATEXMATH_EXPORT Render {
private:
  static constexpr color DFT_COLOR = black;

  sptr<Box> _box;
  float _textSize;
  float _fixedScale;
  color _fg = black;

  void buildDebug(
    const sptr<BoxGroup>& parent,
    const sptr<Box>& box,
    const BoxFilter& filter
  );

  static sptr<BoxGroup> wrap(const sptr<Box>& box);

public:

  Render(const sptr<Box>& box, float textSize);

  /** Get the text size of this render */
  float getTextSize() const;

  /** Get the total height of this render */
  int getHeight() const;

  /** Get the size amount below the baseline, in positive */
  int getDepth() const;

  /** Get the width of this render */
  int getWidth() const;

  float getBaseline() const;

  void setTextSize(float textSize);

  /** Set the foreground color to draw. */
  void setForeground(color fg);

  void setWidth(int width, Alignment align);

  void setHeight(int height, Alignment align);

  /** Draw the formula */
  void draw(Graphics2D& g2, int x, int y);
};

class RenderBuilder {
private:
  static bool _enableOverrideStyle;
  static TexStyle _overrideStyle;

  TexStyle _style = TexStyle::display;
  UnitType _widthUnit = UnitType::none;
  UnitType _lineSpaceUnit = UnitType::none;
  float _textSize = 0, _textWidth = 0, _lineSpace = 0;
  bool _isMaxWidth = false;
  color _fg = black;
  Alignment _align = Alignment::none;
  std::string _mathFontName;
  std::string _mainFontName;

public:
  RenderBuilder() = default;

  inline static void overrideTexStyle(bool enable, TexStyle style) {
    _enableOverrideStyle = enable;
    _overrideStyle = style;
  }

  inline RenderBuilder& setStyle(TexStyle style) {
    _style = style;
    return *this;
  }

  inline RenderBuilder& setTextSize(float textSize) {
    _textSize = textSize;
    return *this;
  }

  inline RenderBuilder& setForeground(color c) {
    _fg = c;
    return *this;
  }

  inline RenderBuilder& setWidth(UnitType unit, float width, Alignment align) {
    _widthUnit = unit;
    _textWidth = width;
    _align = align;
    return *this;
  }

  inline RenderBuilder& setMathFontName(const std::string& name) {
    _mathFontName = name;
    return *this;
  }

  inline RenderBuilder& setMainFontName(const std::string& name) {
    _mainFontName = name;
    return *this;
  }

  RenderBuilder& setIsMaxWidth(bool i);

  RenderBuilder& setLineSpace(UnitType unit, float space);

  Render* build(const sptr<Atom>& f);

  Render* build(Formula& f);
};

}  // namespace tinytex

#endif  // RENDER_H_INCLUDED
