#ifndef TINYTEX_BUILDER_H
#define TINYTEX_BUILDER_H

#include "render.h"

namespace tinytex {

class Formula;

class Atom;

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

} // namespace tinytex

#endif //TINYTEX_BUILDER_H
