#ifndef STRING_UTILS_H_INCLUDED
#define STRING_UTILS_H_INCLUDED

#include "config.h"
#include <algorithm>
#include <cerrno>
#include <climits>
#include <sstream>
#include <string>
#include <functional>

namespace tex {

/** Convert a value to string */
template <class T>
inline std::string tostring(T val) {
  std::ostringstream os;
  os << val;
  return os.str();
}

/** Convert a value to wide string */
template <class T>
inline std::wstring towstring(T val) {
  std::wostringstream os;
  os << val;
  return os.str();
}

template <class T>
inline void valueof(const std::string& s, T& val) {
  std::stringstream ss;
  ss << s;
  ss >> val;
}

template <class T>
inline void valueof(const std::wstring& s, T& val) {
  std::wstringstream ss;
  ss << s;
  ss >> val;
}

inline bool str2int(const std::string& str, int& res, int radix) {
  char* endptr = nullptr;
  errno = 0;

  const long val = strtol(str.c_str(), &endptr, radix);

  if ((val == LONG_MAX || val == LONG_MIN) && errno == ERANGE)
    return false;

  res = static_cast<int>(val);
  return endptr == str.c_str() + str.size();
}

/** Transform a string to lowercase */
inline std::string& tolower(std::string& src) {
  std::transform(src.begin(), src.end(), src.begin(), ::tolower);
  return src;
}

inline std::wstring& tolower(std::wstring& src) {
  std::transform(src.begin(), src.end(), src.begin(), ::tolower);
  return src;
}

/** Ignore left side whitespace in a string */
inline std::string& ltrim(std::string& s) {
#if CLATEX_CXX17
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not_fn<int(int)>(isspace)));
#else
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::cref<int(int)>(isspace))));
#endif
  return s;
}

/** Ignore right side whitespace in a string */
inline std::string& rtrim(std::string& s) {
#if CLATEX_CXX17
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not_fn<int(int)>(isspace)).base(), s.end());
#else
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::cref<int(int)>(isspace))).base(), s.end());
#endif
  return s;
}

/** Ignore left and right side whitespace in a string */
inline std::string& trim(std::string& s) {
  return ltrim(rtrim(s));
}

/** Split string with specified delimeter */
inline void split(const std::string& str, char del, std::vector<std::string>& res) {
  std::stringstream ss(str);
  std::string tok;
  while (std::getline(ss, tok, del)) res.push_back(tok);
}

inline bool startswith(const std::string& str, const std::string& cmp) {
  return str.find(cmp) == 0;
}

inline bool endswith(const std::string& str, const std::string& cmp) {
  return str.rfind(cmp) == (str.length() - cmp.length());
}

inline bool startswith(const std::wstring& str, const std::wstring& cmp) {
  return str.find(cmp) == 0;
}

inline bool endswith(const std::wstring& str, const std::wstring& cmp) {
  return str.rfind(cmp) == (str.length() - cmp.length());
}

/** Split string with delimiter */
class strtokenizer {
private:
  std::string _str;
  std::string _del;
  bool _ret;
  int _pos;

public:
  strtokenizer(const std::string& str) {
    _str = str;
    _del = " \t\n\r\f";
    _ret = false;
    _pos = 0;
  }

  strtokenizer(const std::string& str, const std::string& del, bool ret = false) {
    _str = str;
    _del = del;
    _ret = ret;
    _pos = 0;
  }

  int count_tokens() {
    int c = 0;
    bool in = false;
    for (int i = _pos, len = _str.length(); i < len; i++) {
      if (_del.find(_str[i]) != std::string::npos) {
        if (_ret) c++;
        if (in) {
          c++;
          in = false;
        }
      } else {
        in = true;
      }
    }
    if (in) c++;
    return c;
  }

  std::string next_token() {
    int i = _pos;
    int len = _str.length();

    if (i < len) {
      if (_ret) {
        if (_del.find(_str[_pos]) != std::string::npos)
          return std::string({_str[_pos++]});
        for (_pos++; _pos < len; _pos++)
          if (_del.find(_str[_pos]) != std::string::npos)
            return _str.substr(i, _pos - i);
        return _str.substr(i);
      }

      while (i < len && _del.find(_str[i]) != std::string::npos) i++;

      _pos = i;
      if (i < len) {
        for (_pos++; _pos < len; _pos++)
          if (_del.find(_str[_pos]) != std::string::npos)
            return _str.substr(i, _pos - i);
        return _str.substr(i);
      }
    }
    return "";
  }
};

/**
 * Returns a replacement string for the given one that has all backslashes
 * and dollar signs escaped
 */
inline std::string& quotereplace(const std::string& src, std::string& out) {
  for (size_t i = 0; i < src.length(); i++) {
    char c = src[i];
    if (c == '\\' || c == '$') out.append(1, '\\');
    out.append(1, c);
  }
  return out;
}

inline std::wstring& quotereplace(const std::wstring& src, std::wstring& out) {
  for (size_t i = 0; i < src.length(); i++) {
    wchar_t c = src[i];
    if (c == L'\\' || c == L'$') out.append(1, L'\\');
    out.append(1, c);
  }
  return out;
}

/** Replace string with specified string in the first */
inline std::string& replacefirst(std::string& src, const std::string& from, const std::string& to) {
  size_t start = src.find(from);
  if (start == std::string::npos) return src;
  src.replace(start, from.length(), to);
  return src;
}

inline std::string& replaceall(std::string& src, const std::string& from, const std::string& to) {
  if (from.empty()) return src;
  size_t start = 0;
  while ((start = src.find(from, start)) != std::string::npos) {
    src.replace(start, from.length(), to);
    start += to.length();
  }
  return src;
}

inline std::wstring& replacefirst(std::wstring& src, const std::wstring& from, const std::wstring& to) {
  size_t start = src.find(from);
  if (start == std::wstring::npos) return src;
  src.replace(start, from.length(), to);
  return src;
}

inline std::wstring& replaceall(std::wstring& src, const std::wstring& from, const std::wstring& to) {
  if (from.empty()) return src;
  size_t start = 0;
  while ((start = src.find(from, start)) != std::wstring::npos) {
    src.replace(start, from.length(), to);
    start += to.length();
  }
  return src;
}

}  // namespace tex

#endif
