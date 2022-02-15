#ifndef CLM_INCLUDED
#define CLM_INCLUDED

#include "config.h"
#include "otf/otf.h"

namespace tinytex {

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

#ifdef HAVE_GLYPH_RENDER_PATH

  static Glyph* readGlyph(bool isMathFont, BinaryReader& reader);

#endif

  static void readGlyphs(Otf& font, BinaryReader& reader);

  static Otf* read(BinaryReader& reader);

public:
  no_copy_assign(CLMReader);

  CLMReader() = default;

  Otf* read(const char* clmFilePath) const;

  Otf* read(size_t len, const u8* bytes) const;
};

}  // namespace tinytex

#endif
