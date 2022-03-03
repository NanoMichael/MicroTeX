#ifndef TINYTEX_OTFCONFIG_H
#define TINYTEX_OTFCONFIG_H

// if have glyph render path
#if GLYPH_RENDER_TYPE == 0 || GLYPH_RENDER_TYPE == 1
#   define HAVE_GLYPH_RENDER_PATH
#endif

// The clm data major version
#define CLM_VER_MAJOR 4

// The clm data minor version, must be 1 or 2
#ifdef HAVE_GLYPH_RENDER_PATH
#   define CLM_VER_MINOR 2
#else
#   define CLM_VER_MINOR 1
#endif

// If clm supports glyph path
#define CLM_SUPPORT_GLYPH_PATH(minorVersion) ((minorVersion) == 2)

#endif //TINYTEX_OTFCONFIG_H
