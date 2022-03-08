#include "graphic/graphic_basic.h"
#include "atom/atom_basic.h"
#include "utils/string_utils.h"

tinytex::color tinytex::decodeColor(const std::string& s) {
  if (s[0] == '#') {
    const std::string x = s.substr(1);
    color c = black;
    auto success = str2int(s.c_str() + 1, s.length() - 1, reinterpret_cast<int&>(c), 16);
    if (!success) {
      return black;
    }
    if (s.size() == 7) {
      // set alpha value
      c |= 0xff000000;
    } else if (s.size() != 9) {
      return black;
    }
    return c;
  }
  return black;
}

tinytex::color tinytex::getColor(const std::string& name) {
  return ColorAtom::getColor(name);
}

bool tinytex::Point::operator==(const Point& other) const {
  return x == other.x && y == other.y;
}

bool tinytex::Rect::operator==(const Rect& other) const {
  return x == other.x && y == other.y && w == other.w && h == other.h;
}

bool tinytex::Stroke::operator==(const Stroke& other) const {
  return lineWidth == other.lineWidth
         && miterLimit == other.miterLimit
         && cap == other.cap
         && join == other.join;
}
