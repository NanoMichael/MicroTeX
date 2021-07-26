#include <cstring>
#include <cmath>
#include "env/units.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

using namespace tex;
using namespace std;

// IMPORTANT: sorted by the unit name, you must resort this array after add item
const pair<const char*, UnitType> Units::_units[]{
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

const u32 Units::_unitsCount = sizeof(_units) / sizeof(pair<const char*, UnitType>);

// IMPORTANT: the order corresponds to the order of the enum UnitType
const function<float(const Env&)> Units::_unitConversions[]{
  // em
  [](const Env& env) -> float {
    return env.em();
  },
  // ex
  [](const Env& env) -> float {
    return env.xHeight();
  },
  // pixel
  [](const Env& env) -> float {
    return pt(env) / Env::pixelsPerPoint();
  },
  // point
  [](const Env& env) -> float {
    return pt(env);
  },
  // 1 pica = 12 point
  [](const Env& env) -> float {
    return 12 * pt(env);
  },
  // 1 mu = 1/18 em
  [](const Env& env) -> float {
    return env.em() / 18.f;
  },
  // 1 cm = 28.346456693 point
  [](const Env& env) -> float {
    return 28.346456693f * pt(env);
  },
  // 1 mm = 1/10 cm
  [](const Env& env) -> float {
    return 2.8346456693f * pt(env);
  },
  // 1 in = 72 point
  [](const Env& env) -> float {
    return 72.f * pt(env);
  },
  // 1 sp = 65536 point
  [](const Env& env) -> float {
    return 65536.f * pt(env);
  },
  // 1 pt = 0.99626401 point (in printer's)
  [](const Env& env) -> float {
    return 0.99626401f * pt(env);
  },
  // 1 dd = 1.0660349422 point
  [](const Env& env) -> float {
    return 1.0660349422f * pt(env);
  },
  // 1 cc = 12 dd
  [](const Env& env) -> float {
    return 12.7924193070f * pt(env);
  },
  // x8
  [](const Env& env) -> float {
    return env.ruleThickness();
  }
};

float Units::pt(const Env& env) {
  return Env::pixelsPerPoint() * env.upem() / env.ppem() * env.fixedScale();
}

float Units::fsize(UnitType unit, float size, const Env& env) {
  if (unit == UnitType::none) return size;
  return _unitConversions[static_cast<i8>(unit)](env) * size;
}

UnitType Units::getUnit(const std::string& unit) {
  const auto i = binIndexOf(
    _unitsCount,
    [&](int i) { return strcmp(unit.c_str(), _units[i].first); }
  );
  if (i < 0) return UnitType::pixel;
  return _units[i].second;
}

std::pair<UnitType, float> Units::getLength(const std::string& lgth) {
  if (lgth.empty()) return {UnitType::em, 0.f};

  size_t i = 0;
  for (; i < lgth.length() && !isalpha(lgth[i]); i++);
  float f = 0;
  valueof(lgth.substr(0, i), f);

  UnitType unit = UnitType::pixel;
  string str = lgth.substr(i);
  string x = trim(str);
  tolower(x);
  if (i != lgth.size()) unit = getUnit(x);

  return {unit, f};
}

std::pair<UnitType, float> Units::getLength(const std::wstring& lgth) {
  const string str = wide2utf8(lgth);
  return getLength(str);
}
