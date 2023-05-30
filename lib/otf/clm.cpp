#include "otf/clm.h"

#include <cstring>

#include "utils/exceptions.h"
#include "utils/string_utils.h"

namespace microtex {

class BinaryReader {
public:
  virtual ~BinaryReader() = default;

  virtual const u8* readBytes(size_t bytes) = 0;

  virtual size_t lookfwd(u8 until) = 0;

  template <typename T>
  T read() {
    const auto bytes = sizeof(T);
    const u8* p = readBytes(bytes);
    auto shift = bytes - 1;
    T t = 0;
    for (std::size_t i = 0; i < bytes; i++) {
      t |= static_cast<T>(*(p + i)) << ((shift - i) * 8);
    }
    return t;
  }

  const char* readString() {
    size_t len = lookfwd(0x00);
    const char* ret = (const char*)readBytes(len);
    return ret;
  }
};

class BinaryFileReader : public BinaryReader {
private:
  constexpr static const u32 CHUNK_SIZE = 10 * 1024;
  FILE* _file;
  u8 _buff[CHUNK_SIZE]{};
  u32 _currentSize = 0;
  u32 _index = 0;
  bool _eof = false;

  void readChunk(u32 remain = 0) {
    if (_eof) return;
    const auto size = CHUNK_SIZE - remain;
    memcpy(_buff, _buff + size, remain);
    auto read = fread(_buff + remain, 1, size, _file);
    if (read < size) _eof = true;
    _currentSize = read + remain;
    _index = 0;
  }

public:
  explicit BinaryFileReader(const char* filePath) {
    _file = fopen(filePath, "rb");
    if (_file == nullptr) {
      throw ex_file_not_found(std::string(filePath) + " cannot be opened.");
    }
  }

  const u8* readBytes(size_t bytes) override {
    const auto remain = _currentSize - _index;
    if (remain < bytes) readChunk(remain);
    if (_index >= _currentSize) throw ex_eof("end of data");
    const u8* p = _buff + _index;
    _index += bytes;
    return p;
  }

  size_t lookfwd(u8 until) override {
    size_t len = 0;
    u8 active = 0x00;
    while (active != until || len == 0) {
      const auto remain = _currentSize - (_index + len);
      if (remain < 32) readChunk(32);

      if (_index + len >= _currentSize) throw ex_eof("end of data");
      active = _buff[_index + len];
      len++;
    }
    return len;
  }

  ~BinaryFileReader() override {
    if (_file != nullptr) fclose(_file);
  }
};

class BinaryDataReader : public BinaryReader {
private:
  const u8* _data;
  const size_t _len;
  u32 _index = 0;

public:
  explicit BinaryDataReader(size_t len, const u8* data) : _len(len), _data(data) {}

  const u8* readBytes(size_t bytes) override {
    if (_index >= _len) throw ex_eof("end of data");
    const u8* p = _data + _index;
    _index += bytes;
    return p;
  }

  size_t lookfwd(u8 until) override {
    size_t len = 0;
    u8 active = 0x00;
    while (active != until || len == 0) {
      if (_index + len >= _len) throw ex_eof("end of data");
      active = _data[_index + len];
      len++;
    }
    return len;
  }
};

void CLMReader::readMeta(Otf& font, BinaryReader& reader) {
  font._name = std::string(reader.readString());
  font._family = std::string(reader.readString());
  font._isMathFont = reader.read<bool>();
  font._style = reader.read<u16>();
  font._em = reader.read<u16>();
  font._xHeight = reader.read<u16>();
  font._ascent = reader.read<u16>();
  font._descent = reader.read<u16>();
  u16 count = reader.read<u16>();
  u32* unicodes = new u32[count];
  u16* glyphs = new u16[count];
  for (u16 i = 0; i < count; i++) {
    unicodes[i] = reader.read<u32>();
    glyphs[i] = reader.read<u16>();
  }
  font._unicodeCount = count;
  font._unicodes = unicodes;
  font._unicodeGlyphs = glyphs;
}

std::pair<u16, u16*> CLMReader::readClassKerningGlyphs(BinaryReader& reader) {
  const u16 count = reader.read<u16>();
  u16* glyphs = new u16[count * 2];
  for (u16 i = 0; i < count; i++) {
    glyphs[i << 1] = reader.read<u16>();
    glyphs[(i << 1) + 1] = reader.read<u16>();
  }
  return std::make_pair(count, glyphs);
}

ClassKerning* CLMReader::readClassKerning(BinaryReader& reader) {
  auto* ptr = new ClassKerning();
  ClassKerning& ck = *ptr;
  // read left glyphs
  ck._rowLength = reader.read<u16>();
  auto [lc, lg] = readClassKerningGlyphs(reader);
  ck._leftCount = lc;
  ck._lefts = lg;
  // read right glyphs
  ck._columnLength = reader.read<u16>();
  auto [rc, rg] = readClassKerningGlyphs(reader);
  ck._rightCount = rc;
  ck._rights = rg;
  // read table
  const u32 size = (u32)ck._rowLength * (u32)ck._columnLength;
  i16* table = new i16[size];
  for (u32 i = 0; i < size; i++) {
    table[i] = reader.read<i16>();
  }
  ck._table = table;
  return ptr;
}

void CLMReader::readClassKernings(Otf& font, BinaryReader& reader) {
  const u16 count = reader.read<u16>();
  font._classKerningCount = count;
  if (count == 0) {
    font._classKernings = nullptr;
    return;
  }
  font._classKernings = new ClassKerning*[count];
  for (u16 i = 0; i < count; i++) {
    font._classKernings[i] = readClassKerning(reader);
  }
}

LigaTable* CLMReader::readLigatures(BinaryReader& reader) {
  const u16 glyph = reader.read<u16>();
  const i32 liga = reader.read<i32>();
  const u16 childCount = reader.read<u16>();
  auto* t = new LigaTable(glyph, liga, childCount);
  for (u16 i = 0; i < childCount; i++) {
    t->child(i) = readLigatures(reader);
  }
  return t;
}

MathConsts* CLMReader::readMathConsts(BinaryReader& reader) {
  auto* consts = new MathConsts();
  for (i16& _field : consts->_fields) {
    _field = reader.read<i16>();
  }
  return consts;
}

KernRecord* CLMReader::readKerns(BinaryReader& reader) {
  const u16 count = reader.read<u16>();
  if (count == 0) return nullptr;
  auto* record = new KernRecord(count);
  for (u16 i = 0; i < count; i++) {
    record->_fields[i << 1] = reader.read<u16>();
    record->_fields[(i << 1) + 1] = reader.read<i16>();
  }
  return record;
}

Variants* CLMReader::readVariants(BinaryReader& reader) {
  const u16 count = reader.read<u16>();
  if (count == 0) return nullptr;
  auto* variants = new Variants(count);
  for (u16 i = 0; i < count; i++) {
    variants->_glyphs[i] = reader.read<u16>();
  }
  return variants;
}

GlyphAssembly* CLMReader::readGlyphAssembly(BinaryReader& reader) {
  const bool isPresent = reader.read<bool>();
  if (!isPresent) return nullptr;
  const u16 partCount = reader.read<u16>();
  auto* assembly = new GlyphAssembly(partCount);
  assembly->_italicsCorrection = reader.read<i16>();
  for (u16 i = 0; i < partCount; i++) {
    GlyphPart& part = assembly->_parts[i];
    part._glyph = reader.read<u16>();
    part._flags = reader.read<u16>();
    part._startConnectorLength = reader.read<u16>();
    part._endConnectorLength = reader.read<u16>();
    part._fullAdvance = reader.read<u16>();
  }
  return assembly;
}

Math* CLMReader::readMath(BinaryReader& reader) {
  Math* ptr = new Math(0);
  Math& math = *ptr;
  math._italicsCorrection = reader.read<i16>();
  math._topAccentAttachment = reader.read<i16>();
  // variants
  Variants* hv = readVariants(reader);
  math._horizontalVariants = hv == nullptr ? &Variants::empty : hv;
  Variants* vv = readVariants(reader);
  math._verticalVariants = vv == nullptr ? &Variants::empty : vv;
  Variants* ss = readVariants(reader);
  math._scriptsVariants = ss == nullptr ? &Variants::empty : ss;
  // glyph assembly
  GlyphAssembly* hg = readGlyphAssembly(reader);
  math._horizontalAssembly = hg == nullptr ? &GlyphAssembly::empty : hg;
  GlyphAssembly* vg = readGlyphAssembly(reader);
  math._verticalAssembly = vg == nullptr ? &GlyphAssembly::empty : vg;
  // kern record
  auto* record = new MathKernRecord(0);
  for (auto& field : record->_fields) {
    const u16 count = reader.read<u16>();
    if (count == 0) {
      field = &MathKern::empty;
      continue;
    }
    auto* kern = new MathKern(count);
    for (u16 i = 0; i < count; i++) {
      kern->_fields[i << 1] = reader.read<i16>();
      kern->_fields[(i << 1) + 1] = reader.read<i16>();
    }
    field = kern;
  }
  math._kernRecord = record;
  return ptr;
}

#ifdef HAVE_GLYPH_RENDER_PATH

Path* CLMReader::readPath(BinaryReader& reader) {
  // !NOT THREAD SAFE
  static i32 id = 0;

  const auto len = reader.read<u16>();
  if (len == 0) return nullptr;
  auto cmds = new PathCmd*[len];
  for (u16 i = 0; i < len; i++) {
    const auto cmd = reader.read<char>();
    const auto cnt = microtex::pathCmdArgsCount(cmd);
    auto args = new i16[cnt];
    for (u16 j = 0; j < cnt; j++) {
      args[j] = reader.read<i16>();
    }
    cmds[i] = new PathCmd(cmd, args);
  }
  return new Path(++id, len, cmds);
}

#else

void CLMReader::skipGlyphPath(BinaryReader& reader) {
  const auto len = reader.read<u16>();
  if (len == 0) return;
  for (u16 i = 0; i < len; i++) {
    const auto cmd = reader.read<char>();
    const auto cnt = microtex::pathCmdArgsCount(cmd);
    for (u16 j = 0; j < cnt; j++) {
      reader.read<i16>();
    }
  }
}

#endif  // HAVE_GLYPH_RENDER_PATH

Glyph* CLMReader::readGlyph(bool isMathFont, bool hasGlyphPath, BinaryReader& reader) {
  auto* glyph = new Glyph();
  // Metrics is required
  glyph->_metrics._width = reader.read<i16>();
  glyph->_metrics._height = reader.read<i16>();
  glyph->_metrics._depth = reader.read<i16>();
  // read kern record, optional
  auto kern = readKerns(reader);
  glyph->_kernRecord = kern == nullptr ? &KernRecord::empty : kern;
  // read math, optional
  glyph->_math = isMathFont ? readMath(reader) : &Math::empty;
#ifdef HAVE_GLYPH_RENDER_PATH
  // read path
  if (hasGlyphPath) {
    auto path = readPath(reader);
    glyph->_path = path == nullptr ? &Path::empty : path;
  } else {
    // The font does not have glyph paths, although we have the path render
    // capability
    glyph->_path = &Path::empty;
  }
#else
  // Need to skip the glyph paths, because we don't have the path render
  // capability
  if (hasGlyphPath) skipGlyphPath(reader);
#endif
  return glyph;
}

void CLMReader::readGlyphs(Otf& font, bool hasGlyphPath, BinaryReader& reader) {
  const u16 count = reader.read<u16>();
  auto** glyphs = new Glyph*[count];
  for (u16 i = 0; i < count; i++) {
    glyphs[i] = readGlyph(font._isMathFont, hasGlyphPath, reader);
  }
  font._glyphCount = count;
  font._glyphs = glyphs;
}

Otf* CLMReader::read(BinaryReader& reader) {
  // read format
  const auto c = reader.read<u8>();
  const auto l = reader.read<u8>();
  const auto m = reader.read<u8>();
  if (c != 'c' || l != 'l' || m != 'm') {
    throw ex_invalid_param("invalid clm data format");
  }
  const auto ver = reader.read<u16>();
  if (ver != CLM_VER_MAJOR) {
    throw ex_invalid_param(
      "clm data (with version: " + toString(ver) + ") does not match the required version ("
      + toString(CLM_VER_MAJOR) + ")!"
    );
  }
  const auto minor = reader.read<u8>();
  const bool hasGlyphPath = CLM_SUPPORT_GLYPH_PATH(minor);
#if GLYPH_RENDER_TYPE == GLYPH_RENDER_TYPE_PATH
  if (!hasGlyphPath) {
    throw ex_invalid_param("The given clm data does not have glyph paths.");
  }
#endif
  // read otf-spec
  Otf* font = new Otf();
  font->_hasGlyphPath = hasGlyphPath;
#ifdef HAVE_GLYPH_RENDER_PATH
  font->_hasGlyphPath = hasGlyphPath;
#else
  // We don't have the path render capability
  font->_hasGlyphPath = false;
#endif
  readMeta(*font, reader);
  readClassKernings(*font, reader);
  font->_ligatures = readLigatures(reader);
  font->_mathConsts = font->_isMathFont ? readMathConsts(reader) : nullptr;
  readGlyphs(*font, hasGlyphPath, reader);
  return font;
}

Otf* CLMReader::read(const char* clmFilePath) const {
  BinaryFileReader reader(clmFilePath);
  return read(reader);
}

Otf* CLMReader::read(size_t len, const u8* bytes) const {
  BinaryDataReader reader(len, bytes);
  return read(reader);
}

}  // namespace microtex
