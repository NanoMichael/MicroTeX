#include "macro/macro_colors.h"

#include "utils/exceptions.h"
#include "utils/string_utils.h"

namespace microtex {

macro(definecolor) {
  color c = TRANSPARENT;
  const auto& cs = args[3];
  if (args[2] == "gray") {
    float f = 0;
    valueOf(args[3], f);
    c = rgb(f, f, f);
  } else if (args[2] == "rgb") {
    StrTokenizer stok(cs, ":,");
    if (stok.count() != 3) throw ex_parse("RGB color must have three components!");
    float r, g, b;
    std::string R = stok.next(), G = stok.next(), B = stok.next();
    valueOf(trim(R), r);
    valueOf(trim(G), g);
    valueOf(trim(B), b);
    c = rgb(r, g, b);
  } else if (args[2] == "cmyk") {
    StrTokenizer stok(cs, ":,");
    if (stok.count() != 4) throw ex_parse("CMYK color must have four components!");
    float cmyk[4];
    for (float& i : cmyk) {
      std::string X = stok.next();
      valueOf(trim(X), i);
    }
    float k = 1 - cmyk[3];
    c = rgb(k * (1 - cmyk[0]), k * (1 - cmyk[1]), k * (1 - cmyk[2]));
  } else {
    throw ex_parse("Invalid color model!");
  }

  ColorAtom::defineColor(args[1], c);
  return nullptr;
}

}  // namespace microtex
