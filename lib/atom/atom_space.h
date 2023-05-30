#ifndef MICROTEX_ATOM_SPACE_H
#define MICROTEX_ATOM_SPACE_H

#include "atom/atom.h"
#include "utils/utils.h"

namespace microtex {

/** An atom representing whitespace. */
class SpaceAtom : public Atom {
private:
  bool _isMathMode = true;
  // whether a hard space should be represented
  bool _blankSpace = false;
  // thin-mu-skip, med-mu-skip, thick-mu-skip
  SpaceType _blankType{};
  // dimensions
  float _width = 0, _height = 0, _depth = 0;
  // units of the dimensions
  UnitType _unit{};

public:
  explicit SpaceAtom(bool isMathMode = true) noexcept
      : _blankSpace(true), _isMathMode(isMathMode) {}

  explicit SpaceAtom(SpaceType type) noexcept : _blankSpace(true), _blankType(type) {}

  SpaceAtom(UnitType unit, float width, float height, float depth) noexcept
      : _unit(unit), _width(width), _height(height), _depth(depth) {}

  sptr<Box> createBox(Env& env) override;

  static sptr<SpaceAtom> empty() { return sptrOf<SpaceAtom>(UnitType::em, 0.f, 0.f, 0.f); }
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_SPACE_H
