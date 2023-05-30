#include "env/units.h"

#include <cstring>

#include "utils/string_utils.h"
#include "utils/utf.h"

using namespace microtex;
using namespace std;

namespace {

// IMPORTANT: sorted by the unit name, you must re-sort this array after add item
const pair<const char*, UnitType> _units[]{
  {"bp",    UnitType::point},
  {"cc",    UnitType::cc   },
  {"cm",    UnitType::cm   },
  {"dd",    UnitType::dd   },
  {"em",    UnitType::em   },
  {"ex",    UnitType::ex   },
  {"in",    UnitType::in   },
  {"mm",    UnitType::mm   },
  {"mu",    UnitType::mu   },
  {"pc",    UnitType::pica },
  {"pica",  UnitType::pica },
  {"pix",   UnitType::pixel},
  {"pixel", UnitType::pixel},
  {"pt",    UnitType::pt   },
  {"px",    UnitType::pixel},
  {"sp",    UnitType::sp   },
};

const u32 _unitsCount = sizeof(_units) / sizeof(pair<const char*, UnitType>);

/**
 * Helper function to get the size of 1 point (big-point) corresponds to
 * the font design unit.
 */
float pt(const Env& env) {
  return Env::pixelsPerPoint() * env.upem() / env.ppem() * env.fixedScale();
}

// IMPORTANT: the order corresponds to the order of the enum UnitType
const function<float(const Env&)> _unitConversions[]{
  // em
  [](const Env& env) -> float { return env.em(); },
  // ex
  [](const Env& env) -> float { return env.xHeight(); },
  // pixel
  [](const Env& env) -> float { return pt(env) / Env::pixelsPerPoint(); },
  // point
  [](const Env& env) -> float { return pt(env); },
  // 1 pica = 12 point
  [](const Env& env) -> float { return 12 * pt(env); },
  // 1 mu = 1/18 em
  [](const Env& env) -> float { return env.em() / 18.f; },
  // 1 cm = 28.346456693 point
  [](const Env& env) -> float { return 28.346456693f * pt(env); },
  // 1 mm = 1/10 cm
  [](const Env& env) -> float { return 2.8346456693f * pt(env); },
  // 1 in = 72 point
  [](const Env& env) -> float { return 72.f * pt(env); },
  // 1 sp = 65536 point
  [](const Env& env) -> float { return 65536.f * pt(env); },
  // 1 pt = 0.99626401 point (in printer's)
  [](const Env& env) -> float { return 0.99626401f * pt(env); },
  // 1 dd = 1.0660349422 point
  [](const Env& env) -> float { return 1.0660349422f * pt(env); },
  // 1 cc = 12 dd
  [](const Env& env) -> float { return 12.7924193070f * pt(env); },
  // 1 tt = rule thickness
  [](const Env& env) -> float { return env.ruleThickness() * env.scale(); },
};

}  // namespace

float Units::fsize(UnitType unit, float size, const Env& env) {
  if (unit == UnitType::none) return size;
  return _unitConversions[static_cast<i8>(unit)](env) * size;
}

float Units::fsize(const Dimen& dimen, const Env& env) {
  return Units::fsize(dimen.unit, dimen.val, env);
}

UnitType Units::getUnit(const std::string& unit) {
  const auto i =
    binIndexOf(_unitsCount, [&](int i) { return strcmp(unit.c_str(), _units[i].first); });
  if (i < 0) return UnitType::pixel;
  return _units[i].second;
}

Dimen Units::getDimen(const std::string& lgth) {
  if (lgth.empty()) return {0.f, UnitType::none};

  size_t i = 0;
  for (; i < lgth.length() && !isAlpha(lgth[i]); i++)
    ;
  float f = 0;
  valueOf(lgth.substr(0, i), f);

  UnitType unit = UnitType::none;
  string str = lgth.substr(i);
  string x = trim(str);
  toLower(x);
  if (i != lgth.size()) unit = getUnit(x);

  return {f, unit};
}
