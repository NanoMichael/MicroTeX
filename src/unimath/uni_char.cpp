#include "unimath/uni_char.h"
#include "unimath/uni_font.h"

using namespace tex;

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

float Char::italic() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->math().italicsCorrection() * _scale;
}

float Char::topAccentAttachment() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->math().topAccentAttachment() * _scale;
}

static Char variant(const Char& chr, u32 index, std::function<const Variants&(const Glyph*)>&& f) {
  auto g = chr.glyph();
  if (g == nullptr) return Char(chr);
  const auto& v = f(g);
  if (index >= v.count()) return Char(chr);
  // only changes the glyph version, other fields remain unchanged
  return {chr._code, chr._mappedCode, chr._font, v[index], chr._scale};
}

u16 Char::vLargerCount() const {
  auto g = glyph();
  return g == nullptr ? 0 : g->math().verticalVariants().count();
}

Char Char::vLarger(u32 index) const {
  return variant(
    *this,
    index,
    [](const Glyph* g) -> const Variants& { return g->math().verticalVariants(); }
  );
}

u16 Char::hLargerCount() const {
  auto g = glyph();
  return g == nullptr ? 0 : g->math().horizontalVariants().count();
}

Char Char::hLarger(u32 index) const {
  return variant(
    *this,
    index,
    [](const Glyph* g) -> const Variants& { return g->math().horizontalVariants(); }
  );
}
