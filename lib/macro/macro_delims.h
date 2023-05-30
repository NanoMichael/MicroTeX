#ifndef MICROTEX_MACRO_DELIMS_H
#define MICROTEX_MACRO_DELIMS_H

#include "atom/atom.h"
#include "atom/atom_basic.h"
#include "atom/atom_delim.h"
#include "atom/atom_fence.h"
#include "atom/atom_misc.h"
#include "atom/atom_root.h"
#include "macro/macro_decl.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

namespace microtex {

inline macro(overdelim) {
  const auto& name = args[0];
  const auto& base = Formula(tp, args[1], false)._root;
  return sptrOf<OverUnderDelimiter>(base, name, true);
}

inline macro(underdelim) {
  const auto& name = args[0];
  const auto& base = Formula(tp, args[1], false)._root;
  return sptrOf<OverUnderDelimiter>(base, name, false);
}

macro(xarrow);

inline macro(overline) {
  return sptrOf<OverUnderBar>(Formula(tp, args[1], false)._root, true);
}

inline macro(underline) {
  return sptrOf<OverUnderBar>(Formula(tp, args[1], false)._root, false);
}

inline macro(Braket) {
  std::string str(args[1]);
  replaceAll(str, "\\|", "\\middle\\vert ");
  return Formula(tp, "\\left\\langle " + str + "\\right\\rangle")._root;
}

inline macro(Set) {
  std::string str(args[1]);
  replaceFirst(str, "\\|", "\\middle\\vert ");
  return Formula(tp, "\\left\\{" + str + "\\right\\}")._root;
}

inline macro(leftparenthesis) {
  std::string grp = tp.getGroup("\\(", "\\)");
  return sptrOf<MathAtom>(Formula(tp, grp, false)._root, TexStyle::text);
}

inline macro(leftbracket) {
  std::string grp = tp.getGroup("\\[", "\\]");
  return sptrOf<MathAtom>(Formula(tp, grp, false)._root, TexStyle::display);
}

inline macro(middle) {
  return sptrOf<MiddleAtom>(args[1]);
}

inline macro(sqrt) {
  if (args[2].empty()) return sptrOf<NthRoot>(Formula(tp, args[1], false)._root, nullptr);
  return sptrOf<NthRoot>(Formula(tp, args[1], false)._root, Formula(tp, args[2], false)._root);
}

macro(left);

}  // namespace microtex

#endif  // MICROTEX_MACRO_DELIMS_H
