#include "atom/atom.h"
#include "atom/atom_basic.h"

using namespace microtex;

WrapAtom::WrapAtom(const sptr<Atom>& base) {
    if (base) {
      _type = base->_type;
      _base = base;
    } else {
      _type = AtomType::ordinary;
      _base = sptrOf<EmptyAtom>();
    }
}
