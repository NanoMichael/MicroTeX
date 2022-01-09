#ifndef LATEX_CONFIG_H
#define LATEX_CONFIG_H

// if have glyph render path
#if GLYPH_RENDER_TYPE == 0 || GLYPH_RENDER_TYPE == 1
#   define HAVE_GLYPH_RENDER_PATH
#endif

// The clm data major version
#define CLM_VER_MAJOR 2

// The clm data minor version, must be 1 or 2
#ifdef HAVE_GLYPH_RENDER_PATH
#   define CLM_VER_MINOR 2
#else
#   define CLM_VER_MINOR 1
#endif

// If clm supports glyph path
#define CLM_SUPPORT_GLYPH_PATH(minorVersion) ((minorVersion) == 2)

// Flag for debug
#if defined(_DEBUG) || defined(DEBUG) || defined(__DEBUG__) || !defined(NDEBUG)
#   ifndef CLATEX_DEBUG
#       define CLATEX_DEBUG
#   endif
#endif

// Check platforms
#if defined(__linux__)
#   ifdef __ANDROID__
#       define CLATEX_OS_Android
#   else
#       define CLATEX_OS_Linux
#   endif
#elif defined(__APPLE__) && defined(__MACH__)
#   define CLATEX_OS_Mac
#elif defined(_WIN32)
#   define CLATEX_OS_Windows
#endif
// Other platforms...

#if defined(CLATEX_OS_Linux) || defined(CLATEX_OS_Mac)
#   define CLATEX_OS_Unix_like_PC
#endif

// Disable log if not in debug mode
#ifndef CLATEX_DEBUG
#   undef HAVE_LOG
#endif

#ifdef _MSC_VER
#include "vcruntime.h"
#endif

// if has c++17
#if (__cplusplus >= 201703L) || (defined(_MSC_VER) && defined(_HAS_CXX17) && _HAS_CXX17)
#define CLATEX_CXX17 1
#else
#define CLATEX_CXX17 0
#endif

// export
#ifdef _MSC_VER
#if defined(CLATEXMATH_LIBRARY)
#define CLATEXMATH_EXPORT __declspec(dllexport)
#else
#define CLATEXMATH_EXPORT __declspec(dllimport)
#endif
#else
#define CLATEXMATH_EXPORT
#endif

#endif  // LATEX_CONFIG_H
