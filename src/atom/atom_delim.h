#ifndef LATEX_ATOM_DELIM_H
#define LATEX_ATOM_DELIM_H

#include "atom/atom.h"
#include "atom/atom_char.h"
#include "atom/atom_space.h"

namespace tex {

class OverUnderBar : public Atom {
private:
  sptr<Atom> _base;
  bool _over;

public:
  OverUnderBar() = delete;

  OverUnderBar(const sptr<Atom>& base, bool over)
    : _base(base), _over(over) {
    _type = AtomType::ordinary;
  }

  sptr<Box> createBox(Env& env) override;

  __decl_clone(OverUnderBar)
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
    : _base(base), _delim(std::move(delim)), _over(over) {
    _type = AtomType::inner;
    _limitsType = LimitsType::limits;
  }

  sptr<Box> createBox(Env& env) override;

  __decl_clone(OverUnderDelimiter)
};

}

#endif //LATEX_ATOM_DELIM_H
