#include "otf/clm.h"

#include <cstring>

#include "utils/exceptions.h"

namespace tex {

class BinaryFileReader final {
private:
  constexpr static const uint32 CHUNK_SIZE = 10 * 1024;
  FILE* _file;
  uint8 _buff[CHUNK_SIZE];
  uint32 _currentSize = 0;
  uint32 _index = 0;
  bool _eof = false;

  void readChunk(uint32 remain = 0) {
    if (_eof) return;
    const auto size = CHUNK_SIZE - remain;
    memcpy(_buff, _buff + size, remain);
    auto read = fread(_buff + remain, 1, size, _file);
    if (read < size) _eof = true;
    _currentSize = read + remain;
    _index = 0;
  }

public:
  BinaryFileReader(const char* filePath) {
    _file = fopen(filePath, "rb");
    if (_file == nullptr) {
      throw ex_file_not_found(std::string(filePath) + " cannot be opened.");
    }
  }

  template <typename T>
  T read() {
    const auto bytes = sizeof(T);
    const auto remain = _currentSize - _index;
    if (remain < bytes) readChunk(remain);
    if (_index >= _currentSize) throw ex_eof("");
    const uint8* p = _buff + _index;
    _index += bytes;
    auto shift = bytes - 1;
    T t = 0;
    for (std::size_t i = 0; i < bytes; i++) {
      t |= (T)(*(p + i)) << ((shift - i) * 8);
    }
    return t;
  }

  ~BinaryFileReader() {
    if (_file != nullptr) fclose(_file);
  }
};

void CLMReader::readMeta(OTFFont& font, BinaryFileReader& reader) const {
  font._isMathFont = reader.read<bool>();
  font._em = reader.read<uint16>();
  uint16 count = reader.read<uint16>();
  uint32* unicodes = new uint32[count];
  uint16* glyphs = new uint16[count];
  for (uint16 i = 0; i < count; i++) {
    unicodes[i] = reader.read<uint32>();
    glyphs[i] = reader.read<uint16>();
  }
  font._unicodeCount = count;
  font._unicodes = unicodes;
  font._unicodeGlyphs = glyphs;
}

std::pair<uint16, uint16*> CLMReader::readClassKerningGlyphs(BinaryFileReader& reader) const {
  const uint16 count = reader.read<uint16>();
  uint16* glyphs = new uint16[count * 2];
  for (uint16 i = 0; i < count; i++) {
    glyphs[i << 1] = reader.read<uint16>();
    glyphs[(i << 1) + 1] = reader.read<uint16>();
  }
  return std::make_pair(count, glyphs);
}

ClassKerning* CLMReader::readClassKerning(BinaryFileReader& reader) const {
  ClassKerning* ptr = new ClassKerning();
  ClassKerning& ck = *ptr;
  // read left glyphs
  ck._rowLength = reader.read<uint16>();
  auto [lc, lg] = readClassKerningGlyphs(reader);
  ck._leftCount = lc;
  ck._lefts = lg;
  // read right glyphs
  ck._columnLength = reader.read<uint16>();
  auto [rc, rg] = readClassKerningGlyphs(reader);
  ck._rightCount = rc;
  ck._rights = rg;
  // read table
  const uint32 size = (uint32)ck._rowLength * (uint32)ck._columnLength;
  int16* table = new int16[size];
  for (uint32 i = 0; i < size; i++) {
    table[i] = reader.read<int16>();
  }
  ck._table = table;
  return ptr;
}

void CLMReader::readClassKernings(OTFFont& font, BinaryFileReader& reader) const {
  const uint16 count = reader.read<uint16>();
  font._classKerningCount = count;
  if (count == 0) {
    font._classKernings = nullptr;
    return;
  }
  font._classKernings = new ClassKerning*[count];
  for (uint16 i = 0; i < count; i++) {
    font._classKernings[i] = readClassKerning(reader);
  }
}

LigaTable* CLMReader::readLigatures(BinaryFileReader& reader) const {
  const uint16 glyph = reader.read<uint16>();
  const uint32 liga = reader.read<int32>();
  const uint16 childCount = reader.read<uint16>();
  LigaTable* t = new LigaTable(glyph, liga, childCount);
  for (uint16 i = 0; i < childCount; i++) {
    t->child(i) = readLigatures(reader);
  }
  return t;
}

MathConsts* CLMReader::readMathConsts(BinaryFileReader& reader) const {
  MathConsts* consts = new MathConsts();
  for (uint16 i = 0; i < TEX_MATH_CONSTS_COUNT; i++) {
    consts->_fields[i] = reader.read<int16>();
  }
  return consts;
}

KernRecord* CLMReader::readKerns(BinaryFileReader& reader) const {
  const uint16 count = reader.read<uint16>();
  if (count == 0) return nullptr;
  KernRecord* record = new KernRecord(count);
  for (uint16 i = 0; i < count; i++) {
    record->_fields[i << 1] = reader.read<uint16>();
    record->_fields[(i << 1) + 1] = reader.read<int16>();
  }
  return record;
}

Variants* CLMReader::readVariants(BinaryFileReader& reader) const {
  const uint16 count = reader.read<uint16>();
  if (count == 0) return nullptr;
  Variants* variants = new Variants(count);
  for (uint16 i = 0; i < count; i++) {
    variants->_glyphs[i] = reader.read<uint16>();
  }
  return variants;
}

GlyphAssembly* CLMReader::readGlyphAssembly(BinaryFileReader& reader) const {
  const bool isPresent = reader.read<bool>();
  if (!isPresent) return nullptr;
  const uint16 partCount = reader.read<uint16>();
  GlyphAssembly* assembly = new GlyphAssembly(partCount);
  assembly->_italicsCorrection = reader.read<int16>();
  for (uint16 i = 0; i < partCount; i++) {
    GlyphPart& part = assembly->_parts[i];
    part._glyph = reader.read<uint16>();
    part._flags = reader.read<uint16>();
    part._startConnectorLength = reader.read<uint16>();
    part._endConnectorLength = reader.read<uint16>();
    part._fullAdvance = reader.read<uint16>();
  }
  return assembly;
}

Math* CLMReader::readMath(BinaryFileReader& reader) const {
  Math* ptr = new Math(0);
  Math& math = *ptr;
  math._italicsCorrection = reader.read<int16>();
  math._topAccentAttachment = reader.read<int16>();
  // variants
  Variants* hv = readVariants(reader);
  math._horizontalVariants = hv == nullptr ? &Variants::empty : hv;
  Variants* vv = readVariants(reader);
  math._verticalVariants = vv == nullptr ? &Variants::empty : vv;
  // glyph assembly
  GlyphAssembly* hg = readGlyphAssembly(reader);
  math._horizontalAssembly = hg == nullptr ? &GlyphAssembly::empty : hg;
  GlyphAssembly* vg = readGlyphAssembly(reader);
  math._verticalAssembly = vg == nullptr ? &GlyphAssembly::empty : vg;
  // kern record
  MathKernRecord* record = new MathKernRecord(0);
  for (uint16 i = 0; i < 4; i++) {
    const uint16 count = reader.read<uint16>();
    if (count == 0) {
      record->_fields[i] = &MathKern::empty;
      continue;
    }
    MathKern* kern = new MathKern(count);
    for (uint16 i = 0; i < count; i++) {
      kern->_fields[i << 1] = reader.read<int16>();
      kern->_fields[(i << 1) + 1] = reader.read<int16>();
    }
    record->_fields[i] = kern;
  }
  math._kernRecord = record;
  return ptr;
}

Glyph* CLMReader::readGlyph(bool isMathFont, BinaryFileReader& reader) const {
  Glyph* glyph = new Glyph();
  // Metrics is required
  glyph->_metrics._width = reader.read<int16>();
  glyph->_metrics._height = reader.read<int16>();
  glyph->_metrics._depth = reader.read<int16>();
  // reader kern record, optional
  glyph->_kernRecord = readKerns(reader);
  // read math, optional
  glyph->_math = isMathFont ? readMath(reader) : &Math::empty;
  return glyph;
}

void CLMReader::readGlyphs(OTFFont& font, BinaryFileReader& reader) const {
  const uint16 count = reader.read<uint16>();
  Glyph** glyphs = new Glyph*[count];
  for (uint16 i = 0; i < count; i++) {
    glyphs[i] = readGlyph(font._isMathFont, reader);
  }
  font._glyphCount = count;
  font._glyphs = glyphs;
}

OTFFont* CLMReader::read(const char* clmFilePath) const {
  BinaryFileReader reader(clmFilePath);
  OTFFont* font = new OTFFont();
  readMeta(*font, reader);
  readClassKernings(*font, reader);
  font->_ligatures = readLigatures(reader);
  font->_mathConsts = font->_isMathFont ? readMathConsts(reader) : nullptr;
  readGlyphs(*font, reader);
  return font;
}

}  // namespace tex
