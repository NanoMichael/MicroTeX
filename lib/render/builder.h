#ifndef TINYTEX_BUILDER_H
#define TINYTEX_BUILDER_H

#include "render.h"
#include "env/units.h"

namespace tinytex {

class Formula;

class Atom;

class RenderBuilder {
private:
  static bool _enableOverrideStyle;
  static TexStyle _overrideStyle;

  TexStyle _style = TexStyle::display;
  Dimen _textWidth;
  Dimen _lineSpace;
  float _textSize = 0;
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

  inline RenderBuilder& setWidth(const Dimen& dimen, Alignment align) {
    _textWidth = dimen;
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

  RenderBuilder& setLineSpace(const Dimen& dimen);

  Render* build(const sptr<Atom>& f);

  Render* build(Formula& f);
};

} // namespace tinytex

#endif //TINYTEX_BUILDER_H
