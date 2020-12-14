#include "openmath/glyph.h"

namespace tex {

const KernRecord KernRecord::empty(0);

const Variants Variants::empty(0);

const GlyphAssembly GlyphAssembly::empty(0);

const MathKern MathKern::empty(0);

const MathKernRecord MathKernRecord::empty(0);

const Math Math::empty;

template <>
const LigaTable LigaTable::empty(-1, -1, 0);

int16 KernRecord::operator[](uint16 glyph) const {
  const int i = binSearchIndex(_count, [&](int i) { return glyph - _fields[i << 1]; });
  return i < 0 ? 0 : _fields[i << 1 + 1];
}

uint16 MathKern::indexOf(int32 height) const {
  if (_count == 0) return 0;
  return binSearchIndex(
      _count,
      [&](int i) { return height - _fields[i << 1]; },
      true);
}

MathKernRecord::MathKernRecord(uint16 ignore) {
  for (std::size_t i = 0; i < 4; i++) _fields[i] = &MathKern::empty;
}

MathKernRecord::~MathKernRecord() {
  for (std::size_t i = 0; i < 4; i++) {
    if (_fields[i] != &MathKern::empty) delete _fields[i];
  }
}

Math::~Math() {
  if (_horizontalVariants != &Variants::empty) delete _horizontalVariants;
  if (_verticalVariants != &Variants::empty) delete _verticalVariants;
  if (_horizontalAssembly != &GlyphAssembly::empty) delete _horizontalAssembly;
  if (_verticalAssembly != &GlyphAssembly::empty) delete _verticalAssembly;
  if (_kernRecord != &MathKernRecord::empty) delete _kernRecord;
}

Glyph::~Glyph() {
  if (_kernRecord != &KernRecord::empty) delete _kernRecord;
  if (_math != &Math::empty) delete _math;
}

}  // namespace tex
