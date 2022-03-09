#include "utf.h"

using namespace std;
using namespace microtex;

bool microtex::isVariationSelector(c32 code) {
  if (code <= 0x7ff) return false;
  // variation selectors (Unicode block)
  if (code >= 0xFE00 && code <= 0xFE0F) {
    return true;
  }
  // variation selectors supplement
  if (code >= 0xE0100 && code <= 0xE01EF) {
    return true;
  }
  // Mongolian variation selectors
  if (code >= 0x180B && code <= 0x180D) {
    return true;
  }
  return false;
}

bool microtex::isZWJ(c32 code) {
  return code == 0x200D;
}

bool microtex::isZWNJ(c32 code) {
  return code == 0x200C;
}

bool microtex::isJoiner(c32 code) {
  return isZWJ(code) || isZWNJ(code);
}

void microtex::appendToUtf8(std::string& out, c32 code) {
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

c32 microtex::nextUnicode(const std::string& src, int i, int& cnt) {
  const auto l = src.length();
  if (i >= l) {
    cnt = 0;
    return 0;
  }
  const char* in = src.c_str() + i;
  auto ch = static_cast<unsigned char>(*in);
  // 1 byte
  cnt = 1;
  if (ch <= 0b01111111) {
    return static_cast<c32>(ch);
  }
  // multi-bytes
  c32 code = 0;
  if (ch <= 0b11011111) {
    code = (ch & 0b00011111) << 6;
    cnt = 2;
  } else if (ch <= 0b11101111) {
    code = (ch & 0b00001111) << 12;
    cnt = 3;
  } else if (ch <= 0b11110111) {
    code = (ch & 0b00000111) << 18;
    cnt = 4;
  } else if (ch <= 0b11111011) {
    code = (ch & 0b00000011) << 24;
    cnt = 5;
  } else if (ch <= 0b11111101) {
    code = (ch & 0b00000001) << 30;
    cnt = 6;
  }
  // otherwise, the utf-8 string is malformed
  for (int j = 1; j < cnt; j++) {
    // malformed utf-8 string, we ignore it
    if (i + j >= l) break;
    auto c = static_cast<unsigned char>(*(in + j));
    code |= (c & 0b00111111) << (6 * (cnt - j - 1));
  }
  return code;
}

void microtex::scanContinuedUnicodes(
  const std::function<c32()>& next,
  const std::function<void(c32)>& collect
) {
  const c32 x = next();
  collect(x);
  while (true) {
    const c32 a = next();
    if (microtex::isVariationSelector(a)) {
      collect(a);
    } else if (microtex::isJoiner(a)) {
      collect(a);
      c32 b = 0;
      do {
        b = next();
        collect(b);
      } while (isJoiner(b));
    } else {
      break;
    }
  }
}
