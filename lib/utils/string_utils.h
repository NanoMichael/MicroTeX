#ifndef STRING_UTILS_H_INCLUDED
#define STRING_UTILS_H_INCLUDED

#include <algorithm>
#include <cerrno>
#include <climits>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace microtex {

/**
 * Parse options from given string. The options are given in the form of
 * [key=value,key=value...].
 */
std::map<std::string, std::string> parseOption(const std::string& options);

/** Convert a value to string */
template <class T>
inline std::string toString(T val) {
  return std::to_string(val);
}

inline std::string toString(char val) {
  return {val};
}

template <class T>
inline void valueOf(const std::string& s, T& val) {
  char* endptr = nullptr;
  val = strtod(s.c_str(), &endptr);
}

inline bool str2int(const char* str, size_t len, int& res, int radix) {
  char* endptr = nullptr;
  errno = 0;

  const long val = strtol(str, &endptr, radix);

  if ((val == LONG_MAX || val == LONG_MIN) && errno == ERANGE) return false;

  res = static_cast<int>(val);
  return endptr == str + len;
}

/** Transform a string to lowercase */
inline std::string& toLower(std::string& src) {
  std::transform(src.begin(), src.end(), src.begin(), ::tolower);
  return src;
}

/** Ignore left side whitespace in a string */
inline std::string& ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char c) { return !isspace(c); }));
  return s;
}

/** Ignore right side whitespace in a string */
inline std::string& rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](char c) { return !isspace(c); }).base(), s.end());
  return s;
}

/** Ignore left and right side whitespace in a string */
inline std::string& trim(std::string& s) {
  return ltrim(rtrim(s));
}

inline bool startsWith(const std::string& str, const std::string& cmp) {
  return str.find(cmp) == 0;
}

inline bool endsWith(const std::string& str, const std::string& cmp) {
  return str.rfind(cmp) == (str.length() - cmp.length());
}

/** Split string with delimiter */
class StrTokenizer {
private:
  std::string _str;
  std::string _del;
  bool _ret;
  int _pos;

public:
  StrTokenizer(std::string str);

  StrTokenizer(std::string str, std::string del, bool ret = false);

  int count();

  std::string next();
};

/**
 * Returns a replacement string for the given one that has all backslashes
 * and dollar signs escaped
 */
inline std::string& quoteReplace(const std::string& src, std::string& out) {
  for (size_t i = 0; i < src.length(); i++) {
    char c = src[i];
    if (c == '\\' || c == '$') out.append(1, '\\');
    out.append(1, c);
  }
  return out;
}

/** Replace string with specified string in the first */
inline std::string& replaceFirst(std::string& src, const std::string& from, const std::string& to) {
  size_t start = src.find(from);
  if (start == std::string::npos) return src;
  src.replace(start, from.length(), to);
  return src;
}

inline std::string& replaceAll(std::string& src, const std::string& from, const std::string& to) {
  if (from.empty()) return src;
  size_t start = 0;
  while ((start = src.find(from, start)) != std::string::npos) {
    src.replace(start, from.length(), to);
    start += to.length();
  }
  return src;
}

}  // namespace microtex

#endif
