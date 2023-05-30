#ifndef MICROTEX_MATH_TYPE_H
#define MICROTEX_MATH_TYPE_H

#include <map>

#include "graphic/font_style.h"
#include "utils/utils.h"

namespace microtex {

/*
 * The math styles. The math glyphs are painted in following styles:
 *
 * style    latin  Latin  greek  Greek
 * -----------------------------------
 * TeX      it     it     it     up
 * ISO      it     it     it     it
 * French   it     up     up     up
 * upright  up     up     up     up
 */
enum class MathStyle : u8 {
  TeX,
  ISO,
  French,
  upright,
};

enum class LetterType : u8 {
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
 * there are several code-point come up as <Reserved> at Unicode, this handler will
 * replace these <Reserved> code-points with the code-point that has the same "glyph"
 * representation.
 *
 * <p>
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
class MathVersion {
private:
  c32 _codepoints[6];
  FontStyle _fontStyle;

public:
  no_copy_assign(MathVersion);

  MathVersion(
    c32 digit,
    c32 latinSmall,
    c32 latinCapital,
    c32 greekSmall,
    c32 greekCapital,
    FontStyle fontStyle = FontStyle::none
  ) noexcept;

  /** Map an Unicode char to version-specific Unicode char. */
  c32 map(c32 codepoint) const;

  /**
   * Set math style to display formulas.
   *
   * @param style one of the TeX, ISO, French, and upright
   */
  static void setMathStyle(MathStyle style);

  /**
   * Map an Unicode char to version-specific Unicode char.
   *
   * @param style the font style
   * @param code the given code
   * @return the mapped version-specific Unicode code-point
   */
  static c32 map(FontStyle style, c32 code);
};

}  // namespace microtex

#endif  // MICROTEX_MATH_TYPE_H
