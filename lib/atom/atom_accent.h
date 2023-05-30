#ifndef MICROTEX_ATOM_ACCENT_H
#define MICROTEX_ATOM_ACCENT_H

#include "atom/atom.h"
#include "atom/atom_char.h"

namespace microtex {

/** An atom representing another atom with an accent symbol above it */
class AccentedAtom : public Atom {
public:
  sptr<SymbolAtom> _accenter;
  sptr<Atom> _accentee;
  sptr<Atom> _base;

  bool _fakeAccent = false;
  bool _fitSize = false;

  void setupBase(const sptr<Atom>& base);

public:
  AccentedAtom() = delete;

  /**
   * Create an AccentedAtom from a base atom and an accent symbol defined by
   * its name
   *
   * @param base base atom
   * @param name name of the accent symbol to be put over the base atom
   * @param fitSize if accent fit the base atom's width
   * @param fake if is a fake accent
   */
  AccentedAtom(
    const sptr<Atom>& base,
    const std::string& name,
    bool fitSize = false,
    bool fake = false
  );

  AtomType leftType() const override {
    return _accentee == nullptr ? AtomType::ordinary : _accentee->leftType();
  }

  AtomType rightType() const override {
    return _accentee == nullptr ? AtomType::ordinary : _accentee->rightType();
  }

  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_ACCENT_H
