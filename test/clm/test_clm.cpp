#include <cstdio>

#include "openmath/clm.h"
#include "openmath/otf.h"

tex::int32 find_liga(const tex::OTFFont* font, const char* txt) {
  auto p = font->ligatures();
  while (p != nullptr && *txt != '\0') {
    auto id = font->glyphId(*txt);
    p = (*p)[id];
    txt++;
  }
  return p == nullptr ? -1 : p->value();
}

void show_metrics(const tex::Glyph* glyph) {
  printf(
    "metrics: [%d, %d, %d]\n",
    glyph->metrics().width(),
    glyph->metrics().height(),
    glyph->metrics().depth()  //
  );
}

void show_glyph_assembly(const tex::GlyphAssembly& assembly, const char* dir) {
  printf("%s glyph assembly: italics correction: %d\n", dir, assembly.italicsCorrection());
  printf(
    "glyph | extender | start length | end length | full advance\n"
    "-----------------------------------------------------------\n"  //
  );
  for (tex::uint16 i = 0; i < assembly.partCount(); i++) {
    const tex::GlyphPart& part = assembly[i];
    printf(
      "%5u | %8d | %12u | %10u | %12u\n",
      part.glyph(),
      part.isExtender(),
      part.startConnectorLength(),
      part.endConnectorLength(),
      part.fullAdvance()  //
    );
  }
}

void show_math_kern(const tex::MathKern& kern) {
  if (kern.count() == 0) printf("absent");
  for (tex::uint16 i = 0; i < kern.count(); i++) {
    printf("(%d, %d), ", kern.correctionHeight(i), kern.value(i));
  }
  printf("\n");
}

void show_math_kerns(const tex::Math& math) {
  printf("math kern record:\n");
  const tex::MathKernRecord& record = math.kernRecord();
  printf("    top left: ");
  show_math_kern(record.topLeft());
  printf("   top right: ");
  show_math_kern(record.topRight());
  printf(" bottom left: ");
  show_math_kern(record.bottomLeft());
  printf("bottom right: ");
  show_math_kern(record.bottomRight());
}

void show_math(tex::OTFFont* font, const tex::Glyph* glyph) {
  const tex::Math& math = glyph->math();
  const tex::Variants& hv = math.horizontalVariants();
  printf("horizontal variants: ");
  for (tex::uint16 i = 0; i < hv.count(); i++) {
    printf("%u, ", hv[i]);
  }
  printf("\n");
  const tex::Variants& vv = math.verticalVariants();
  printf("vertical variants: ");
  for (tex::uint16 i = 0; i < vv.count(); i++) {
    printf("%u, ", vv[i]);
  }
  printf("\n");
  show_glyph_assembly(math.horizontalAssembly(), "horizontal");
  show_glyph_assembly(math.verticalAssembly(), "vertical");
  show_math_kerns(math);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("No clm file specified\n");
    exit(1);
  }
  tex::OTFFont* font = tex::OTFFont::fromFile(argv[1]);
  printf("font from %s\n", argv[1]);
  printf(
    "unicodes count: %u, glyphs count: %u, is math font: %d, em: %u\n",
    font->unicodesCount(),
    font->glyphsCount(),
    font->isMathFont(),
    font->em()  //
  );
  printf("\n");

  const tex::Glyph* glyph_f = font->glyphOfUnicode('f');
  printf("glyph info for 'f':\n");
  printf("glyph id: %d, \n", font->glyphId('f'));
  show_metrics(glyph_f);
  printf("kerning of [f, a]: %d\n", glyph_f->kernRecord()[font->glyphId('a')]);
  // glyph id of 2378 is the a.sc in XITS-Regular font
  printf("class kerning of [V, a.sc]: %d\n", font->classKerning(font->glyphId('V'), 2378));
  printf("ligatures:\n");
  printf(
    "   fj |   fa |  ffj | ffjk\n"
    "--------------------------\n"
    "%5d |%5d |%5d |%5d\n",
    find_liga(font, "fj"),
    find_liga(font, "fa"),
    find_liga(font, "ffj"),
    find_liga(font, "ffjk")  //
  );
  printf("\n");

  const tex::Glyph* brace_left = font->glyphOfUnicode('{');
  printf("glyph info for '{':\n");
  printf("glyph id: %d\n", font->glyphId('{'));
  show_metrics(brace_left);
  show_math(font, brace_left);
  printf("\n");

  const tex::Glyph* Tau = font->glyphOfUnicode(0x0393);
  printf("glyph info for 'Tau':\n");
  printf("glyph id: %d\n", font->glyphId(0x0393));
  show_metrics(Tau);
  show_math(font, Tau);
  printf("\n");

  delete font;
  return 0;
}
