#ifndef LATEX_MATH_TYPE_H
#define LATEX_MATH_TYPE_H

#include "utils/utils.h"

namespace tex {

enum class MathType : u8 {
  none,
  digit,
  latinSmall,
  latinCapital,
  greekSmall,
  greekCapital,
};

/**
 * Represents math-version for digit, Latin and Greek alphabets.
 *
 * <p>
 * This for maps a given Unicode character to the mathematical alphanumeric in various
 * math versions.
 *
 * Mathematical Alphanumeric Symbols is a Unicode block comprising styled forms of
 * Latin and Greek letters and decimal digits that enable mathematicians to denote
 * different notions with different letter styles.
 *
 * <p>
 * Unicode includes many such symbols (in the range U+1D400–U+1D7FF). But notice that
 * there are several code-point come up as <Reserved> at Unicode, this class will
 * replace these <Reserved> code-points with the code-point that has the same "glyph"
 * representation.
 *
 * For example, the math-italic version of the character 'h' has the code-point
 * "U+1D455" that is reserved in Unicode, and it will be replaced by "U+210E" that has
 * the meaning of "PLANCK CONSTANT".
 *
 * <p>
 * See
 * <a href="https://en.wikipedia.org/wiki/Mathematical_Alphanumeric_Symbols">
 * Mathematical Alphanumeric Symbols
 * </a>
 * for details.
 */
struct MathVersion {
private:
  /** The reserved code map */
  static const std::pair<c32, c32> _reserved[];
  static const int _reservedCount;

  c32 _codepoints[6];

  /** Get the MathType and the version-specific offset of the given codepoint. */
  static std::pair<MathType, c32> ofChar(c32 codepoint);

public:
  no_copy_assign(MathVersion);

  MathVersion(
    c32 digit, c32 latinSmall, c32 latinCapital, c32 greekSmall, c32 greekCapital
  ) noexcept;

  /** Map an Unicode char to version-specific Unicode char. */
  c32 map(c32 codepoint) const;
};

}

#endif //LATEX_MATH_TYPE_H
