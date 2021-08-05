#ifndef LATEX_ATOM_FRAC_H
#define LATEX_ATOM_FRAC_H

#include "atom/atom.h"
#include "utils/enums.h"

namespace tex {

/** An atom representing a fraction */
class FractionAtom : public Atom {
private:
  // unit used for the thickness of the fraction line
  UnitType _unit = UnitType::none;
  // thickness of the fraction line
  float _thickness = 0;
  Alignment _numAlign = Alignment::center, _denomAlign = Alignment::center;
  // the atoms representing the numerator and denominator
  sptr<Atom> _num, _dnom;
  bool _rule = false;

  static inline Alignment checkAlign(Alignment align) {
    if (align == Alignment::left || align == Alignment::right) return align;
    return Alignment::center;
  }

public:
  FractionAtom(
    const sptr<Atom>& num, const sptr<Atom>& den, bool rule,
    UnitType unit = UnitType::none, float thickness = 0.f,
    Alignment numAlign = Alignment::center, Alignment denAlign = Alignment::center
  );

  AtomType leftType() const override {
    return AtomType::inner;
  }

  AtomType rightType() const override {
    return AtomType::inner;
  }

  sptr<Box> createBox(Env& env) override;

  __decl_clone(FractionAtom)
};

}

#endif //LATEX_ATOM_FRAC_H
