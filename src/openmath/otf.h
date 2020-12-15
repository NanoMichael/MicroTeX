#ifndef OTF_INCLUDED
#define OTF_INCLUDED

#include "openmath/glyph.h"
#include "openmath/math.h"
#include "utils/utils.h"

namespace tex {

class OTFFont {
private:
  uint16 _glyphCount;
  Glyph* _glyphs;
  uint16 _unicodeCount;
  uint32* _unicodeGlyphMap;
  MathConsts* _mathConsts;
};

}  // namespace tex

#endif
