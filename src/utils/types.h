#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

#include <cinttypes>

#define __no_copy_assign(T) \
  T() = default;            \
  T(const T&) = delete;     \
  void operator=(const T&) = delete

namespace tex {

using int16 = std::int16_t;
using uint16 = std::uint16_t;

}  // namespace tex

#endif
