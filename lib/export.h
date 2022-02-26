#ifndef TINYTEX_EXPORT_H
#define TINYTEX_EXPORT_H

#ifdef _MSC_VER
#include "vcruntime.h"
#endif

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

#endif //TINYTEX_EXPORT_H
