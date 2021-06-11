#include "unimath/math_type.h"
#include "utils/utils.h"

using namespace std;
using namespace tex;

// IMPORTANT: sorted by the reserved code, you must sort this after add item
const pair<c32, c32> MathVersion::_reserved[]{
  // greek capital normal
  {0x3a2,   0x3f4},
  {0x3aa,   0x2207},
  // greek small normal
  {0x3ca,   0x2202},
  {0x3cb,   0x3f5},
  {0x3cc,   0x3d1},
  {0x3cd,   0x3f0},
  {0x3ce,   0x3d5},
  {0x3cf,   0x3f1},
  {0x3d0,   0x3d6},
  // small serif italic
  {0x1d455, 0x210e},
  // capital calligraphy normal
  {0x1d49d, 0x212c},
  {0x1d4a0, 0x2130},
  {0x1d4a1, 0x2131},
  {0x1d4a3, 0x210b},
  {0x1d4a4, 0x2110},
  {0x1d4a7, 0x2112},
  {0x1d4a8, 0x2133},
  {0x1d4ad, 0x211b},
  // small script normal
  {0x1d4ba, 0x212f},
  {0x1d4bc, 0x210a},
  {0x1d4c4, 0x2134},
  // capital fraktur normal
  {0x1d506, 0x212d},
  {0x1d50b, 0x210c},
  {0x1d50c, 0x2111},
  {0x1d515, 0x211c},
  {0x1d51d, 0x2128},
  // capital double-struck bold
  {0x1d53a, 0x2102},
  {0x1d53f, 0x210d},
  {0x1d545, 0x2115},
  {0x1d547, 0x2119},
  {0x1d548, 0x211a},
  {0x1d549, 0x211d},
  {0x1d551, 0x2124},
};

const int MathVersion::_reservedCount = sizeof(_reserved) / sizeof(pair<c32, c32>);

MathVersion::MathVersion(
  c32 digit, c32 latinSmall, c32 latinCapital, c32 greekSmall, c32 greekCapital
) noexcept: _codepoints{0, digit, latinSmall, latinCapital, greekSmall, greekCapital} {}

pair<MathType, c32> MathVersion::ofChar(c32 codepoint) {
  if (codepoint >= '0' && codepoint <= '9') return {MathType::digit, codepoint - '0'};
  if (codepoint >= 'a' && codepoint <= 'z') return {MathType::latinSmall, codepoint - 'a'};
  if (codepoint >= 'A' && codepoint <= 'Z') return {MathType::latinCapital, codepoint - 'A'};
  // greek small letters: α - ω
  if (codepoint >= 0x03B1 && codepoint <= 0x03C9) {
    return {MathType::greekSmall, codepoint - 0x03B1};
  }
  // greek capital letters: Α - Ω
  if (codepoint >= 0x0391 && codepoint <= 0x03A9) {
    return {MathType::greekCapital, codepoint - 0x0391};
  }
  // otherwise no mapping was found
  return {MathType::none, codepoint};
}

c32 MathVersion::map(const c32 codepoint) const {
  auto[type, offset] = ofChar(codepoint);
  const c32 mapped = _codepoints[static_cast<u8>(type)] + offset;
  const int i = binIndexOf(
    _reservedCount,
    [&](int i) { return mapped - _reserved[i].first; }
  );
  if (i >= 0) return _reserved[i].second;
  return mapped;
}
