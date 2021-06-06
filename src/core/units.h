#ifndef LATEX_UNITS_H
#define LATEX_UNITS_H

#include "utils/utils.h"
#include "utils/enums.h"
#include "core/env.h"

namespace tex {

class Units final {
private:
  /** Unit name to type map */
  static const std::pair<const char*, UnitType> _units[];
  /** Functions to convert unit type to font design size */
  static const std::function<float(const Env&)> _unitConversions[];
  static const u32 _unitsCount;

  static float pt(const Env& env);

public:
  /** Convert the given size with unit to the font design size (in font unit) */
  static float fsize(UnitType unit, float size, const Env& env);

  /** Get the unit type from the given unit name */
  static UnitType getUnit(const std::string& unit);

  /**
   * Get the unit and length from given string. The string must be in the format: a number
   * following with the unit (e.g. 10px, 1cm, 8.2em, ...) or (UnitType::em, 0) will be returned.
   */
  static std::pair<UnitType, float> getLength(const std::string& lgth);

  /**
   * Get the unit and length from given string. The string must be in the format: a number
   * following with the unit (e.g. 10px, 1cm, 8.2em, ...) or (UnitType::em, 0) will be returned.
   */
  static std::pair<UnitType, float> getLength(const std::wstring& lgth);
};

}

#endif //LATEX_UNITS_H
