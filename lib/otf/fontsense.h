#include "config.h"

#ifdef HAVE_AUTO_FONT_FIND
#ifndef TINYTEX_FONTSENSE_H
#define TINYTEX_FONTSENSE_H

#include <optional>
#include <string>

namespace tinytex {

/** Find font resources auto. Return the first found math font name. */
std::optional<const std::string> fontsenseLookup();

}

#endif // TINYTEX_FONTSENSE_H
#endif // HAVE_AUTO_FONT_FIND
