#include "utils/utils.h"

const std::locale& microtex::defaultLocale() {
#if !defined(__WIN32__)
  static const std::locale locale("en_US.UTF-8");
#else
  static const std::locale locale("C");
#endif
  return locale;
}

bool microtex::isUnicodeLower(c32 code) {
  // the type-cast is necessary, or a std::bad_cast will be thrown,
  // because std::toupper is a template function
  return std::islower((wchar_t)code, defaultLocale());
}

bool microtex::isUnicodeDigit(c32 code) {
  return std::isdigit((wchar_t)code, defaultLocale());
}

microtex::c32 microtex::toUnicodeUpper(c32 code) {
  return std::toupper((wchar_t)code, defaultLocale());
}

microtex::c32 microtex::toUnicodeLower(c32 code) {
  return std::tolower((wchar_t)code, defaultLocale());
}

int microtex::binIndexOf(int count, const std::function<int(int)>& compare, bool returnClosest) {
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
