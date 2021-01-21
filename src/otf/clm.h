#ifndef CLM_INCLUDED
#define CLM_INCLUDED

#include <cstdio>

#include "otf/otf.h"

namespace tex {

class BinaryFileReader;

/** Read a OTFFont from `.clm` file. */
class CLMReader final {
private:
  void readMeta(OTFFont& font, BinaryFileReader& reader) const;

  void readClassKernings(OTFFont& font, BinaryFileReader& reader) const;

  ClassKerning* readClassKerning(BinaryFileReader& reader) const;

  std::pair<u16, u16*> readClassKerningGlyphs(BinaryFileReader& reader) const;

  LigaTable* readLigatures(BinaryFileReader& reaader) const;

  MathConsts* readMathConsts(BinaryFileReader& reader) const;

  KernRecord* readKerns(BinaryFileReader& reader) const;

  Variants* readVariants(BinaryFileReader& reader) const;

  GlyphAssembly* readGlyphAssembly(BinaryFileReader& reader) const;

  Math* readMath(BinaryFileReader& reader) const;

  Glyph* readGlyph(bool isMathFont, BinaryFileReader& reader) const;

  void readGlyphs(OTFFont& font, BinaryFileReader& reader) const;

public:
  __no_copy_assign(CLMReader);

  CLMReader() {}

  OTFFont* read(const char* clmFilePath) const;
};

}  // namespace tex

#endif
