#ifndef OTF_INCLUDED
#define OTF_INCLUDED

#include "openmath/glyph.h"
#include "openmath/math.h"
#include "utils/utils.h"

namespace tex {

class CLMReader;

/** Table represents standard ligatures. */
using LigaTable = SortedDictTree<uint16, int32>;

/** Represents classified kerning */
struct ClassKerning final {
private:
  uint16 _leftCount = 0;
  /** left glyphs, in pair format (glyph, class_index) */
  uint16* _lefts = nullptr;

  uint16 _rightCount = 0;
  /** right glyphs, in pair format (glyph, class_index) */
  uint16* _rights = nullptr;

  uint16 _rowLength = 0, _columnLength = 0;
  /** kerning value table, size = (_rowLength * _columnLength) */
  int16* _table = nullptr;

  ClassKerning() {}

public:
  __no_copy_assign(ClassKerning);

  /**
   * Get the kerning value for the given left and right glyph.
   * Return [false, 0] if not found.
   */
  std::pair<bool, int16> operator()(uint16 left, uint16 right) const;

  ~ClassKerning();

  friend CLMReader;
};

/** Class to represent an otf font */
class OTFFont final {
private:
  uint16 _unicodeCount = 0;
  uint32* _unicodes = nullptr;
  uint16* _unicodeGlyphs = nullptr;

  uint16 _em = 0;
  bool _isMathFont = false;
  MathConsts* _mathConsts = nullptr;

  LigaTable* _ligatures = nullptr;
  uint16 _classKerningCount = 0;
  ClassKerning** _classKernings = nullptr;

  uint16 _glyphCount = 0;
  Glyph** _glyphs = nullptr;

  OTFFont() {}

public:
  __no_copy_assign(OTFFont);

  /** Read otf font from file */
  static OTFFont* fromFile(const char* filePath);

  inline bool isMathFont() const { return _isMathFont; }

  inline uint16 unicodesCount() const { return _unicodeCount; }

  inline uint16 glyphsCount() const { return _glyphCount; }

  /** Get the em size of this font. */
  inline uint16 em() const { return _em; }

  /** Get the math-consts table, return null if absent. */
  inline const MathConsts* mathConsts() const { return _mathConsts; }

  /** Get the glyph id from the given unicode-codepoint, return -1 if not found. */
  int32 glyphId(uint32 codepoint) const;

  /** Get the glyph from the given unicode-codepoint, return null if not found. */
  const Glyph* glyphOfUnicode(uint32 codepoint) const;

  /** Get the glyph from the given glyph id, return null if absent. */
  const Glyph* glyph(int32 id) const;

  /** Get the ligatures of this font, return null if absent. */
  const LigaTable* ligatures() const { return _ligatures; }

  /**
   * Get the classified-kerning value between left and right glyph.
   *
   * @param left the glyph id of the left character
   * @param right the glyph id of the right character
   * @return the kerning value or 0 if absent
   */
  int16 classKerning(uint16 left, uint16 right) const;

  ~OTFFont();

  friend CLMReader;
};

}  // namespace tex

#endif
