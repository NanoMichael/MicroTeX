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

  /** Get the kerning value for the given left and right glyph. */
  int16 operator()(uint16 left, uint16 right) const;

  ~ClassKerning();

  friend CLMReader;
};

/** Class to represent a otf font */
class OTFFont {
private:
  uint16 _unicodeCount = 0;
  uint32* _unicodes;
  uint16* _unicodeGlyphs;

  bool _isMathFont = false;
  MathConsts* _mathConsts = nullptr;

  uint16 _glyphCount;
  Glyph** _glyphs;

  LigaTable* _ligatures = nullptr;
  ClassKerning** _classKernings = nullptr;

  OTFFont() {}

public:
  __no_copy_assign(OTFFont);

  friend CLMReader;
};

}  // namespace tex

#endif
