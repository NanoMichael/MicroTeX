#ifndef MICROTEX_FONT_META_H
#define MICROTEX_FONT_META_H

#include "microtexexport.h"
#include "graphic/font_style.h"
#include <string>

namespace microtex {

struct MICROTEX_EXPORT FontMeta {
  std::string family;
  std::string name;
  FontStyle style;
  bool isMathFont;

  inline bool isValid() const {
    return !family.empty() && !name.empty();
  }
};

}

#endif //MICROTEX_FONT_META_H
