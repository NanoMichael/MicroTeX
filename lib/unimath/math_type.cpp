#include "unimath/math_type.h"

#include "utils/utils.h"

using namespace std;
using namespace microtex;

namespace {

/** The reserved code map, thanks to the Unicode Consortium */
// IMPORTANT: sorted by the reserved code, you must sort this after add item
const pair<c32, c32> _reserved[]{
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

const int _reservedCount = sizeof(_reserved) / sizeof(pair<c32, c32>);

struct Orphan {
  LetterType type;
  c32 code;
  std::map<FontStyle, c32> styles;
};

// clang-format off
const Orphan _orphans[]{
  // latin small letter dotless i
  {
    LetterType::latinSmall,   0x131,
    {
      {FontStyle::rm, 0x131},
      {FontStyle::it, 0x1d6a4},
    }
  },
  // latin small letter dotless j
  {
    LetterType::latinSmall,   0x237,
    {
      {FontStyle::rm, 0x237},
      {FontStyle::it, 0x1d6a5},
    }
  },
  // greek theta symbol
  {
    LetterType::greekSmall,   0x3d1,
    {
      {FontStyle::rm, 0x3d1},
      {FontStyle::bf, 0x1d6dd},
      {FontStyle::it, 0x1d717},
      {FontStyle::bfit, 0x1d751},
      {FontStyle::sfbf, 0x1d78b},
      {FontStyle::sfbfit, 0x1d7c5},
    }
  },
  // greek phi symbol
  {
    LetterType::greekSmall,   0x3d5,
    {
      {FontStyle::rm, 0x3d5},
      {FontStyle::bf, 0x1d6df},
      {FontStyle::it, 0x1d719},
      {FontStyle::bfit, 0x1d753},
      {FontStyle::sfbf, 0x1d78d},
      {FontStyle::sfbfit, 0x1d7c7},
    }
  },
  // greek pi symbol
  {
    LetterType::greekSmall,   0x3d6,
    {
      {FontStyle::rm, 0x3d6},
      {FontStyle::bf, 0x1d6e1},
      {FontStyle::it, 0x1d71b},
      {FontStyle::bfit, 0x1d755},
      {FontStyle::sfbf, 0x1d78f},
      {FontStyle::sfbfit, 0x1d7c9},
    }
  },
  // greek capital letter digamma
  {
    LetterType::greekCapital, 0x3dc,
    {
      {FontStyle::rm, 0x3dc},
      {FontStyle::bf, 0x1d7ca},
    }
  },
  // greek small letter digamma
  {
    LetterType::greekSmall,   0x3dd,
    {
      {FontStyle::rm, 0x3dd},
      {FontStyle::bf, 0x1d7cb},
    }
  },
  // greek kappa symbol
  {
    LetterType::greekSmall,   0x3f0,
    {
      {FontStyle::rm, 0x3f0},
      {FontStyle::bf, 0x1d6de},
      {FontStyle::it, 0x1d718},
      {FontStyle::bfit, 0x1d752},
      {FontStyle::sfbf, 0x1d78c},
      {FontStyle::sfbfit, 0x1d7c6},
    }
  },
  // greek rho symbol
  {
    LetterType::greekSmall,   0x3f1,
    {
      {FontStyle::rm, 0x3f1},
      {FontStyle::bf, 0x1d6e0},
      {FontStyle::it, 0x1d71a},
      {FontStyle::bfit, 0x1d754},
      {FontStyle::sfbf, 0x1d783},
      {FontStyle::sfbfit, 0x1d7c8},
    }
  },
  // greek capital theta symbol
  {
    LetterType::greekCapital, 0x3f4,
    {
      {FontStyle::rm, 0x3f4},
      {FontStyle::bf, 0x1d6b9},
      {FontStyle::it, 0x1d6f3},
      {FontStyle::bfit, 0x1d72d},
      {FontStyle::sfbf, 0x1d767},
      {FontStyle::sfbfit, 0x1d7a1},
    }
  },
  // greek lunate epsilon symbol
  {
    LetterType::greekSmall,   0x3f5,
    {
      {FontStyle::rm, 0x3f5},
      {FontStyle::bf, 0x1d6dc},
      {FontStyle::it, 0x1d716},
      {FontStyle::bfit, 0x1d750},
      {FontStyle::sfbf, 0x1d78a},
      {FontStyle::sfbfit, 0x1d7c4},
    }
  },
  // partial differential
  {
    LetterType::greekSmall,   0x2202,
    {
      {FontStyle::rm, 0x2202},
      {FontStyle::bf, 0x1d6db},
      {FontStyle::it, 0x1d715},
      {FontStyle::bfit, 0x1d74f},
      {FontStyle::sfbf, 0x1d789},
      {FontStyle::sfbfit, 0x1d7c3},
    }
  },
  // nabla
  {
    LetterType::greekCapital, 0x2207,
    {
      {FontStyle::rm, 0x2207},
      {FontStyle::bf, 0x1d6c1},
      {FontStyle::it, 0x1d6fb},
      {FontStyle::bfit, 0x1d735},
      {FontStyle::sfbf, 0x1d76f},
      {FontStyle::sfbfit, 0x1d7a9},
    }
  },
};
// clang-format on

const int _orphansCount = sizeof(_orphans) / sizeof(Orphan);

#define style(digit, latinSmall, latinCapital, greekSmall, greekCapital) \
  sptrOf<const MathVersion>(digit, latinSmall, latinCapital, greekSmall, greekCapital)

/** style to version map */
sptr<const MathVersion> _mathStyles[4]{
  style('0', 0x1D44E, 0x1D434, 0x1D6FC, 0x0391),
  style('0', 0x1D44E, 0x1D434, 0x1D6FC, 0x1D6E2),
  style('0', 0x1D44E, 'A', 0x03B1, 0x0391),
  style('0', 'a', 'A', 0x03B1, 0x0391),
};

#define version(style, digit, latinSmall, latinCapital, greekSmall, greekCapital)                 \
  {                                                                                               \
    style,                                                                                        \
      sptrOf<const MathVersion>(digit, latinSmall, latinCapital, greekSmall, greekCapital, style) \
  }

/** font style to version map */
map<FontStyle, sptr<const MathVersion>> _mathVersions{
  // default TeX style
  {FontStyle::none, _mathStyles[0]},
  version(FontStyle::rm, '0', 'a', 'A', 0x03B1, 0x0391),
  version(FontStyle::bf, 0x1D7CE, 0x1D41A, 0x1D400, 0x1D6C2, 0x1D6A8),
  version(FontStyle::it, '0', 0x1D44E, 0x1D434, 0x1D6FC, 0x1D6E2),
  version(FontStyle::cal, '0', 0x1D4B6, 0x1D49C, 0x03B1, 0x0391),
  version(FontStyle::frak, '0', 0x1D51E, 0x1D504, 0x03B1, 0x0391),
  version(FontStyle::bb, 0x1D7D8, 0x1D552, 0x1D538, 0x03B1, 0x0391),
  version(FontStyle::sf, 0x1D7E2, 0x1D5BA, 0x1D5A0, 0x03B1, 0x0391),
  version(FontStyle::tt, 0x1D7F6, 0x1D68A, 0x1D670, 0x03B1, 0x0391),
 // composed styles
  version(FontStyle::bfit, '0', 0x1D482, 0x1D468, 0x1D736, 0x1D71C),
  version(FontStyle::bfcal, '0', 0x1D4EA, 0x1D4D0, 0x03B1, 0x0391),
  version(FontStyle::bffrak, '0', 0x1D586, 0x1D56C, 0x03B1, 0x0391),
  version(FontStyle::sfbf, 0x1D7EC, 0x1D5EE, 0x1D5D4, 0x1D770, 0x1D756),
  version(FontStyle::sfit, '0', 0x1D622, 0x1D608, 0x03B1, 0x0391),
  version(FontStyle::sfbfit, '0', 0x1D656, 0x1D63C, 0x1D7AA, 0x1D790),
};

/** current math style */
MathStyle _mathStyle = MathStyle::TeX;

/** Get the MathType and the version-specific offset of the given codepoint. */
pair<LetterType, c32> ofChar(c32 codepoint) {
  if (codepoint >= '0' && codepoint <= '9') return {LetterType::digit, codepoint - '0'};
  if (codepoint >= 'a' && codepoint <= 'z') return {LetterType::latinSmall, codepoint - 'a'};
  if (codepoint >= 'A' && codepoint <= 'Z') return {LetterType::latinCapital, codepoint - 'A'};
  // greek small letters: α - ω
  if (codepoint >= 0x03B1 && codepoint <= 0x03C9) {
    return {LetterType::greekSmall, codepoint - 0x03B1};
  }
  // greek capital letters: Α - Ω
  if (codepoint >= 0x0391 && codepoint <= 0x03A9) {
    return {LetterType::greekCapital, codepoint - 0x0391};
  }
  // otherwise no mapping was found
  return {LetterType::none, codepoint};
}

FontStyle fontStyleOfOrphan(const MathStyle mathStyle, const LetterType type) {
  switch (type) {
    case LetterType::latinSmall:
      return (mathStyle == MathStyle::upright ? FontStyle::rm : FontStyle::it);
    case LetterType::latinCapital:
    case LetterType::greekSmall:
      return mathStyle == MathStyle::French || mathStyle == MathStyle::upright ? FontStyle::rm
                                                                               : FontStyle::it;
    case LetterType::greekCapital:
    default: return mathStyle == MathStyle::ISO ? FontStyle::it : FontStyle::rm;
  }
}

}  // namespace

MathVersion::MathVersion(
  c32 digit,
  c32 latinSmall,
  c32 latinCapital,
  c32 greekSmall,
  c32 greekCapital,
  FontStyle fontStyle
) noexcept
    : _codepoints{0, digit, latinSmall, latinCapital, greekSmall, greekCapital},
      _fontStyle(fontStyle) {}

c32 MathVersion::map(const c32 codepoint) const {
  auto [type, offset] = ofChar(codepoint);
  const c32 mapped = _codepoints[static_cast<u8>(type)] + offset;
  // try find from orphans
  if (type == LetterType::none) {
    const int j = binIndexOf(_orphansCount, [&](int j) { return mapped - _orphans[j].code; });
    if (j >= 0) {
      const auto& orphan = _orphans[j];
      FontStyle fontStyle = _fontStyle;
      if (fontStyle == FontStyle::none) {
        fontStyle = fontStyleOfOrphan(_mathStyle, orphan.type);
      }
      const auto it = orphan.styles.find(fontStyle);
      if (it == orphan.styles.end()) {
        return mapped;
      }
      return it->second;
    }
  }
  // try find from reserved
  const int i = binIndexOf(_reservedCount, [&](int i) { return mapped - _reserved[i].first; });
  if (i >= 0) return _reserved[i].second;
  return mapped;
}

void MathVersion::setMathStyle(const MathStyle style) {
  auto v = _mathStyles[static_cast<u8>(style)];
  _mathVersions[FontStyle::none] = v;
}

c32 MathVersion::map(FontStyle style, c32 code) {
  const auto it = _mathVersions.find(style);
  const MathVersion& version = it == _mathVersions.end() ? *_mathVersions[findClosestStyle(style)]
                                                         : *it->second;
  return version.map(code);
}
