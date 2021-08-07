#ifndef LATEX_ATOM_OPERATOR_H
#define LATEX_ATOM_OPERATOR_H

#include "atom/atom.h"

namespace tex {

/**
 * An atom representing a "big operator" (or an atom that acts as one) together
 * with its limits
 */
class OperatorAtom : public Atom {
private:
  sptr<Atom> _base, _under, _over;

public:
  OperatorAtom() = delete;

  /**
   * Create a new BigOperatorAtom from the given atoms. The default rules the
   * positioning of the limits will be applied.
   *
   * @param base atom representing the big operator
   * @param under atom representing the under limit
   * @param over atom representing the over limit
   */
  OperatorAtom(const sptr<Atom>& base, const sptr<Atom>& under, const sptr<Atom>& over)
    : _base(base), _under(under), _over(over) {
    _limitsType = _base->_limitsType;
  }

  sptr<Box> createBox(Env& env) override;

  AtomType leftType() const override {
    return _base->leftType();
  }

  AtomType rightType() const override {
    return _base->rightType();
  }
};

}

#endif //LATEX_ATOM_OPERATOR_H
