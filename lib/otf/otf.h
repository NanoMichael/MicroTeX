#ifndef MICROTEX_OTF_H
#define MICROTEX_OTF_H

#include "otf/glyph.h"
#include "otf/math_consts.h"
#include "utils/utils.h"

namespace microtex {

class CLMReader;

/** Table represents standard ligatures. */
using LigaTable = SortedDictTree<u16, i32>;

/** Represents classified kerning */
struct ClassKerning final {
private:
  u16 _leftCount = 0;
  /** left glyphs, in pair format (glyph, class_index) */
  u16* _lefts = nullptr;

  u16 _rightCount = 0;
  /** right glyphs, in pair format (glyph, class_index) */
  u16* _rights = nullptr;

  u16 _rowLength = 0, _columnLength = 0;
  /** kerning value table, size = (_rowLength * _columnLength) */
  i16* _table = nullptr;

  ClassKerning() = default;

public:
  no_copy_assign(ClassKerning);

  /**
   * Get the kerning value for the given left and right glyph.
   * Return [false, 0] if not found.
   */
  std::pair<bool, i16> operator()(u16 left, u16 right) const;

  ~ClassKerning();

  friend CLMReader;
};

/** Class to represent an otf font */
class Otf final {
private:
  std::string _name;
  std::string _family;

  u16 _style;

  u16 _unicodeCount = 0;
  u32* _unicodes = nullptr;
  u16* _unicodeGlyphs = nullptr;

  u16 _em = 0;
  u16 _xHeight = 0;
  u16 _ascent = 0;
  u16 _descent = 0;

  bool _isMathFont = false;
  MathConsts* _mathConsts = nullptr;
  bool _hasGlyphPath = false;

  LigaTable* _ligatures = nullptr;
  u16 _classKerningCount = 0;
  ClassKerning** _classKernings = nullptr;

  u16 _glyphCount = 0;
  Glyph** _glyphs = nullptr;

  Otf() = default;

public:
  no_copy_assign(Otf);

  /** Undefined value */
  static constexpr auto undefinedMathValue = 0x7FFF;

  /** Read otf font from file */
  static Otf* fromFile(const char* filePath);

  /** Read otf font from data */
  static Otf* fromData(size_t len, const u8* data);

  /** Get the full postscript name of this font */
  inline std::string name() const { return _name; }

  /** Get the postscript family name of this font */
  inline std::string family() const { return _family; }

  /** Test if this font is a math font */
  inline bool isMathFont() const { return _isMathFont; }

  /**
   * Test if this font has glyph's path.
   * Always is false if GLYPH_RENDER_TYPE equals to GLYPH_RENDER_TYPE_TYPEFACE.
   */
  inline bool hasGlyphPath() const { return _hasGlyphPath; }

  /** Get the font style in clm (short enum see [lib/graphics/font_style.h: FontStyle]) format **/
  inline u16 style() const { return _style; }

  /** Get the char count covered by this font */
  inline u16 unicodesCount() const { return _unicodeCount; }

  /** Get the glyphs count covered by this font */
  inline u16 glyphsCount() const { return _glyphCount; }

  /** Get the em size of this font. */
  inline u16 em() const { return _em; }

  /** Get the quad of this font. */
  inline u16 quad() const { return _em; }

  /** Get the x-height of this font. */
  inline u16 xHeight() const { return _xHeight; }

  /** Get the ascent of this font. */
  inline u16 ascent() const { return _ascent; }

  /** Get the descent of this font. */
  inline u16 descent() const { return _descent; }

  /** Get the math-consts table, return null if absent. */
  inline const MathConsts* mathConsts() const { return _mathConsts; }

  /** Get the space size of this font. */
  u16 space() const;

  /**
   * Get the glyph id from the given Unicode code-point, return -1 if not found.
   *
   * <p>
   * The glyph id actually represents the index of the glyph in font file.
   * <strong>Does not support CID-keyed fonts (either in PS or in CFF format).</strong>
   *
   * <p>
   * See
   * <a href="https://docs.microsoft.com/en-us/typography/opentype/spec/cff">
   * CFF - Compact Font Format Table</a> for details.
   *
   * And
   * <a
   * href="https://freetype-py.readthedocs.io/en/latest/ft_face_flags.html#FT_FACE_FLAG_CID_KEYED">
   * FT_FACE_FLAG_CID_KEYED</a> explains why.
   */
  i32 glyphId(c32 codepoint) const;

  /** Get the glyph from the given Unicode code-point, return null if not found. */
  const Glyph* glyphOfUnicode(c32 codepoint) const;

  /** Get the glyph from the given glyph id, return null if absent. */
  const Glyph* glyph(i32 id) const;

  /** Get the ligatures of this font, return null if absent. */
  const LigaTable* ligatures() const { return _ligatures; }

  /**
   * Get the classified-kerning value between left and right glyph.
   *
   * @param left the glyph id of the left character
   * @param right the glyph id of the right character
   * @return the kerning value or 0 if absent
   */
  i16 classKerning(u16 left, u16 right) const;

  ~Otf();

  friend CLMReader;
};

}  // namespace microtex

#endif
