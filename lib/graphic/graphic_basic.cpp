#include "graphic/graphic_basic.h"
#include <sstream>

tinytex::color tinytex::decodeColor(const std::string& s) {
  if (s[0] == '#') {
    const std::string x = s.substr(1);
    std::stringstream ss;
    ss << std::hex << x;
    color c;
    ss >> c;
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
