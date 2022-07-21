#ifndef MICROTEX_CLM_H
#define MICROTEX_CLM_H

#include "otf/otf.h"

namespace microtex {

class BinaryReader;

/** Read an Otf from `.clm` file. A class wrapper to read/write private fields of glyph. */
class CLMReader final {
private:
  static void readMeta(Otf& font, BinaryReader& reader);

  static void readClassKernings(Otf& font, BinaryReader& reader);

  static ClassKerning* readClassKerning(BinaryReader& reader);

  static std::pair<u16, u16*> readClassKerningGlyphs(BinaryReader& reader);

  static LigaTable* readLigatures(BinaryReader& reader);

  static MathConsts* readMathConsts(BinaryReader& reader);

  static KernRecord* readKerns(BinaryReader& reader);

  static Variants* readVariants(BinaryReader& reader);

  static GlyphAssembly* readGlyphAssembly(BinaryReader& reader);

  static Math* readMath(BinaryReader& reader);

#ifdef HAVE_GLYPH_RENDER_PATH

  static Path* readPath(BinaryReader& reader);

#else

  static void skipGlyphPath(BinaryReader& reader);

#endif

  static Glyph* readGlyph(bool isMathFont, bool hasGlyphPath, BinaryReader& reader);

  static void readGlyphs(Otf& font, bool hasGlyphPath, BinaryReader& reader);

  static Otf* read(BinaryReader& reader);

public:
  no_copy_assign(CLMReader);

  CLMReader() = default;

  Otf* read(const char* clmFilePath) const;

  Otf* read(size_t len, const u8* bytes) const;
};

}  // namespace microtex

#endif
