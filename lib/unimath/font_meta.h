#ifndef TINYTEX_FONT_META_H
#define TINYTEX_FONT_META_H

#include "tinytexexport.h"
#include <graphic/font_style.h>
#include <string>

namespace microtex {

struct TINYTEX_EXPORT FontMeta {
  std::string family;
  std::string name;
  FontStyle style;
  bool isMathFont;

  inline bool isValid() const {
    return !family.empty() && !name.empty();
  }
};

}

#endif //TINYTEX_FONT_META_H
