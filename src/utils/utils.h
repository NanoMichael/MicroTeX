#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <cinttypes>
#include <functional>

#define __no_copy_assign(T) \
  T(const T&) = delete;     \
  void operator=(const T&) = delete

namespace tex {

using uint8 = std::uint8_t;
using int16 = std::int16_t;
using uint16 = std::uint16_t;
using int32 = std::int32_t;
using uint32 = std::uint32_t;

/** 
 * Binary search for index of the given target.
 * 
 * @param count total count of the container
 * @param compare function to compare between the target value and the value at given index
 * @param returnClosest if return the closest index while not found, default is false
 *
 * @return the index which the target value found at, or the closest index if returnClosest is true,
 * or -1 otherwise
 */
int binSearchIndex(
  int count,
  const std::function<int(int)>&& compare,
  bool returnClosest = false  //
);

}  // namespace tex

#endif
