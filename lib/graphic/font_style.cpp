#include "graphic/font_style.h"

using namespace tinytex;

FontStyle tinytex::findClosestStyle(const FontStyle src) {
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

bool tinytex::isUnspecified(FontStyle style) {
  return style == FontStyle::none;
}

bool tinytex::isRoman(FontStyle style) {
  return style != FontStyle::invalid && (static_cast<u16>(style) & static_cast<u16>(FontStyle::rm)) != 0;
}

bool tinytex::isBold(FontStyle style) {
  return style != FontStyle::invalid && (static_cast<u16>(style) & static_cast<u16>(FontStyle::bf)) != 0;
}

bool tinytex::isItalic(FontStyle style) {
  return style != FontStyle::invalid && (static_cast<u16>(style) & static_cast<u16>(FontStyle::it)) != 0;
}

bool tinytex::isSansSerif(FontStyle style) {
  return style != FontStyle::invalid && (static_cast<u16>(style) & static_cast<u16>(FontStyle::sf)) != 0;
}

bool tinytex::isMono(FontStyle style) {
  return style != FontStyle::invalid && (static_cast<u16>(style) & static_cast<u16>(FontStyle::tt)) != 0;
}
