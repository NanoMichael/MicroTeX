#ifndef CLM_INCLUDED
#define CLM_INCLUDED

#include <cstdio>

#include "otf/otf.h"

namespace tex {

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

  static Path* readPath(BinaryReader& reader);

  static Glyph* readGlyph(bool isMathFont, BinaryReader& reader);

  static void readGlyphs(Otf& font, BinaryReader& reader);

public:
  no_copy_assign(CLMReader);

  CLMReader() = default;

  Otf* read(const char* clmFilePath) const;

  Otf* read(size_t cnt, const u8* bytes) const;
};

}  // namespace tex

#endif
