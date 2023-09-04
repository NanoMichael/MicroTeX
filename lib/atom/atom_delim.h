#ifndef MICROTEX_ATOM_DELIM_H
#define MICROTEX_ATOM_DELIM_H

#include <utility>

#include "atom/atom.h"
#include "atom/atom_basic.h"
#include "atom/atom_char.h"
#include "atom/atom_space.h"

namespace microtex {

class OverUnderBar : public Atom {
private:
  sptr<Atom> _base;
  bool _over;

public:
  OverUnderBar() = delete;

  OverUnderBar(const sptr<Atom>& base, bool over) : _base(base), _over(over) {
    _type = AtomType::ordinary;
  }

  sptr<Box> createBox(Env& env) override;
};

/**
 * An atom representing another atom with a delimiter and a script above or
 * under it, with script and delimiter separated by a kerning
 */
class OverUnderDelimiter : public Atom {
private:
  sptr<Atom> _base;
  // whether the delimiter should be positioned above or under the base
  bool _over;
  std::string _delim;

public:
  OverUnderDelimiter() = delete;

  OverUnderDelimiter(const sptr<Atom>& base, std::string delim, bool over)
      : _delim(std::move(delim)), _over(over) {
    _base = base == nullptr ? sptrOf<EmptyAtom>() : base;
    _type = AtomType::inner;
    _limitsType = LimitsType::limits;
  }

  AtomType leftType() const override { return AtomType::inner; }

  AtomType rightType() const override { return AtomType::inner; }

  sptr<Box> createBox(Env& env) override;
};

}  // namespace microtex

#endif  // MICROTEX_ATOM_DELIM_H
