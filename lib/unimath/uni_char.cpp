#include "unimath/uni_char.h"

#include "unimath/uni_font.h"
#include "utils/log.h"

using namespace microtex;

Char::Char(c32 c, c32 mc, i32 fid, i32 gid, float s)
    : code(c), mappedCode(mc), fontId(fid), glyphId(gid), scale(s) {}

Char::Char(const Char& c)
    : code(c.code),
      mappedCode(c.mappedCode),
      fontId(c.fontId),
      glyphId(c.glyphId),
      scale(c.scale) {}

sptr<const OtfFont> Char::otfFont() const {
  return FontContext::getFont(fontId);
}

const Glyph* Char::glyph() const {
  auto font = FontContext::getFont(fontId);
  if (font == nullptr) {
#ifdef HAVE_LOG
    logv("There's no font was found with id = %d, use '?' instead.\n", fontId);
#endif
    return FontContext::getFont(0)->otf().glyphOfUnicode('?');
  }
  auto g = font->otf().glyph(glyphId);
  if (g == nullptr) {
#ifdef HAVE_LOG
    logv(
      "There's no glyph was found with (unicode = %u, id = %d), use '?' instead.\n",
      static_cast<std::uint32_t>(mappedCode),
      glyphId
    );
#endif
    return font->otf().glyphOfUnicode('?');
  }
  return g;
}

i32 Char::fallbackGlyphId() const {
  if (isValid()) return glyphId;
  auto font = FontContext::getFont(fontId);
  if (font == nullptr) {
    return FontContext::getFont(0)->otf().glyphId('?');
  }
  return font->otf().glyphId('?');
}

float Char::width() const {
  return glyph()->metrics().width() * scale;
}

float Char::height() const {
  return glyph()->metrics().height() * scale;
}

float Char::depth() const {
  return glyph()->metrics().depth() * scale;
}

float Char::italic() const {
  auto italic = glyph()->math().italicsCorrection();
  return italic == Otf::undefinedMathValue ? 0.f : italic * scale;
}

float Char::topAccentAttachment() const {
  auto t = glyph()->math().topAccentAttachment();
  return t == Otf::undefinedMathValue ? width() / 2.f : t * scale;
}

static Char
variant(const Char& chr, u32 index, const std::function<const Variants&(const Glyph*)>& f) {
  auto g = chr.glyph();
  const auto& v = f(g);
  if (v.isEmpty()) return chr;
  index = std::min<int>(index, v.count() - 1);
  // only changes the glyph version, other fields remain unchanged
  return {chr.code, chr.mappedCode, chr.fontId, v[index], chr.scale};
}

u16 Char::vLargerCount() const {
  return glyph()->math().verticalVariants().count();
}

Char Char::vLarger(u32 index) const {
  return variant(*this, index, [](const Glyph* g) -> const Variants& {
    return g->math().verticalVariants();
  });
}

u16 Char::hLargerCount() const {
  return glyph()->math().horizontalVariants().count();
}

Char Char::hLarger(u32 index) const {
  return variant(*this, index, [](const Glyph* g) -> const Variants& {
    return g->math().horizontalVariants();
  });
}

Char Char::script(u32 index) const {
  return variant(*this, index, [](const Glyph* g) -> const Variants& {
    return g->math().scriptsVariants();
  });
}

const GlyphAssembly& Char::vAssembly() const {
  return glyph()->math().verticalAssembly();
}

const GlyphAssembly& Char::hAssembly() const {
  return glyph()->math().horizontalAssembly();
}

const Char Char::assemblyPart(i32 id) const {
  return onlyGlyph(fontId, id, scale);
}
