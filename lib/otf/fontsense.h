#include "microtexconfig.h"

#ifdef HAVE_AUTO_FONT_FIND
#ifndef MICROTEX_FONTSENSE_H
#define MICROTEX_FONTSENSE_H

#include "unimath/font_meta.h"
#include <optional>
#include <string>

namespace microtex {

/** Find font resources auto. Return the first found math font name. */
std::optional<FontMeta> fontsenseLookup();

}

#endif // MICROTEX_FONTSENSE_H
#endif // HAVE_AUTO_FONT_FIND
