#ifndef MICROTEX_FONT_META_H
#define MICROTEX_FONT_META_H

#include <string>

#include "graphic/font_style.h"
#include "microtexexport.h"

namespace microtex {

struct MICROTEX_EXPORT FontMeta {
  std::string family;
  std::string name;
  FontStyle style;
  bool isMathFont;

  inline bool isValid() const { return !family.empty() && !name.empty(); }
};

}  // namespace microtex

#endif  // MICROTEX_FONT_META_H
