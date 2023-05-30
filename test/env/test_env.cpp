#include <cstdio>
#include <string>

#include "env/env.h"
#include "env/units.h"
#include "utils/utils.h"

using namespace std;
using namespace microtex;

void show_char(c32 code, const Env& env, bool isMathMode = true) {
  const auto&& chr = env.getChar(code, isMathMode);
  std::u32string str;
  str.append(1, chr.mappedCode);
  printf(
    "font style: %hx: "
    "{ code: %X, map: %X, font: %d, glyph: %d, scale: %f }\n",
    static_cast<u16>(env.mathFontStyle()),
    chr.code,
    chr.mappedCode,
    chr.fontId,
    chr.glyphId,
    chr.scale
  );
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(
      stderr,
      "Usage:\n"
      "  <clm file path>\n"
      "  <font file path>\n"
    );
    exit(1);
  }
  // load math font to context
  FontSrcFile src{string(argv[1]), string(argv[2])};
  FontContext::addFont(src);
  // create a font context and env
  auto fc = sptrOf<FontContext>();
  fc->selectMathFont(argv[1]);
  Env env(TexStyle::display, fc, 20.f);
  // test unit conversions
  printf("varies length in font design unit of 20 point-size:\n");
  printf(
    "1em = %f\n"
    "1ex = %f\n"
    "1px = %f\n"
    "1bp = %f\n"
    "1pc = %f\n"
    "1mu = %f\n"
    "1cm = %f\n"
    "1mm = %f\n"
    "1in = %f\n"
    "1sp = %f\n"
    "1pt = %f\n"
    "1dd = %f\n"
    "1cc = %f\n",
    Units::fsize(UnitType::em, 1.f, env),
    Units::fsize(UnitType::ex, 1.f, env),
    Units::fsize(UnitType::pixel, 1.f, env),
    Units::fsize(UnitType::point, 1.f, env),
    Units::fsize(UnitType::pica, 1.f, env),
    Units::fsize(UnitType::mu, 1.f, env),
    Units::fsize(UnitType::cm, 1.f, env),
    Units::fsize(UnitType::mm, 1.f, env),
    Units::fsize(UnitType::in, 1.f, env),
    Units::fsize(UnitType::sp, 1.f, env),
    Units::fsize(UnitType::pt, 1.f, env),
    Units::fsize(UnitType::dd, 1.f, env),
    Units::fsize(UnitType::cc, 1.f, env)
  );
  printf("\n");

  env.setStyle(TexStyle::scriptScript);
  printf("varies char with TexStyle::scriptScript in 20 point size:\n");
  show_char('a', env);
  show_char('h', env);
  show_char('z', env);
  show_char('{', env);
  show_char(0x1d53a, env);
  printf("\n");

  env.addMathFontStyle(FontStyle::rm);
  env.addMathFontStyle(FontStyle::bf);
  env.addMathFontStyle(FontStyle::it);
  env.addMathFontStyle(FontStyle::cal);
  env.addMathFontStyle(FontStyle::frak);
  env.addMathFontStyle(FontStyle::bb);
  env.addMathFontStyle(FontStyle::sf);
  env.addMathFontStyle(FontStyle::tt);
  printf("varies char with TexStyle::scriptScript and full FontStyle in 20 point size:\n");
  show_char('a', env);
  show_char('h', env);
  show_char('z', env);
  show_char('{', env);
  printf("\n");
  return 0;
}
