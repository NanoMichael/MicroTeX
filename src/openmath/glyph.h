#ifndef GLYPH_INCLUDED
#define GLYPH_INCLUDED

#include "utils/types.h"

namespace tex {

/** Defines info for one glyph, all divice-tables are JUST IGNORED. */
struct GlyphInfo {
  /** Glyph width */
  int16 width;
  /** Distance above baseline (positive) */
  int16 height;
  /** Distance below baseline (positive) */
  int16 depth;
  /** Italic correction */
  int16 italicCorrection;
  /** Top accent placement */
  int16 topAccent;
};



}  // namespace tex

#endif
