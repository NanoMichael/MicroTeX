#include "openmath/otf.h"

namespace tex {

template <>
const LigaTable LigaTable::empty(0, -1, 0);

int16 ClassKerning::operator()(uint16 left, uint16 right) const {
  const int li = binSearchIndex(_leftCount, [&](int i) { return left - _lefts[i << 1]; });
  if (li < 0) return 0;
  const int ri = binSearchIndex(_rightCount, [&](int i) { return right - _rights[i << 1]; });
  if (ri < 0) return 0;
  const int i = _lefts[li << 1 + 1], j = _rights[ri << 1 + 1];
  return _table[i * _columnLength + j];
}

ClassKerning::~ClassKerning() {
  if (_lefts != nullptr) delete _lefts;
  if (_rights != nullptr) delete _rights;
  if (_table != nullptr) delete _table;
}

}  // namespace tex
