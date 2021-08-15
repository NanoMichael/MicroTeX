#include "utf.h"

using namespace std;
using namespace tex;

void tex::appendToUtf8(std::string& out, c32 code) {
  if (code <= 0x7f) {
    out.append(1, static_cast<char>(code));
  } else if (code <= 0x7ff) {
    out.append(1, static_cast<char>(0xc0 | ((code >> 6) & 0x1f)));
    out.append(1, static_cast<char>(0x80 | (code & 0x3f)));
  } else if (code <= 0xffff) {
    out.append(1, static_cast<char>(0xe0 | ((code >> 12) & 0x0f)));
    out.append(1, static_cast<char>(0x80 | ((code >> 6) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | (code & 0x3f)));
  } else {
    out.append(1, static_cast<char>(0xf0 | ((code >> 18) & 0x07)));
    out.append(1, static_cast<char>(0x80 | ((code >> 12) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | ((code >> 6) & 0x3f)));
    out.append(1, static_cast<char>(0x80 | (code & 0x3f)));
  }
}

std::string tex::wide2utf8(const std::wstring& src) {
  std::string out;
  const wchar_t* in = src.c_str();
  c32 codepoint = 0;
  for (; *in != 0; ++in) {
    if (*in >= 0xd800 && *in <= 0xdbff) {
      // surrogate
      codepoint = ((*in - 0xd800) << 10) + 0x10000;
    } else {
      if (*in >= 0xdc00 && *in <= 0xdfff) {
        codepoint |= *in - 0xdc00;
      } else {
        codepoint = *in;
      }
      appendToUtf8(out, codepoint);
      codepoint = 0;
    }
  }
  return out;
}

wstring tex::utf82wide(const std::string& src) {
  const char* in = src.c_str();
  wstring out;
  if (in == nullptr) return out;
  unsigned int codepoint;
  while (*in != 0) {
    auto ch = static_cast<unsigned char>(*in);
    if (ch <= 0x7f) {
      codepoint = ch;
    } else if (ch <= 0xbf) {
      codepoint = (codepoint << 6) | (ch & 0x3f);
    } else if (ch <= 0xdf) {
      codepoint = ch & 0x1f;
    } else if (ch <= 0xef) {
      codepoint = ch & 0x0f;
    } else {
      codepoint = ch & 0x07;
    }
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
