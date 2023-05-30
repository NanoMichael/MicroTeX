#ifndef MICROTEX_ATOM_FRAC_H
#define MICROTEX_ATOM_FRAC_H

#include "atom/atom.h"
#include "env/units.h"

namespace microtex {

/** An atom representing a fraction */
class FracAtom : public Atom {
private:
  Dimen _thickness;
  Alignment _numAlign = Alignment::center, _dnomAlign = Alignment::center;
  // the atoms representing the numerator and denominator
  sptr<Atom> _num, _dnom;
  bool _rule = false;

  static inline Alignment checkAlign(Alignment align) {
    if (align == Alignment::left || align == Alignment::right) return align;
    return Alignment::center;
  }

public:
  FracAtom(const sptr<Atom>& num, const sptr<Atom>& den, bool rule, const Dimen& thickness = {});

  FracAtom(
    const sptr<Atom>& num,
    const sptr<Atom>& den,
    Alignment numAlign = Alignment::center,
    Alignment denAlign = Alignment::center
  );

  AtomType leftType() const override { return AtomType::inner; }

  AtomType rightType() const override { return AtomType::inner; }

  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_FRAC_H
