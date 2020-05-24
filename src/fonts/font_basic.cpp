#include "fonts/font_basic.h"

Char::Char(wchar_t c, const Font* f, int fc, const sptr<Metrics>& m)
    : _c(c), _fontCode(fc), _font(f), _m(m), _cf(new CharFont(_c, _fontCode)) {}

Extension::~Extension() {
  if (hasTop()) delete _top;
  if (hasMiddle()) delete _middle;
  if (hasRepeat()) delete _repeat;
  if (hasBottom()) delete _bottom;
}

#ifdef HAVE_LOG
namespace tex {
ostream& operator<<(ostream& os, const CharFont& font) {
  os << "CharFont { char: " << font._c
     << ", font: " << font._fontId
     << ", bold font: " << font._boldFontId
     << " }";
  return os;
}
}  // namespace tex
#endif
