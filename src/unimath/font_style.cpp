#include "font_style.h"

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
