#include "graphic/graphic_basic.h"
#include <sstream>

tex::color tex::decodeColor(const std::string& s) {
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
