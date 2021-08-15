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

c32 tex::nextUnicode(const std::string& src, int& i) {
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
  i += n - 1;
  return code;
}
