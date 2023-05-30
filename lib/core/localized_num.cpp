#include "core/parser.h"

using namespace microtex;

microtex::c32 microtex::convertToRomanNumber(c32 c) {
  if (c == 0x66b) {                         // Arabic dot
    return '.';
  } else if (0x660 <= c && c <= 0x669) {    // Arabic
    return (c32)(c - (c32)0x630);
  } else if (0x6f0 <= c && c <= 0x6f9) {    // Arabic
    return (c32)(c - (c32)0x6c0);
  } else if (0x966 <= c && c <= 0x96f) {    // Devanagari
    return (c32)(c - (c32)0x936);
  } else if (0x9e6 <= c && c <= 0x9ef) {    // Bengali
    return (c32)(c - (c32)0x9b6);
  } else if (0xa66 <= c && c <= 0xa6f) {    // Gurmukhi
    return (c32)(c - (c32)0xa36);
  } else if (0xae6 <= c && c <= 0xaef) {    // Gujarati
    return (c32)(c - (c32)0xab6);
  } else if (0xb66 <= c && c <= 0xb6f) {    // Oriya
    return (c32)(c - (c32)0xb36);
  } else if (0xc66 <= c && c <= 0xc6f) {    // Telugu
    return (c32)(c - (c32)0xc36);
  } else if (0xd66 <= c && c <= 0xd6f) {    // Malayalam
    return (c32)(c - (c32)0xd36);
  } else if (0xe50 <= c && c <= 0xe59) {    // Thai
    return (c32)(c - (c32)0xe20);
  } else if (0xed0 <= c && c <= 0xed9) {    // Lao
    return (c32)(c - (c32)0xea0);
  } else if (0xf20 <= c && c <= 0xf29) {    // Tibetan
    return (c32)(c - (c32)0xe90);
  } else if (0x1040 <= c && c <= 0x1049) {  // Myanmar
    return (c32)(c - (c32)0x1010);
  } else if (0x17e0 <= c && c <= 0x17e9) {  // Khmer
    return (c32)(c - (c32)0x17b0);
  } else if (0x1810 <= c && c <= 0x1819) {  // Mongolian
    return (c32)(c - (c32)0x17e0);
  } else if (0x1b50 <= c && c <= 0x1b59) {  // Balinese
    return (c32)(c - (c32)0x1b20);
  } else if (0x1bb0 <= c && c <= 0x1bb9) {  // Sundanese
    return (c32)(c - (c32)0x1b80);
  } else if (0x1c40 <= c && c <= 0x1c49) {  // Lepcha
    return (c32)(c - (c32)0x1c10);
  } else if (0x1c50 <= c && c <= 0x1c59) {  // Ol Chiki
    return (c32)(c - (c32)0x1c20);
  } else if (0xa8d0 <= c && c <= 0xa8d9) {  // Saurashtra
    return (c32)(c - (c32)0xa8a0);
  }

  return c;
}
