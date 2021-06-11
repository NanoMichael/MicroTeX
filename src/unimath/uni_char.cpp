#include "unimath/uni_char.h"
#include "unimath/uni_font.h"

using namespace tex;

inline bool Char::isValid() const {
  return _glyph >= 0;
}

const Glyph* Char::glyph() const {
  auto font = FontContext::getFont(_font);
  return font == nullptr ? nullptr : font->otf().glyph(_glyph);
}

float Char::width() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->metrics().width() * _scale;
}

float Char::height() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->metrics().height() * _scale;
}

float Char::depth() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->metrics().depth() * _scale;
}
