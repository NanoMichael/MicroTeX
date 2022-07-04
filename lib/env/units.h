#ifndef MICROTEX_UNITS_H
#define MICROTEX_UNITS_H

#include "utils/utils.h"
#include "env/env.h"

namespace microtex {

/** Represents a dimension */
struct Dimen {
  float val = 0.f;
  UnitType unit = UnitType::none;

  inline bool isValid() const { return unit != UnitType::none; }

  inline bool isEmpty() const { return !isValid() || val == 0.f; }

  inline Dimen& operator-() {
    val = -val;
    return *this;
  }
};

constexpr Dimen operator ""_ex(long double val) {
  return {static_cast<float>(val), UnitType::ex};
};

constexpr Dimen operator ""_em(long double val) {
  return {static_cast<float>(val), UnitType::em};
}

constexpr Dimen operator ""_pt(long double val) {
  return {static_cast<float>(val), UnitType::pt};
}

constexpr Dimen operator ""_tt(long double val) {
  return {static_cast<float>(val), UnitType::tt};
}

class Units final {
private:
  /** Unit name to type map */
  static const std::pair<const char*, UnitType> _units[];
  /** Functions to convert unit type to font design size */
  static const std::function<float(const Env&)> _unitConversions[];
  static const u32 _unitsCount;

  /**
   * Helper function to get the size of 1 point (big-point) corresponds to
   * the font design unit.
   */
  static float pt(const Env& env);

public:
  /** Convert the given size with unit to the font design size (in font unit) */
  static float fsize(UnitType unit, float size, const Env& env);

  /** Convert the given dimension to the font design size (in font unit) */
  static float fsize(const Dimen& dimen, const Env& env);

  /** Get the unit type from the given unit name */
  static UnitType getUnit(const std::string& unit);

  /**
   * Get the unit and length from given string. The string must be in the format: a number
   * following with the unit (e.g. 10px, 1cm, 8.2em, ...), or (UnitType::none, 0) will be
   * returned.
   */
  static Dimen getDimen(const std::string& lgth);
};

}

#endif //MICROTEX_UNITS_H
