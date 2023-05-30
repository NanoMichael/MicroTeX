#include "graphic/font_style.h"

#include "utils/utils.h"

using namespace microtex;

FontStyle microtex::findClosestStyle(const FontStyle src) {
  u16 x = static_cast<u16>(src);
  // roman or none
  if (x == 0 || x == 1) return src;
  // the last bit is 1, that means added a roman style, remove it
  if ((x & 0x1) == 1) x &= 0xfffe;
  // single bit, that means a single style
  if ((x & (x - 1)) == 0) return static_cast<FontStyle>(x);
  // find the max match from composed styles
  static const FontStyle composedStyles[]{
    FontStyle::bfit,
    FontStyle::bfcal,
    FontStyle::bffrak,
    FontStyle::sfbf,
    FontStyle::sfit,
    FontStyle::sfbfit,
  };
  u32 similarity = 0;
  FontStyle target = FontStyle::none;
  for (FontStyle style : composedStyles) {
    const auto n = countSetBits(x & static_cast<u16>(style));
    if (n > similarity) {
      target = style;
      similarity = n;
    }
  }
  return target;
}

bool microtex::isUnspecified(FontStyle style) {
  return style == FontStyle::none;
}

bool microtex::isRoman(FontStyle style) {
  return style != FontStyle::invalid
         && (static_cast<u16>(style) & static_cast<u16>(FontStyle::rm)) != 0;
}

bool microtex::isBold(FontStyle style) {
  return style != FontStyle::invalid
         && (static_cast<u16>(style) & static_cast<u16>(FontStyle::bf)) != 0;
}

bool microtex::isItalic(FontStyle style) {
  return style != FontStyle::invalid
         && (static_cast<u16>(style) & static_cast<u16>(FontStyle::it)) != 0;
}

bool microtex::isSansSerif(FontStyle style) {
  return style != FontStyle::invalid
         && (static_cast<u16>(style) & static_cast<u16>(FontStyle::sf)) != 0;
}

bool microtex::isMono(FontStyle style) {
  return style != FontStyle::invalid
         && (static_cast<u16>(style) & static_cast<u16>(FontStyle::tt)) != 0;
}
