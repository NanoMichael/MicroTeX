#include "microtexconfig.h"

#ifdef HAVE_AUTO_FONT_FIND
#ifndef MICROTEX_FONTSENSE_H
#define MICROTEX_FONTSENSE_H

#include <optional>
#include <string>

#include "unimath/font_meta.h"

namespace microtex {

/** Find font resources auto. Return the first found math font name. */
std::optional<FontMeta> fontsenseLookup();

}  // namespace microtex

#endif  // MICROTEX_FONTSENSE_H
#endif  // HAVE_AUTO_FONT_FIND
