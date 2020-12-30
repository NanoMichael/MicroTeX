#include "atom/atom_basic.h"
#include "core/core.h"
#include "core/formula.h"

using namespace std;
using namespace tex;

const map<string, UnitType> SpaceAtom::_units = {
  {"em", UnitType::em},
  {"ex", UnitType::ex},
  {"px", UnitType::pixel},
  {"pix", UnitType::pixel},
  {"pixel", UnitType::pixel},
  {"pt", UnitType::pt},
  {"bp", UnitType::point},
  {"pica", UnitType::pica},
  {"pc", UnitType::pica},
  {"mu", UnitType::mu},
  {"cm", UnitType::cm},
  {"mm", UnitType::mm},
  {"in", UnitType::in},
  {"sp", UnitType::sp},
  {"dd", UnitType::dd},
  {"cc", UnitType::cc},
};

const int SpaceAtom::_units_count = 14;

const function<float(const TeXEnvironment&)> SpaceAtom::_unitConversions[] = {
  // EM
  [](const TeXEnvironment& env) -> float {
    return env.getTeXFont()->getEM(env.getStyle());
  },
  // EX
  [](const TeXEnvironment& env) -> float {
    return env.getTeXFont()->getXHeight(env.getStyle(), env.getLastFontId());
  },
  //PIXEL
  [](const TeXEnvironment& env) -> float {
    return 1.f / env.getSize();
  },
  // BP
  [](const TeXEnvironment& env) -> float {
    return TeXFormula::PIXELS_PER_POINT / env.getSize();
  },
  // PICA
  [](const TeXEnvironment& env) -> float {
    return (12 * TeXFormula::PIXELS_PER_POINT) / env.getSize();
  },
  // MU
  [](const TeXEnvironment& env) -> float {
    auto tf = env.getTeXFont();
    return tf->getQuad(env.getStyle(), tf->getMuFontId()) / 18.f;
  },
  // CM
  [](const TeXEnvironment& env) -> float {
    return (28.346456693f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
  },
  // MM
  [](const TeXEnvironment& env) -> float {
    return (2.8346456693f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
  },
  // IN
  [](const TeXEnvironment& env) -> float {
    return (72.f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
  },
  // SP
  [](const TeXEnvironment& env) -> float {
    return (65536 * TeXFormula::PIXELS_PER_POINT) / env.getSize();
  },
  // PT
  [](const TeXEnvironment& env) -> float {
    return (.9962640099f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
  },
  // DD
  [](const TeXEnvironment& env) -> float {
    return (1.0660349422f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
  },
  // CC
  [](const TeXEnvironment& env) -> float {
    return (12.7924193070f * TeXFormula::PIXELS_PER_POINT) / env.getSize();
  },
  // X8
  [](const TeXEnvironment& env) -> float {
    return env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
  },
};
