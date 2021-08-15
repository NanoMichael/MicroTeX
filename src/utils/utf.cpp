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

c32 tex::nextUnicode(std::string& src, std::size_t& i) {
  const auto l = src.length();
  if (i >= l) return 0;
  const char* in = src.c_str() + i;
  auto ch = static_cast<unsigned char>(*in);
  // 1 byte
  if (ch <= 0b01111111) {
    return static_cast<c32>(ch);
  }
  // multi-bytes
  size_t n = 0;
  c32 code = 0;
  if (ch <= 0b11011111) {
    code = (ch & 0b00011111) << 6;
    n = 2;
  } else if (ch <= 0b11101111) {
    code = (ch & 0b00001111) << 12;
    n = 3;
  } else if (ch <= 0b11110111) {
    code = (ch & 0b00000111) << 18;
    n = 4;
  } else if (ch <= 0b11111011) {
    code = (ch & 0b00000011) << 24;
    n = 5;
  } else if (ch <= 0b11111101) {
    code = (ch & 0b00000001) << 30;
  }
  for (size_t j = 1; j < n; j++) {
    // malformed utf-8 string, we ignore it
    if (i + j >= l) break;
    auto c = static_cast<unsigned char>(*(in + j));
    code |= (c & 0b00111111) << (6 * (n - j - 1));
  }
  return code;
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
