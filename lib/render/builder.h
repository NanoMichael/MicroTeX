#ifndef MICROTEX_BUILDER_H
#define MICROTEX_BUILDER_H

#include "env/units.h"
#include "render.h"

namespace microtex {

class Formula;

class Atom;

class RenderBuilder {
private:
  TexStyle _style = TexStyle::display;
  Dimen _textWidth;
  Dimen _lineSpace;
  float _textSize = 0;
  bool _fillWidth = false;
  color _fg = black;
  Alignment _align = Alignment::none;
  std::string _mathFontName;
  std::string _mainFontName;

public:
  RenderBuilder() = default;

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

  inline RenderBuilder& setFillWidth(bool i) {
    _fillWidth = i;
    return *this;
  }

  RenderBuilder& setLineSpace(const Dimen& dimen);

  Render* build(const sptr<Atom>& f);

  Render* build(Formula& f);
};

}  // namespace microtex

#endif  // MICROTEX_BUILDER_H
