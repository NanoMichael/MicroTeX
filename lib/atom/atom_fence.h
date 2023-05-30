#ifndef MICROTEX_ATOM_FENCE_H
#define MICROTEX_ATOM_FENCE_H

#include "atom/atom_char.h"
#include "box/box_single.h"

namespace microtex {

/**
 * An atom representing a middle atom which must be rounded by a left and right
 * delimiter.
 */
class MiddleAtom : public Atom {
private:
  std::string _sym;

public:
  const sptr<Box> _placeholder;
  float _height = 0.f;

  MiddleAtom() = delete;

  explicit MiddleAtom(std::string sym) : _sym(std::move(sym)), _placeholder(StrutBox::empty()) {}

  sptr<Box> createBox(Env& env) override;
};

/**
 * An atom representing a base atom surrounded with delimiters that change their
 * size according to the height of the base
 */
class FencedAtom : public Atom {
private:
  // base atom
  sptr<Atom> _base;
  // delimiters
  std::string _l, _r;
  std::vector<sptr<MiddleAtom>> _m;

public:
  FencedAtom(const sptr<Atom>& b, std::string l, std::string r)
      : _base(b), _l(std::move(l)), _r(std::move(r)) {}

  FencedAtom(const sptr<Atom>& b, std::string l, std::string r, std::vector<sptr<MiddleAtom>> m)
      : _base(b), _l(std::move(l)), _r(std::move(r)), _m(std::move(m)) {}

  AtomType leftType() const override { return AtomType::inner; }

  AtomType rightType() const override { return AtomType::inner; }

  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_FENCE_H
