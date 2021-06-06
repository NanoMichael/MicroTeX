#include <cstdio>
#include <string>

#include "utils/utils.h"
#include "core/env.h"
#include "core/units.h"

using namespace std;
using namespace tex;

int main(int argc, char* argv[]) {
  if (argc < 4) {
    printf(
      "Usage:\n"
      "  <math font name> e.g.: xits\n"
      "  <font file path>\n"
      "  <clm file path>\n"
    );
    exit(1);
  }
  // load math font to context
  FontSpec spec = make_tuple(string(argv[1]), string(argv[2]), string(argv[3]));
  FontContext::addMathFont(spec);
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
    "1cc = %f\n"
    "1x8 = %f\n",
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
    Units::fsize(UnitType::cc, 1.f, env),
    Units::fsize(UnitType::x8, 1.f, env)
  );
  return 0;
}
