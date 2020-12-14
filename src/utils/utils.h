#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <cinttypes>
#include <functional>

#define __no_copy_assign(T) \
  T(const T&) = delete;     \
  void operator=(const T&) = delete

namespace tex {

using int16 = std::int16_t;
using uint16 = std::uint16_t;
using int32 = std::int32_t;

/** 
 * Binary search for index of the given target.
 * 
 * @param count total count of the container
 * @param compare function to compare target value between the value at given index
 * @param returnClosest if not found, return the closest index if is true or -1 otherwise,
 *  default is false
 */
static int binSearchIndex(
    int count,
    const std::function<int(int)>&& compare,
    bool returnClosest = false) {
  if (count == 0) return -1;
  int l = 0, h = count - 1;
  while (l <= h) {
    const int m = l + ((h - l) >> 1);
    const int cmp = compare(m);
    if (cmp == 0) return m;
    cmp < 0 ? h = m - 1 : l = m + 1;
  }
  return returnClosest ? std::max(0, l - 1) : -1;
}

}  // namespace tex

#endif
