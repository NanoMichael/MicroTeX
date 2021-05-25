#include "atom/atom_basic.h"
#include "core/core.h"
#include "core/formula.h"

using namespace std;
using namespace tex;

// IMPORTANT: sorted by the unit name, you must resort this array after add item
const pair<const char*, UnitType> SpaceAtom::_units[]{
  {"bp",    UnitType::point},
  {"cc",    UnitType::cc},
  {"cm",    UnitType::cm},
  {"dd",    UnitType::dd},
  {"em",    UnitType::em},
  {"ex",    UnitType::ex},
  {"in",    UnitType::in},
  {"mm",    UnitType::mm},
  {"mu",    UnitType::mu},
  {"pc",    UnitType::pica},
  {"pica",  UnitType::pica},
  {"pix",   UnitType::pixel},
  {"pixel", UnitType::pixel},
  {"pt",    UnitType::pt},
  {"px",    UnitType::pixel},
  {"sp",    UnitType::sp},
};

const i32 SpaceAtom::_unitsCount = sizeof(_units) / sizeof(pair<const char*, UnitType>);

// IMPORTANT: the order corresponds to the order of the enum UnitType
const function<float(const Environment&)> SpaceAtom::_unitConversions[]{
  // EM
  [](const Environment& env) -> float {
    return env.getTeXFont()->getEM(env.getStyle());
  },
  // EX
  [](const Environment& env) -> float {
    return env.getTeXFont()->getXHeight(env.getStyle(), env.getLastFontId());
  },
  //PIXEL
  [](const Environment& env) -> float {
    return 1.f / env.getSize();
  },
  // BP
  [](const Environment& env) -> float {
    return Formula::PIXELS_PER_POINT / env.getSize();
  },
  // PICA
  [](const Environment& env) -> float {
    return (12 * Formula::PIXELS_PER_POINT) / env.getSize();
  },
  // MU
  [](const Environment& env) -> float {
    const auto& tf = env.getTeXFont();
    return tf->getQuad(env.getStyle(), tf->getMuFontId()) / 18.f;
  },
  // CM
  [](const Environment& env) -> float {
    return (28.346456693f * Formula::PIXELS_PER_POINT) / env.getSize();
  },
  // MM
  [](const Environment& env) -> float {
    return (2.8346456693f * Formula::PIXELS_PER_POINT) / env.getSize();
  },
  // IN
  [](const Environment& env) -> float {
    return (72.f * Formula::PIXELS_PER_POINT) / env.getSize();
  },
  // SP
  [](const Environment& env) -> float {
    return (65536 * Formula::PIXELS_PER_POINT) / env.getSize();
  },
  // PT
  [](const Environment& env) -> float {
    return (.9962640099f * Formula::PIXELS_PER_POINT) / env.getSize();
  },
  // DD
  [](const Environment& env) -> float {
    return (1.0660349422f * Formula::PIXELS_PER_POINT) / env.getSize();
  },
  // CC
  [](const Environment& env) -> float {
    return (12.7924193070f * Formula::PIXELS_PER_POINT) / env.getSize();
  },
  // X8
  [](const Environment& env) -> float {
    return env.getTeXFont()->getDefaultRuleThickness(env.getStyle());
  },
};
