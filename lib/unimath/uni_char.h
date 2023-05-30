#ifndef MICROTEX_UNI_CHAR_H
#define MICROTEX_UNI_CHAR_H

#include "otf/glyph.h"
#include "utils/utils.h"

namespace microtex {

struct OtfFont;

/** Represents a character-glyph to be measured and drawn with its font, glyph id and scale */
struct Char final {
  /** The original code point of the character */
  const c32 code = 0;
  /** The mapped code point (by math style) of the character */
  const c32 mappedCode = 0;
  /** The font id */
  const i32 fontId = -1;
  /** The glyph id, -1 if no corresponding glyph in the font */
  const i32 glyphId = -1;

  /** The glyph scale, can be modified during using */
  float scale = 1.f;

  Char() = default;

  Char(c32 code, c32 mapped, i32 fid, i32 gid, float scale = 1.f);

  Char(const Char& chr);

  /**
   * Create a char-glyph with only font and glyph, the Unicode code-point is 0.
   * <p>
   * Notice that the char-glyph is valid even if its Unicode is 0, because many glyph has no
   * corresponding Unicode code-point.
   */
  static inline Char onlyGlyph(i32 font, i32 glyph, float scale = 1.f) {
    return {0, 0, font, glyph, scale};
  }

  /** Test if current glyph is valid, basically the #_glyph >= 0 */
  inline bool isValid() const { return glyphId >= 0; }

  /** The otf font spec the char belongs to */
  sptr<const OtfFont> otfFont() const;

  /**
   * The glyph of this char-glyph, NOT NULL.
   * If the char has no corresponding glyph was found, the '?' will be
   * returned instead. You should test the validity of this object before
   * call this function.
   */
  const Glyph* glyph() const;

  /** The fallback glyph id if is invalid */
  i32 fallbackGlyphId() const;

  /** The width of this char-glyph */
  float width() const;

  /** The height of this char-glyph */
  float height() const;

  /** The depth of this char-glyph, in positive */
  float depth() const;

  /** The italic correction of this char-glyph */
  float italic() const;

  /** Get the position (shift) to attach top accent */
  float topAccentAttachment() const;

  /** Get the number of vertical larger variants */
  u16 vLargerCount() const;

  /**
   * Get the vertical larger version of this char-glyph, return the copy of
   * this if no larger version was found. The index 0 is this itself.
   */
  Char vLarger(u32 index) const;

  /** Get the number of horizontal larger variants */
  u16 hLargerCount() const;

  /**
   * Get the horizontal larger version of this char-glyph, return the copy of
   * this if no larger version was found. The index 0 is this itself.
   */
  Char hLarger(u32 index) const;

  /**
   * Get the script version of this char-glyph, return the copy of this if
   * no script version was found.
   */
  Char script(u32 index) const;

  /** Get the glyph assembly in vertical direction */
  const GlyphAssembly& vAssembly() const;

  /** Get the glyph assembly in horizontal direction */
  const GlyphAssembly& hAssembly() const;

  /** Get the char of the assembly part */
  const Char assemblyPart(i32 id) const;
};

}  // namespace microtex

#endif  // MICROTEX_UNI_CHAR_H
