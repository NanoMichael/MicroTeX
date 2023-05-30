#include "macro/macro_scripts.h"

#include "atom/atom_basic.h"
#include "atom/atom_scripts.h"

namespace microtex {

using namespace std;

macro(sideset) {
  auto l = Formula(tp, args[1])._root;
  auto r = Formula(tp, args[2])._root;
  auto op = Formula(tp, args[3])._root;
  if (op == nullptr) {
    auto in = sptrOf<CharAtom>('M', FontStyle::rm, true);
    op = sptrOf<PhantomAtom>(in, false, true, true);
  }
  op->_limitsType = LimitsType::limits;
  op->_type = AtomType::bigOperator;
  auto cl = dynamic_cast<CumulativeScriptsAtom*>(l.get());
  auto cr = dynamic_cast<CumulativeScriptsAtom*>(r.get());
  if (cl != nullptr) l = cl->getScriptsAtom();
  if (cr != nullptr) r = cr->getScriptsAtom();
  return sptrOf<SideSetsAtom>(op, l, r);
}

macro(prescript) {
  auto base = Formula(tp, args[3])._root;
  return sptrOf<ScriptsAtom>(base, Formula(tp, args[2])._root, Formula(tp, args[1])._root, false);
}

}  // namespace microtex
