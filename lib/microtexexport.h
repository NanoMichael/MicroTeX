#ifndef MICROTEX_MICROTEXEXPORT_H
#define MICROTEX_MICROTEXEXPORT_H

#ifdef _MSC_VER
#include "vcruntime.h"
#endif

// msvc
#ifdef _MSC_VER
#   if defined(MICROTEX_LIBRARY)
#       define MICROTEX_EXPORT __declspec(dllexport)
#   else
#       define MICROTEX_EXPORT __declspec(dllimport)
#   endif
// gnuc and clang
#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
#   define MICROTEX_EXPORT __attribute((visibility("default")))
// otherwise...
#else
#   define MICROTEX_EXPORT
#endif

#endif //MICROTEX_MICROTEXEXPORT_H
