#ifndef LATEX_ATOM_ACCENT_H
#define LATEX_ATOM_ACCENT_H

#include "atom/atom.h"
#include "atom/atom_char.h"

namespace tex {

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
   *
   * @throw ex_invalid_symbol_type if the symbol is not defined as An accent ('acc')
   * @throw ex_symbol_not_found if there's no symbol defined with the given name
   */
  AccentedAtom(
    const sptr<Atom>& base, const std::string& name,
    bool fitSize = false,
    bool fake = false
  );

  sptr<Box> createBox(Env& env) override;

  __decl_clone(AccentedAtom)
};

}

#endif //LATEX_ATOM_ACCENT_H
