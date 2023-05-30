#include "otf/otf.h"

#include "otf/clm.h"

using namespace std;

namespace microtex {

pair<bool, i16> ClassKerning::operator()(u16 left, u16 right) const {
  const int li = binIndexOf(_leftCount, [&](int i) { return left - _lefts[i << 1]; });
  if (li < 0) return {false, 0};
  const int ri = binIndexOf(_rightCount, [&](int i) { return right - _rights[i << 1]; });
  if (ri < 0) return {false, 0};
  const int i = _lefts[(li << 1) + 1];
  const int j = _rights[(ri << 1) + 1];
  return {true, _table[i * _columnLength + j]};
}

ClassKerning::~ClassKerning() {
  delete _lefts;
  delete _rights;
  delete _table;
}

Otf* Otf::fromFile(const char* filePath) {
  const CLMReader reader;
  return reader.read(filePath);
}

Otf* Otf::fromData(size_t len, const u8* data) {
  const CLMReader reader;
  return reader.read(len, data);
}

u16 Otf::space() const {
  auto glyph = glyphOfUnicode(' ');
  if (glyph != nullptr) {
    return glyph->metrics().width();
  }
  // If no space glyph was found, we use the 1/3 em size
  return em() / 3;
}

i32 Otf::glyphId(c32 codepoint) const {
  const int index = binIndexOf(_unicodeCount, [&](int i) { return codepoint - _unicodes[i]; });
  if (index < 0) return -1;
  return _unicodeGlyphs[index];
}

const Glyph* Otf::glyphOfUnicode(c32 codepoint) const {
  const int id = glyphId(codepoint);
  if (id < 0) return nullptr;
  return _glyphs[id];
}

const Glyph* Otf::glyph(i32 id) const {
  if (id >= _glyphCount || id < 0) return nullptr;
  return _glyphs[id];
}

i16 Otf::classKerning(u16 left, u16 right) const {
  for (u16 i = 0; i < _classKerningCount; i++) {
    auto [found, value] = (*_classKernings[i])(left, right);
    if (found) return value;
  }
  return 0;
}

Otf::~Otf() {
  delete[] _unicodes;
  delete[] _unicodeGlyphs;
  delete _mathConsts;
  delete _ligatures;
  if (_classKernings != nullptr) {
    for (u16 i = 0; i < _classKerningCount; i++) delete _classKernings[i];
    delete[] _classKernings;
  }
  if (_glyphs != nullptr) {
    for (u16 i = 0; i < _glyphCount; i++) delete _glyphs[i];
    delete[] _glyphs;
  }
}

}  // namespace microtex
