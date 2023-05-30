#ifndef MICROTEX_GLYPH_H
#define MICROTEX_GLYPH_H

#include "otf/path.h"
#include "utils/dict_tree.h"
#include "utils/utils.h"

namespace microtex {

class CLMReader;

class Glyph;

/** Represents metrics for one glyph. */
struct Metrics final {
private:
  i16 _width{}, _height{}, _depth{};

  Metrics() = default;

public:
  no_copy_assign(Metrics);

  /** Glyph width */
  inline i16 width() const { return _width; }

  /** Distance above baseline (positive) */
  inline i16 height() const { return _height; }

  /** Distance below baseline (positive) */
  inline i16 depth() const { return _depth; }

  friend CLMReader;
  friend Glyph;
};

/** Represents standard kerning info for one glyph. */
struct KernRecord final {
private:
  u16 _count;
  u16* _fields;

  explicit KernRecord(u16 count) noexcept
      : _count(count), _fields(_count == 0 ? nullptr : new u16[count * 2]) {}

public:
  no_copy_assign(KernRecord);

  static const KernRecord empty;

  inline u16 count() const { return _count; }

  /** Get the kerning value should be added to the given glyph, return 0 if no kerning. */
  i16 operator[](u16 glyph) const;

  ~KernRecord() { delete[] _fields; }

  friend CLMReader;
};

/** Defines variant for one glyph */
struct Variants final {
private:
  u16 _count = 0;
  /** Array of glyph id in font to represents variant */
  u16* _glyphs = nullptr;

  explicit Variants(u16 count) noexcept
      : _count(count), _glyphs(count == 0 ? nullptr : new u16[count]) {}

public:
  no_copy_assign(Variants);

  /** To represents an empty Variants that the number of its glyphs is 0 */
  static const Variants empty;

  /** Glyph count for this variant */
  inline u16 count() const { return _count; }

  /** Test if the variant is empty */
  inline bool isEmpty() const { return _count == 0; }

  /** Get glyph id at index i */
  inline u16 operator[](u32 i) const { return _glyphs[i]; }

  ~Variants() { delete[] _glyphs; }

  friend CLMReader;
};

struct GlyphAssembly;

/** Defines glyph part to assemble large glyph */
struct GlyphPart final {
private:
  u16 _glyph = 0;
  u16 _startConnectorLength = 0;
  u16 _endConnectorLength = 0;
  u16 _fullAdvance = 0;
  /**
   * Part qualifiers. PartFlags enumeration currently uses only one bit:
   *
   * - 0x0001 EXTENDER_FLAG: If set, the part can be skipped or repeated.
   * - 0xFFFE Reserved.
   */
  u16 _flags = 0;

  GlyphPart() = default;

public:
  no_copy_assign(GlyphPart);

  /** Glyph id for this part. */
  inline u16 glyph() const { return _glyph; }

  /**
   * Advance width/ height, in design units, of the straight bar connector material at the start
   * of the glyph in the direction of the extension (the left end for horizontal extension,
   * the bottom end for vertical extension).
   */
  inline u16 startConnectorLength() const { return _startConnectorLength; }

  /**
   * Advance width/ height, in design units, of the straight bar connector material at the end of
   * the glyph in the direction of the extension (the right end for horizontal extension, the top
   * end for vertical extension).
   */
  inline u16 endConnectorLength() const { return _endConnectorLength; }

  /** Full advance width/height for this part in the direction of the extension, in design units. */
  inline u16 fullAdvance() const { return _fullAdvance; }

  /** Test if this part can be skipped or repeated. */
  inline bool isExtender() const { return (_flags & 0x0001) == 1; }

  friend GlyphAssembly;
  friend CLMReader;
};

/**
 * Specifies how the shape for a particular glyph can be constructed from parts found in the glyph
 * set. This defines the italics correction of the resulting assembly, and a number of parts that
 * have to be put together to form the required shape. Some glyph parts can be designated as
 * extenders, which can be repeated as needed to obtain a target size.
 */
struct GlyphAssembly final {
private:
  i16 _italicsCorrection = 0;
  u16 _partCount = 0;
  /**
   * Array of GlyphPart, from left to right (for assemblies that extend horizontally)
   * or bottom to top (for assemblies that extend vertically).
   */
  GlyphPart* _parts = nullptr;

  explicit GlyphAssembly(u16 partCount) noexcept
      : _partCount(partCount), _parts(partCount == 0 ? nullptr : new GlyphPart[partCount]) {}

public:
  no_copy_assign(GlyphAssembly);

  /** To represents an empty GlyphAssembly that the number of its parts is 0 */
  static const GlyphAssembly empty;

  /** Italics correction of this GlyphAssembly. Should not depend on the assembly size. */
  inline i16 italicsCorrection() const { return _italicsCorrection; }

  /** Number of parts in this assembly. */
  inline u16 partCount() const { return _partCount; }

  /** Test if this assembly is empty. */
  inline bool isEmpty() const { return _partCount == 0; }

  /** Get part at index i */
  inline const GlyphPart& operator[](u16 i) const { return _parts[i]; }

  ~GlyphAssembly() { delete[] _parts; }

  friend CLMReader;
};

/**
 * Provides kerning amounts for different heights in a glyph’s vertical extent. An array of kerning
 * values is provided, each of which applies to a height range. A corresponding array of heights
 * indicate the transition points between consecutive ranges.
 * <p>
 * Correction heights for each glyph are relative to the glyph baseline, with positive height values
 * above the baseline, and negative height values below the baseline. The correctionHeights array is
 * sorted in increasing order, from lowest to highest.
 * <p>
 * The kerning value corresponding to a particular height is determined by finding two consecutive
 * entries in the correctionHeight array such that the given height is greater than or equal to the
 * first entry and less than the second entry. The index of the second entry is used to look up a
 * kerning value in the kernValues array. If the given height is less than the first entry in the
 * correctionHeights array, the first kerning value (index 0) is used. For a height that is greater
 * than or equal to the last entry in the correctionHeights array, the last entry is used.
 */
struct MathKern final {
private:
  const u16 _count = 0;
  /** in (kern, correctionHeight) pair format */
  i16* _fields = nullptr;

  explicit MathKern(u16 count) noexcept
      : _count(count), _fields(count == 0 ? nullptr : new i16[count * 2]) {}

public:
  no_copy_assign(MathKern);

  static const MathKern empty;

  inline u16 count() const { return _count; }

  inline i16 correctionHeight(u16 i) const { return _count == 0 ? 0 : _fields[(i << 1) + 1]; }

  inline i16 value(u16 i) const { return _count == 0 ? 0 : _fields[i << 1]; }

  /** Find the index of the kern values that its correction height closest to the given height. */
  u16 indexOf(i32 height) const;

  ~MathKern() { delete[] _fields; }

  friend CLMReader;
};

/**
 * Each MathKernRecord points to up to four kern info for each of the corners around the glyph.
 * If no kern info is provided for a corner, a kerning amount of zero is assumed.
 */
struct MathKernRecord final {
private:
  /** Content MUST NOT BE NULL, equals &MathKern::empty if absent */
  const MathKern* _fields[4]{
    &MathKern::empty,
    &MathKern::empty,
    &MathKern::empty,
    &MathKern::empty,
  };

  explicit MathKernRecord(u16 ignore) noexcept {}

public:
  no_copy_assign(MathKernRecord);

  static const MathKernRecord empty;

  inline const MathKern& topLeft() const { return *_fields[0]; }

  inline const MathKern& topRight() const { return *_fields[1]; }

  inline const MathKern& bottomLeft() const { return *_fields[2]; }

  inline const MathKern& bottomRight() const { return *_fields[3]; }

  ~MathKernRecord();

  friend CLMReader;
};

/**
 * Provides font data required for math layout.
 *
 * See [https://docs.microsoft.com/en-us/typography/opentype/spec/math]
 */
struct Math final {
private:
  i16 _italicsCorrection = 0;
  i16 _topAccentAttachment = 0;
  /** MUST NOT BE NULL, equals to &Variants::empty if absent */
  const Variants* _horizontalVariants = &Variants::empty;
  /** MUST NOT BE NULL, equals to &Variants::empty if absent */
  const Variants* _verticalVariants = &Variants::empty;
  /** MUST NOT BE NULL, equals to &Variants::empty if absent */
  const Variants* _scriptsVariants = &Variants::empty;
  /** MUST NOT BE NULL, equals to &GlyphAssembly::empty if absent */
  const GlyphAssembly* _horizontalAssembly = &GlyphAssembly::empty;
  /** MUST NOT BE NULL, equals to &GlyphAssembly::empty if absent */
  const GlyphAssembly* _verticalAssembly = &GlyphAssembly::empty;
  /** MUST NOT BE NULL, equals to &MathKernRecord::empty if absent */
  const MathKernRecord* _kernRecord = &MathKernRecord::empty;

  explicit Math(u16 ignore) noexcept {};

public:
  no_copy_assign(Math);

  static const Math empty;

  /** Italics correction */
  inline i16 italicsCorrection() const { return _italicsCorrection; }

  /** Top accent attachment */
  inline i16 topAccentAttachment() const { return _topAccentAttachment; }

  /**
   * Alternate forms of the current glyph for use in typesetting math,
   * are of different sizes to layout horizontally.
   *
   * If absent, return Variants::emtpy
   */
  inline const Variants& horizontalVariants() const { return *_horizontalVariants; }

  /**
   * Alternate forms of the current glyph for use in typesetting math,
   * are of different sizes to layout vertically.
   *
   * If absent, return Variants::empty
   */
  inline const Variants& verticalVariants() const { return *_verticalVariants; }

  /***
   * Alternate forms of the current glyph for use in typesetting math,
   * are of different sizes to place scripts.
   *
   * If absent, return Variants::empty
   */
  inline const Variants& scriptsVariants() const { return *_scriptsVariants; }

  /**
   * This allows constructing very large versions of the glyph by stacking the components together
   * horizontally. Some components may be repeated so there is no bound on the size.
   *
   * This is different from horizontalVariants which expects prebuilt glyphs of various fixed sizes.
   *
   * If absent, return GlyphAssembly::empty
   */
  inline const GlyphAssembly& horizontalAssembly() const { return *_horizontalAssembly; }

  /**
   * This allows constructing very large versions of the glyph by stacking the components together
   * vertically. Some components may be repeated so there is no bound on the size.
   *
   * This is different from verticalVariants which expects prebuilt glyphs of various fixed sizes.
   *
   * If absent, return GlyphAssembly::empty
   */
  inline const GlyphAssembly& verticalAssembly() const { return *_verticalAssembly; }

  inline const MathKernRecord& kernRecord() const { return *_kernRecord; }

  ~Math();

  friend CLMReader;
};

/** Defines info for one glyph, device-table is JUST IGNORED. */
struct Glyph final {
private:
  Metrics _metrics{};
  /** MUST NOT BE NULL, equals to &KernRecord::empty if absent */
  const KernRecord* _kernRecord = &KernRecord::empty;
  /** MUST NOT BE NULL, equals to &Math::empty if absent */
  const Math* _math = &Math::empty;

#ifdef HAVE_GLYPH_RENDER_PATH
  /** MUST NOT BE NULL, equals to &Path::empty if absent */
  const Path* _path = &Path::empty;
#endif

  Glyph() = default;

public:
  no_copy_assign(Glyph);

  inline const Metrics& metrics() const { return _metrics; }

  inline const Math& math() const { return *_math; }

  inline const KernRecord& kernRecord() const { return *_kernRecord; }

#ifdef HAVE_GLYPH_RENDER_PATH

  inline const Path& path() const { return *_path; }

#endif

  ~Glyph();

  friend CLMReader;
};

}  // namespace microtex

#endif
