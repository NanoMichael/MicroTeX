#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <cinttypes>
#include <functional>
#include <memory>
#include <vector>

#define no_copy_assign(T) \
  T(const T&) = delete;   \
  void operator=(const T&) = delete

namespace tex {

using i8 = std::int8_t;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using c32 = char32_t;

/** Type alias shared_ptr<T> to sptr<T> */
template<typename T>
using sptr = std::shared_ptr<T>;

template<typename T, typename... Args>
inline sptr<T> sptrOf(Args&& ... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

/** Find the position of a value in the vector, return -1 if not found */
template<class T>
inline int indexOf(const std::vector<T>& v, const T& x) {
  auto it = find(v.begin(), v.end(), x);
  int i = std::distance(v.begin(), it);
  return (i >= v.size() ? -1 : i);
}

/** 
 * Binary search for index of the given target in a container.
 * 
 * @param count total count of elements in the container
 * @param compare function to compare between the target value and the value at given index
 * @param returnClosest if return the closest index while not found, default is false
 *
 * @return the index which the target value found at, or the closest index if returnClosest is true,
 * or -1 otherwise
 */
int binIndexOf(
  int count,
  const std::function<int(int)>&& compare,
  bool returnClosest = false
);

}  // namespace tex

#endif
