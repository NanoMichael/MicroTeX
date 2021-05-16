#ifndef CLM_INCLUDED
#define CLM_INCLUDED

#include <cstdio>

#include "otf/otf.h"

namespace tex {

class BinaryFileReader;

/** Read a Otf from `.clm` file. */
class CLMReader final {
private:
  static void readMeta(Otf& font, BinaryFileReader& reader);

  static void readClassKernings(Otf& font, BinaryFileReader& reader);

  static ClassKerning* readClassKerning(BinaryFileReader& reader);

  static std::pair<u16, u16*> readClassKerningGlyphs(BinaryFileReader& reader);

  LigaTable* readLigatures(BinaryFileReader& reader) const;

  static MathConsts* readMathConsts(BinaryFileReader& reader);

  static KernRecord* readKerns(BinaryFileReader& reader);

  static Variants* readVariants(BinaryFileReader& reader);

  static GlyphAssembly* readGlyphAssembly(BinaryFileReader& reader);

  static Math* readMath(BinaryFileReader& reader);

  static Glyph* readGlyph(bool isMathFont, BinaryFileReader& reader);

  static void readGlyphs(Otf& font, BinaryFileReader& reader);

public:
  no_copy_assign(CLMReader);

  CLMReader() = default;

  Otf* read(const char* clmFilePath) const;
};

}  // namespace tex

#endif
