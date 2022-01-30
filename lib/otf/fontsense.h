#include <map>
#include <optional>
#include <string>

#include "otf/otf.h"
#include "unimath/font_src.h"

namespace tinytex {
// Map<FileStem, <OTF File, CLM File>>
typedef std::map<std::string, std::pair<char*, char*>> font_paths_t;

// Map<FontFamily, Map<Name, Fonts>>
typedef std::map<std::string, std::map<std::string, FontSrcSense>> font_families_t;

font_paths_t get_font_paths();
void font_paths_free(font_paths_t font_paths);

std::optional<const std::string> fontsense_lookup();
}
