#ifndef UNI_FONT_INCLUDED
#define UNI_FONT_INCLUDED

#include <map>

#include "otf/otf.h"

namespace tex {

struct OTFFontInfo {
  const std::string fontFile;
};

class UniFont {
private:
  static std::map<std::string, OTFFont*> _fonts;
};

};  // namespace tex

#endif
