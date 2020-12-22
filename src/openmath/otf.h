#ifndef OTF_INCLUDED
#define OTF_INCLUDED

#include "openmath/glyph.h"
#include "openmath/math.h"
#include "utils/utils.h"

namespace tex {

class CLMReader;

/** Table represents standard ligatures. */
using LigaTable = SortedDictTree<int32, int32>;

/** Represents classified kerning */
struct ClassKerning final {
private:
  uint16 _leftCount;
  uint16* _lefts;
  uint16 _rightCount;
  uint16* _rights;
  uint16 _rowLength, _columnLength;
  int16* _table;

public:
  /** Get the kerning value for the given left and right glyph. */
  int16 operator()(uint16 left, uint16 right) const;

  ~ClassKerning();

  friend CLMReader;
};

/** Class to represent a otf font */
class OTFFont {
private:
  uint16 _unicodeCount;
  uint32 _unicodes[];
  uint32 _unicodeGlyphs[];

  bool _isMathFont;
  MathConsts* _mathConsts;

  uint16 _glyphCount;
  Glyph* _glyphs[];

  LigaTable* _ligatures;
  ClassKerning* _classKerning;

  friend CLMReader;
};

}  // namespace tex

#endif
