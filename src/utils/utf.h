#ifndef UTF_H_INCLUDED
#define UTF_H_INCLUDED

#include <string>

using namespace std;

namespace tex {

/** Convert unicode wide string to UTF-8 encoded string. */
inline string& wide2utf8(const wchar_t* in, string& out) {
  unsigned int codepoint = 0;
  for (; *in != 0; ++in) {
    if (*in >= 0xd800 && *in <= 0xdbff) {
      codepoint = ((*in - 0xd800) << 10) + 0x10000;
    } else {
      if (*in >= 0xdc00 && *in <= 0xdfff) {
        codepoint |= *in - 0xdc00;
      } else {
        codepoint = *in;
      }

      if (codepoint <= 0x7f) {
        out.append(1, static_cast<char>(codepoint));
      } else if (codepoint <= 0x7ff) {
        out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
      } else if (codepoint <= 0xffff) {
        out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
      } else {
        out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
        out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
      }
      codepoint = 0;
    }
  }
  return out;
}

inline string wide2utf8(const wchar_t* in) {
  string out;
  return wide2utf8(in, out);
}

/**
 * Convert an UTF-8 encoded char sequence to wide unicode string,
 * the encoding of input char sequence must be known as UTF-8
 */
inline wstring& utf82wide(const char* in, wstring& out) {
  if (in == nullptr) return out;
  unsigned int codepoint;
  while (*in != 0) {
    unsigned char ch = static_cast<unsigned char>(*in);
    if (ch <= 0x7f)
      codepoint = ch;
    else if (ch <= 0xbf)
      codepoint = (codepoint << 6) | (ch & 0x3f);
    else if (ch <= 0xdf)
      codepoint = ch & 0x1f;
    else if (ch <= 0xef)
      codepoint = ch & 0x0f;
    else
      codepoint = ch & 0x07;
    ++in;
    if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff)) {
      if (codepoint > 0xffff) {
        out.append(1, static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
        out.append(1, static_cast<wchar_t>(0xdc00 + (codepoint & 0x03ff)));
      } else if (codepoint < 0xd800 || codepoint >= 0xe000)
        out.append(1, static_cast<wchar_t>(codepoint));
    }
  }
  return out;
}

inline wstring utf82wide(const char* in) {
  wstring out;
  return utf82wide(in, out);
}

}  // namespace tex

#endif
