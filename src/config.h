#ifndef LATEX_CONFIG_H
#define LATEX_CONFIG_H

// The clm data version
#define CLM_VER 1

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

// Flag for if compile samples
#if defined(CLATEX_OS_Unix_like_PC) || defined(CLATEX_OS_Windows) || defined(MEM_CHECK)
#   define CLATEX_USE_SAMPLES
#endif

// Disable log if not in debug mode
#ifndef CLATEX_DEBUG
#   undef HAVE_LOG
#endif

#ifdef _MSC_VER
#include "vcruntime.h"
#endif

#if (__cplusplus >= 201703L) || (defined(_MSC_VER) && defined(_HAS_CXX17) && _HAS_CXX17)
#define CLATEX_CXX17 1
#else
#define CLATEX_CXX17 0
#endif
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
