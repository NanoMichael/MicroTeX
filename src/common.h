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

#include "utils/exceptions.h"
#include "utils/log.h"
#include "utils/nums.h"
#include "utils/string_utils.h"
#include "utils/utf.h"
#include "utils/utils.h"

namespace tex {

/** The root directory of the "TeX resources" (defined in latex.cpp) */
extern std::string RES_BASE;

/** Return the real name of the function, class or struct name. */
#ifdef HAVE_LOG
#ifdef __GNUC__

inline std::string demangle_name(const char* name) {
  int status = -4;
  char* res = abi::__cxa_demangle(name, nullptr, nullptr, &status);
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
}  // namespace tex

#endif  // COMMON_H_INCLUDED
