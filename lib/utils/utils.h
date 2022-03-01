#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include "types.h"

#include <functional>
#include <vector>
#include <locale>
#include <sstream>
#include <algorithm>
#include <map>

#define no_copy_assign(T) \
  T(const T&) = delete;   \
  void operator=(const T&) = delete

namespace tinytex {

template<typename T, typename... Args>
inline sptr<T> sptrOf(Args&& ... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
inline uptr<T> uptrOf(Args&& ... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename K, typename V>
inline std::vector<K> keys(const std::map<K, V>& map) {
  std::vector<K> v;
  for (auto& it : map) {
    v.push_back(it.first);
  }
  return v;
}

/** Template version to find the max value of the given list */
template<typename T, typename ... Args>
inline T maxOf(Args&& ... args) {
  return std::max({args...});
}

/** Convert args to string */
template<typename... Args>
inline std::string sstr(Args&& ... args) {
  std::ostringstream str;
  (str <<  ... << args);
  return str.str();
}

/** Get number of set bits in binary representation of the given number */
template<typename T>
u32 countSetBits(T n) {
  u32 cnt = 0;
  while (n) {
    n &= (n - 1);
    cnt++;
  }
  return cnt;
}

/** The default locale */
const std::locale& defaultLocale();

/** Test if a Unicode code point is lower case */
bool isUnicodeLower(c32 code);

/** Test if a Unicode code point is digit */
bool isUnicodeDigit(c32 code);

/** Convert given Unicode code point to upper case */
c32 toUnicodeUpper(c32 code);

/** Convert given Unicode code point to lower case */
c32 toUnicodeLower(c32 code);

/** 
 * Binary-search for the index of the given target in a container. The items in the container must be
 * sorted.
 * 
 * @param count total count of elements in the container
 * @param compare function to compare between the target value and the value at the given index
 * @param returnClosest whether return the closest index while not found, default is false
 *
 * @return the index which the target value found at, or the closest index if returnClosest is true,
 * or -1 otherwise
 */
int binIndexOf(
  int count,
  const std::function<int(int)>& compare,
  bool returnClosest = false
);

}  // namespace tinytex

#endif
