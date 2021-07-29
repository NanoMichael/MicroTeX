#include "unimath/uni_char.h"
#include "unimath/uni_font.h"

using namespace tex;

sptr<const OtfFont> Char::otfFont() const {
  return FontContext::getFont(fontId);
}

const Glyph* Char::glyph() const {
  auto font = FontContext::getFont(fontId);
  return font == nullptr ? nullptr : font->otf().glyph(glyphId);
}

float Char::width() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->metrics().width() * scale;
}

float Char::height() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->metrics().height() * scale;
}

float Char::depth() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->metrics().depth() * scale;
}

float Char::italic() const {
  auto g = glyph();
  auto italic = g == nullptr ? 0 : g->math().italicsCorrection();
  return italic == Otf::undefinedMathValue ? 0.f : italic * scale;
}

float Char::topAccentAttachment() const {
  auto g = glyph();
  return g == nullptr ? 0.f : g->math().topAccentAttachment() * scale;
}

static Char variant(const Char& chr, u32 index, std::function<const Variants&(const Glyph*)>&& f) {
  auto g = chr.glyph();
  if (g == nullptr) return Char(chr);
  const auto& v = f(g);
  if (v.isEmpty()) return chr;
  index = std::min<int>(index, v.count() - 1);
  // only changes the glyph version, other fields remain unchanged
  return {chr.code, chr.mappedCode, chr.fontId, v[index], chr.scale};
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

Char Char::script(u32 index) const {
  return variant(
    *this,
    index,
    [](const Glyph* g) -> const Variants& { return g->math().scriptsVariants(); }
  );
}
