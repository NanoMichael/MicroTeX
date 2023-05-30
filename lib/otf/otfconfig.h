#ifndef MICROTEX_OTFCONFIG_H
#define MICROTEX_OTFCONFIG_H

#define GLYPH_RENDER_TYPE_BOTH 0
#define GLYPH_RENDER_TYPE_PATH 1
#define GLYPH_RENDER_TYPE_TYPEFACE 2

// clang-format off
// if have glyph render path
#if GLYPH_RENDER_TYPE == GLYPH_RENDER_TYPE_BOTH || GLYPH_RENDER_TYPE == GLYPH_RENDER_TYPE_PATH
#   define HAVE_GLYPH_RENDER_PATH
#endif

#if GLYPH_RENDER_TYPE == GLYPH_RENDER_TYPE_BOTH || GLYPH_RENDER_TYPE == GLYPH_RENDER_TYPE_TYPEFACE
#   define HAVE_GLYPH_RENDER_TYPEFACE
#endif

// The clm data major version
#define CLM_VER_MAJOR 5

// The clm data minor version, must be 1 or 2
#ifdef HAVE_GLYPH_RENDER_PATH
#   define CLM_VER_MINOR 2
#else
#   define CLM_VER_MINOR 1
#endif

// If clm supports glyph path
#define CLM_SUPPORT_GLYPH_PATH(minorVersion) ((minorVersion) == 2)
// clang-format on

#endif  // MICROTEX_OTFCONFIG_H
