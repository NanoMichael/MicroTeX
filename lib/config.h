#ifndef TINYTEX_CONFIG_H
#define TINYTEX_CONFIG_H

#ifdef _MSC_VER
#include "vcruntime.h"
#endif

// export
// msvc
#ifdef _MSC_VER
#   if defined(TINYTEX_LIBRARY)
#       define TINYTEX_EXPORT __declspec(dllexport)
#   else
#       define TINYTEX_EXPORT __declspec(dllimport)
#   endif
// gnuc and clang
#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
#   define TINYTEX_EXPORT __attribute((visibility("default")))
// otherwise...
#else
#   define TINYTEX_EXPORT
#endif

#endif  // TINYTEX_CONFIG_H
