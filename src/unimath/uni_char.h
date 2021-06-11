#ifndef LATEX_UNI_CHAR_H
#define LATEX_UNI_CHAR_H

#include "utils/utils.h"
#include "otf/glyph.h"

namespace tex {

/** Represents a character-glyph to be measured and drawn with its font, glyph id and scale */
struct Char final {
  /** The original code point of the character */
  const c32 _code = 0;
  /** The mapped code point (by math style) of the character */
  const c32 _mappedCode = 0;
  /** The font id */
  const i32 _font = -1;
  /** The glyph id, -1 if no corresponding glyph in the font */
  const i32 _glyph = -1;

  /** The glyph scale */
  float _scale = 1.f;

  /** Test if current glyph is valid, basically the #_glyph >= 0 */
  bool isValid() const;

  /** The glyph of this char-glyph */
  const Glyph* glyph() const;

  /** The width of this char-glyph */
  float width() const;

  /** The height of this char-glyph */
  float height() const;

  /** The depth of this char-glyph, in positive */
  float depth() const;
};

}

#endif //LATEX_UNI_CHAR_H
