#include <cstring>
#include "unimath/uni_symbol.h"

using namespace std;
using namespace tex;

static const Symbol static_symbols[]{
};

const i32 Symbol::_count = sizeof(static_symbols) / sizeof(Symbol);
const Symbol* Symbol::_symbols = static_symbols;

const Symbol* Symbol::get(const char* name) {
  const int i = binIndexOf(
    _count,
    [&](int i) { return strcmp(name, _symbols[i].name); }
  );
  if (i < 0) return nullptr;
  return &_symbols[i];
}
