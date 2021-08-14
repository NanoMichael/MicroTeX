#include "graphic/font_style.h"

using namespace tex;

FontStyle tex::findClosestStyle(const FontStyle src) {
  static const FontStyle composedStyles[]{
    FontStyle::bfit, FontStyle::bfcal, FontStyle::bffrak,
    FontStyle::sfbf, FontStyle::sfit, FontStyle::sfbfit,
  };
  u32 similarity = 0;
  FontStyle target = FontStyle::none;
  for (FontStyle style: composedStyles) {
    const auto n = countSetBits(static_cast<u16>(src) & static_cast<u16>(style));
    if (n > similarity) {
      target = style;
      similarity = n;
    }
  }
  return target;
}

bool tex::isBold(FontStyle style) {
  return (static_cast<u16>(style) & static_cast<u16>(FontStyle::bf)) == 1;
}

bool tex::isItalic(FontStyle style) {
  return (static_cast<u16>(style) & static_cast<u16>(FontStyle::it)) == 1;
}

bool tex::isSerif(FontStyle style) {
  return (static_cast<u16>(style) & static_cast<u16>(FontStyle::sf)) == 1;
}

bool tex::isMono(FontStyle style) {
  return (static_cast<u16>(style) & static_cast<u16>(FontStyle::tt)) == 1;
}
