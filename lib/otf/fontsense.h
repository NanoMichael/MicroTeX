#include "tinytexconfig.h"

#ifdef HAVE_AUTO_FONT_FIND
#ifndef TINYTEX_FONTSENSE_H
#define TINYTEX_FONTSENSE_H

#include "unimath/font_meta.h"
#include <optional>
#include <string>

namespace tinytex {

/** Find font resources auto. Return the first found math font name. */
std::optional<FontMeta> fontsenseLookup();

}

#endif // TINYTEX_FONTSENSE_H
#endif // HAVE_AUTO_FONT_FIND
