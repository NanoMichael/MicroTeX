#include "unimath/uni_char.h"
#include "unimath/uni_font.h"

using namespace tex;

inline bool Char::isValid() const {
  return _glyph >= 0;
}

inline const Glyph* Char::glyph() const {
  auto font = FontContext::getFont(_font);
  return font == nullptr ? nullptr : font->otf().glyph(_glyph);
}

inline float Char::width() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->metrics().width() * _scale;
}

inline float Char::height() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->metrics().height() * _scale;
}

inline float Char::depth() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->metrics().depth() * _scale;
}
