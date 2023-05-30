#include "otf/glyph.h"

namespace microtex {

const KernRecord KernRecord::empty(0);

const Variants Variants::empty(0);

const GlyphAssembly GlyphAssembly::empty(0);

const MathKern MathKern::empty(0);

const MathKernRecord MathKernRecord::empty(0);

const Math Math::empty(0);

i16 KernRecord::operator[](u16 glyph) const {
  const int i = binIndexOf(_count, [&](int i) { return glyph - _fields[i << 1]; });
  return i < 0 ? 0 : (i16)_fields[(i << 1) + 1];
}

u16 MathKern::indexOf(i32 height) const {
  if (_count == 0) return 0;
  return binIndexOf(
    _count,
    [&](int i) { return height - _fields[(i << 1) + 1]; },
    true
  );
}

MathKernRecord::~MathKernRecord() {
  for (auto& _field : _fields) {
    if (_field != &MathKern::empty) delete _field;
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
#ifdef HAVE_GLYPH_RENDER_PATH
  if (_path != &Path::empty) delete _path;
#endif
}

}  // namespace microtex
