#ifndef LATEX_MACRO_DELIMS_H
#define LATEX_MACRO_DELIMS_H

#include "macro/macro_decl.h"
#include "atom/atom_impl.h"
#include "atom/atom_basic.h"
#include "atom/atom.h"

namespace tex {

inline sptr <Atom> _big(
  TeXParser& tp,
  std::vector<std::wstring>& args,
  int size,
  AtomType type = AtomType::none
) {
  auto a = Formula(tp, args[1], false)._root;
  auto s = std::dynamic_pointer_cast<SymbolAtom>(a);
  if (s == nullptr) return a;
  auto t = sptrOf<BigDelimiterAtom>(s, size);
  if (type != AtomType::none) t->_type = type;
  return t;
}

inline macro(big) { return _big(tp, args, 1); }

inline macro(Big) { return _big(tp, args, 2); }

inline macro(bigg) { return _big(tp, args, 3); }

inline macro(Bigg) { return _big(tp, args, 4); }

inline macro(bigl) { return _big(tp, args, 1, AtomType::opening); }

inline macro(Bigl) { return _big(tp, args, 2, AtomType::opening); }

inline macro(biggl) { return _big(tp, args, 3, AtomType::opening); }

inline macro(Biggl) { return _big(tp, args, 4, AtomType::opening); }

inline macro(bigr) { return _big(tp, args, 1, AtomType::closing); }

inline macro(Bigr) { return _big(tp, args, 2, AtomType::closing); }

inline macro(biggr) { return _big(tp, args, 3, AtomType::closing); }

inline macro(Biggr) { return _big(tp, args, 4, AtomType::closing); }

}

#endif //LATEX_MACRO_DELIMS_H
