#ifndef LATEX_ATOM_SPACE_H
#define LATEX_ATOM_SPACE_H

#include "atom/atom.h"

namespace tex {

/**
 * An atom representing whitespace. The dimension values can be set using different
 * unit types.
 */
class SpaceAtom : public Atom {
private:
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

  sptr<Box> createBox(Env& env) override;

  static sptr<SpaceAtom> empty() {
    return sptrOf<SpaceAtom>(UnitType::em, 0.f, 0.f, 0.f);
  }
};

}

#endif //LATEX_ATOM_SPACE_H
