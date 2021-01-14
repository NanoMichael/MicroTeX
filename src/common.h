#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "config.h"

#if defined(HAVE_LOG) && defined(__GNUC__)
#include <cxxabi.h>
#endif

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "utils/constants.h"
#include "utils/exceptions.h"
#include "utils/log.h"
#include "utils/nums.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

/** OUT parameter, declaring that the function may has side effect on this parameter. */
#define _out_
/** IN parameter, declaring that the function will never has side effect on this parameter. */
#define _in_

namespace tex {

/** The root directory of the "TeX resources" (defined in latex.cpp) */
extern std::string RES_BASE;

/** Type alias shared_ptr<T> to sptr<T> */
template <typename T>
using sptr = std::shared_ptr<T>;

/** Return the real name of the function, class or struct name. */
#ifdef HAVE_LOG
#ifdef __GNUC__
inline std::string demangle_name(const char* name) {
  int status = -4;
  char* res = abi::__cxa_demangle(name, 0, 0, &status);
  const char* const real_name = status == 0 ? res : name;
  std::string res_str(real_name);
  if (res != nullptr) {
    free(res);
  }
  return res_str;
}
#else
inline std::string demangle_name(const char* name) {
  return name;
}
#endif  // __GNUC__
#endif  // HAVE_LOG

/** Find the position of a value in the vector, return -1 if not found */
template <class T>
inline int indexOf(const std::vector<T>& v, const T& x) {
  auto it = find(v.begin(), v.end(), x);
  int i = std::distance(v.begin(), it);
  return (i >= v.size() ? -1 : i);
}

}  // namespace tex

#endif  // COMMON_H_INCLUDED
