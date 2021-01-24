#include "otf/otf.h"

#include "otf/clm.h"

using namespace std;

namespace tex {

pair<bool, i16> ClassKerning::operator()(u16 left, u16 right) const {
  const int li = binSearchIndex(_leftCount, [&](int i) { return left - _lefts[i << 1]; });
  if (li < 0) return {false, 0};
  const int ri = binSearchIndex(_rightCount, [&](int i) { return right - _rights[i << 1]; });
  if (ri < 0) return {false, 0};
  const int i = _lefts[(li << 1) + 1];
  const int j = _rights[(ri << 1) + 1];
  return {true, _table[i * _columnLength + j]};
}

ClassKerning::~ClassKerning() {
  if (_lefts != nullptr) delete _lefts;
  if (_rights != nullptr) delete _rights;
  if (_table != nullptr) delete _table;
}

Otf* Otf::fromFile(const char* filePath) {
  const CLMReader reader;
  return reader.read(filePath);
}

i32 Otf::glyphId(u32 codepoint) const {
  const int index = binSearchIndex(
    _unicodeCount,
    [&](int i) { return codepoint - _unicodes[i]; }  //
  );
  if (index < 0) return -1;
  return _unicodeGlyphs[index];
}

const Glyph* Otf::glyphOfUnicode(u32 codepoint) const {
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
  if (_unicodes != nullptr) delete[] _unicodes;
  if (_unicodeGlyphs != nullptr) delete[] _unicodeGlyphs;
  if (_mathConsts != nullptr) delete _mathConsts;
  if (_ligatures != nullptr) delete _ligatures;
  if (_classKernings != nullptr) {
    for (u16 i = 0; i < _classKerningCount; i++) delete _classKernings[i];
    delete[] _classKernings;
  }
  if (_glyphs != nullptr) {
    for (u16 i = 0; i < _glyphCount; i++) delete _glyphs[i];
    delete[] _glyphs;
  }
}

}  // namespace tex
