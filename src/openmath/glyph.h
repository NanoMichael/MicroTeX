#ifndef GLYPH_INCLUDED
#define GLYPH_INCLUDED

#include "utils/types.h"

namespace tex {

/** Defines variants for one glyph */
struct Variants {
private:
  uint16 count = 0;
  /** Array of glyph id in font to represents varints */
  uint16* glyphs;

public:
  __no_copy_assign(Variants);

  static const Variants empty;

  /** Glyph count for this variants */
  inline uint16 count() const { return count; }

  /** Get glyph id at index i */
  inline uint16 operator[](int i) const { return glyphs[i]; }
};

/** Defines glyph part to assemble large glyph */
struct GlyphPart {
private:
  uint16 glyph;
  uint16 startConnectorLength;
  uint16 endConnectorLength;
  uint16 fullAdvance;
  /**
   * Part qualifiers. PartFlags enumeration currently uses only one bit:
   *
   * - 0x0001 EXTENDER_FLAG: If set, the part can be skipped or repeated.
   * - 0xFFFE Reserved.
   */
  uint16 flags;

public:
  __no_copy_assign(GlyphPart);

  /** Glyph id for this part. */
  inline uint16 glyph() const { return glyph; }

  /**
   * Advance width/ height, in design units, of the straight bar connector material at the start
   * of the glyph in the direction of the extension (the left end for horizontal extension,
   * the bottom end for vertical extension).
   */
  inline uint16 startConnectorLength() const { return startConnectorLength; }

  /**
   * Advance width/ height, in design units, of the straight bar connector material at the end of
   * the glyph in the direction of the extension (the right end for horizontal extension,
   * the top end for vertical extension).
   */
  inline uint16 endConnectorLength() const { return endConnectorLength; }

  /** Full advance width/height for this part in the direction of the extension, in design units. */
  inline uint16 fullAdvance() const { return fullAdvance; }

  /** Test if this part can be skipped or repeated. */
  inline bool isExtender() const { return flags & 0x0001 == 1; }
};

/**
 * Specifies how the shape for a particular glyph can be constructed from parts found in the glyph
 * set. This defines the italics correction of the resulting assembly, and a number of parts that
 * have to be put together to form the required shape. Some glyph parts can be designated as
 * extenders, which can be repeated as needed to obtain a target size.
 */
struct GlyphAssembly {
private:
  int16 italicsCorrection;
  uint16 partCount = 0;
  /**
   * Array of GlyphPart, from left to right (for assemblies that extend horizontally)
   * or bottom to top (for assemblies that extend vertically).
   */
  GlyphPart* parts;

public:
  __no_copy_assign(GlyphAssembly);

  static const GlyphAssembly empty;

  /** Italics correction of this GlyphAssembly. Should not depend on the assembly size. */
  inline int16 italicsCorrection() const { return italicsCorrection; }

  /** Number of parts in this assembly. */
  inline uint16 partCount() const { return partCount; }

  /** Get part at index i */
  inline const GlyphPart& operator[](int i) const { return parts[i]; }
};

/** Defines info for one glyph, divice-table is JUST IGNORED. */
struct GlyphInfo {
private:
  int16 width;
  int16 height;
  int16 depth;
  int16 italicsCorrection;
  int16 topAccentAttachment;
  Variants* horizontalVariants;
  Variants* verticalVariants;
  GlyphAssembly* horizontalAssembly;
  GlyphAssembly* verticalAssembly;

public:
  __no_copy_assign(GlyphInfo);

  /** Glyph width */
  inline int16 width() const { return width; }

  /** Distance above baseline (positive) */
  inline int16 height() const { return height; }

  /** Distance below baseline (positive) */
  inline int16 depth() const { return depth; }

  /** Italics correction */
  inline int16 italicsCorrection() const { return italicsCorrection; }

  /** Top accent attachment */
  inline int16 topAccentAttachment() const { return topAccentAttachment; }

  /**
   * Alternate forms of the current glyph for use in typesetting math,
   * are of different sizes to layout horizontally.
   *
   * If absent, return Variants::emtpy
   */
  inline const Variants& horizontalVariants() const {
    return horizontalVariants == nullptr ? Variants::empty : *horizontalVariants;
  }

  /**
   * Alternate forms of the current glyph for use in typesetting math,
   * are of different sizes to layout vertically.
   *
   * If absent, return Variants::empty
   */
  inline const Variants& verticalVariants() const {
    return verticalVariants == nullptr ? Variants::empty : *verticalVariants;
  }

  /**
   * This allows constructing very large versions of the glyph by stacking the componants together
   * horizontally. Some components may be repeated so there is no bound on the size.
   *
   * This is different from horizontalVariants which expects prebuilt glyphs of various fixed sizes.
   *
   * If absent, return GlyphAssembly::empty
   */
  inline const GlyphAssembly& horizontalAssembly() const {
    return horizontalAssembly == nullptr ? GlyphAssembly::empty : *horizontalAssembly;
  }

  /**
   * This allows constructing very large versions of the glyph by stacking the componants together
   * vertically. Some components may be repeated so there is no bound on the size.
   *
   * This is different from verticalVariants which expects prebuilt glyphs of various fixed sizes.
   *
   * If absent, return GlyphAssembly::emtpy
   */
  inline const GlyphAssembly& verticalAssembly() const {
    return verticalVariants == nullptr ? GlyphAssembly::empty : *verticalAssembly;
  }
};

}  // namespace tex

#endif
