#ifndef STRING_UTILS_H_INCLUDED
#define STRING_UTILS_H_INCLUDED

#include <algorithm>
#include <cerrno>
#include <climits>
#include <sstream>
#include <string>
#include <functional>

using namespace std;

namespace tex {

/** Convert a value to string */
template <class T>
inline string tostring(T val) {
  ostringstream os;
  os << val;
  return os.str();
}

/** Convert a value to wide string */
template <class T>
inline wstring towstring(T val) {
  wostringstream os;
  os << val;
  return os.str();
}

template <class T>
inline void valueof(const string& s, T& val) {
  stringstream ss;
  ss << s;
  ss >> val;
}

template <class T>
inline void valueof(const wstring& s, T& val) {
  wstringstream ss;
  ss << s;
  ss >> val;
}

inline bool str2int(const string& str, int& res, int radix) {
  char* endptr = nullptr;
  errno = 0;

  const long val = strtol(str.c_str(), &endptr, radix);

  if ((val == LONG_MAX || val == LONG_MIN) && errno == ERANGE)
    return false;

  res = static_cast<int>(val);
  return endptr == str.c_str() + str.size();
}

/** Transform a string to lowercase */
inline string& tolower(string& src) {
  transform(src.begin(), src.end(), src.begin(), ::tolower);
  return src;
}

inline wstring& tolower(wstring& src) {
  transform(src.begin(), src.end(), src.begin(), ::tolower);
  return src;
}

/** Ignore left side whitespace in a string */
inline string& ltrim(string& s) {
#if (__cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17)
  s.erase(s.begin(), find_if(s.begin(), s.end(), not_fn(isspace)));
#else
  s.erase(s.begin(), find_if(s.begin(), s.end(), not1(cref(isspace))));
#endif
  return s;
}

/** Ignore right side whitespace in a string */
inline string& rtrim(string& s) {
#if (__cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17)
  s.erase(find_if(s.rbegin(), s.rend(), not_fn(isspace)).base(), s.end());
#else
  s.erase(find_if(s.rbegin(), s.rend(), not1(cref(isspace))).base(), s.end());
#endif
  return s;
}

/** Ignore left and right side whitespace in a string */
inline string& trim(string& s) {
  return ltrim(rtrim(s));
}

/** Split string with specified delimeter */
inline void split(const string& str, char del, vector<string>& res) {
  stringstream ss(str);
  string tok;
  while (getline(ss, tok, del)) res.push_back(tok);
}

inline bool startswith(const string& str, const string& cmp) {
  return str.find(cmp) == 0;
}

inline bool endswith(const string& str, const string& cmp) {
  return str.rfind(cmp) == (str.length() - cmp.length());
}

inline bool startswith(const wstring& str, const wstring& cmp) {
  return str.find(cmp) == 0;
}

inline bool endswith(const wstring& str, const wstring& cmp) {
  return str.rfind(cmp) == (str.length() - cmp.length());
}

/** Split string with delimiter */
class strtokenizer {
private:
  string _str;
  string _del;
  bool _ret;
  int _pos;

public:
  strtokenizer(const string& str) {
    _str = str;
    _del = " \t\n\r\f";
    _ret = false;
    _pos = 0;
  }

  strtokenizer(const string& str, const string& del, bool ret = false) {
    _str = str;
    _del = del;
    _ret = ret;
    _pos = 0;
  }

  int count_tokens() {
    int c = 0;
    bool in = false;
    for (int i = _pos, len = _str.length(); i < len; i++) {
      if (_del.find(_str[i]) != string::npos) {
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

  string next_token() {
    int i = _pos;
    int len = _str.length();

    if (i < len) {
      if (_ret) {
        if (_del.find(_str[_pos]) != string::npos)
          return string({_str[_pos++]});
        for (_pos++; _pos < len; _pos++)
          if (_del.find(_str[_pos]) != string::npos)
            return _str.substr(i, _pos - i);
        return _str.substr(i);
      }

      while (i < len && _del.find(_str[i]) != string::npos) i++;

      _pos = i;
      if (i < len) {
        for (_pos++; _pos < len; _pos++)
          if (_del.find(_str[_pos]) != string::npos)
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
inline string& quotereplace(const string& src, string& out) {
  for (size_t i = 0; i < src.length(); i++) {
    char c = src[i];
    if (c == '\\' || c == '$') out.append(1, '\\');
    out.append(1, c);
  }
  return out;
}

inline wstring& quotereplace(const wstring& src, wstring& out) {
  for (size_t i = 0; i < src.length(); i++) {
    wchar_t c = src[i];
    if (c == L'\\' || c == L'$') out.append(1, L'\\');
    out.append(1, c);
  }
  return out;
}

/** Replace string with specified string in the first */
inline string& replacefirst(string& src, const string& from, const string& to) {
  size_t start = src.find(from);
  if (start == string::npos) return src;
  src.replace(start, from.length(), to);
  return src;
}

inline string& replaceall(string& src, const string& from, const string& to) {
  if (from.empty()) return src;
  size_t start = 0;
  while ((start = src.find(from, start)) != string::npos) {
    src.replace(start, from.length(), to);
    start += to.length();
  }
  return src;
}

inline wstring& replacefirst(wstring& src, const wstring& from, const wstring& to) {
  size_t start = src.find(from);
  if (start == wstring::npos) return src;
  src.replace(start, from.length(), to);
  return src;
}

inline wstring& replaceall(wstring& src, const wstring& from, const wstring& to) {
  if (from.empty()) return src;
  size_t start = 0;
  while ((start = src.find(from, start)) != wstring::npos) {
    src.replace(start, from.length(), to);
    start += to.length();
  }
  return src;
}

}  // namespace tex

#endif
