#ifndef LATEX_ATOM_SPACE_H
#define LATEX_ATOM_SPACE_H

#include "atom/atom.h"
#include "box/box_group.h"
#include "utils/utils.h"

namespace tex {

/**
 * An atom representing whitespace. The dimension values can be set using different
 * unit types.
 */
class SpaceAtom : public Atom {
private:
  static const std::pair<const char*, UnitType> _units[];
  static const i32 _unitsCount;
  static const std::function<float(const Environment&)> _unitConversions[];

  // whether a hard space should be represented
  bool _blankSpace = false;
  // thin-mu-skip, med-mu-skip, thick-mu-skip
  SpaceType _blankType{};
  // dimensions
  float _width = 0, _height = 0, _depth = 0;
  // units of the dimensions
  UnitType _wUnit{}, _hUnit{}, _dUnit{};

public:
  SpaceAtom() noexcept: _blankSpace(true) {}

  explicit SpaceAtom(SpaceType type) noexcept
    : _blankSpace(true), _blankType(type) {}

  SpaceAtom(UnitType unit, float width, float height, float depth) noexcept
    : _wUnit(unit), _hUnit(unit), _dUnit(unit), _width(width), _height(height), _depth(depth) {}

  SpaceAtom(UnitType wu, float w, UnitType hu, float h, UnitType du, float d) noexcept
    : _wUnit(wu), _hUnit(hu), _dUnit(du), _width(w), _height(h), _depth(d) {}

  static UnitType getUnit(const std::string& unit);

  /** Get the scale factor from the given unit and environment */
  inline static float getFactor(UnitType unit, const Environment& env) {
    return _unitConversions[static_cast<i8>(unit)](env);
  }

  inline static float getSize(UnitType unit, float size, const Environment& env) {
    return _unitConversions[static_cast<i8>(unit)](env) * size;
  }

  sptr<Box> createBox(Environment& env) override;

  /**
   * Get the unit and length from given string. The string must be in the format: a number
   * following with the unit (e.g. 10px, 1cm, 8.2em, ...) or (UnitType::pixel, 0) will be returned.
   */
  static std::pair<UnitType, float> getLength(const std::string& lgth);

  /**
   * Get the unit and length from given string. The string must be in the format: a number
   * following with the unit (e.g. 10px, 1cm, 8.2em, ...) or (UnitType::pixel, 0) will be returned.
   */
  static std::pair<UnitType, float> getLength(const std::wstring& lgth);

  __decl_clone(SpaceAtom)
};

}

#endif //LATEX_ATOM_SPACE_H
