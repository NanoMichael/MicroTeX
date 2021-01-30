#include "utils/utils.h"

int tex::binIndexOf(
  int count,
  const std::function<int(int)>&& compare,
  bool returnClosest
) {
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
