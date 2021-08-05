#ifndef LATEX_MACRO_DELIMS_H
#define LATEX_MACRO_DELIMS_H

#include "macro/macro_decl.h"
#include "atom/atom.h"
#include "atom/atom_misc.h"
#include "atom/atom_basic.h"
#include "atom/atom_delim.h"
#include "atom/atom_root.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

namespace tex {

inline macro(overdelim) {
  const auto& name = wide2utf8(args[0]);
  const auto& base = Formula(tp, args[1], false)._root;
  return sptrOf<OverUnderDelimiter>(base, name, true);
}

inline macro(underdelim) {
  const auto& name = wide2utf8(args[0]);
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
  std::wstring str(args[1]);
  replaceall(str, L"\\|", L"\\middle\\vert ");
  return Formula(tp, L"\\left\\langle " + str + L"\\right\\rangle")._root;
}

inline macro(Set) {
  std::wstring str(args[1]);
  replacefirst(str, L"\\|", L"\\middle\\vert ");
  return Formula(tp, L"\\left\\{" + str + L"\\right\\}")._root;
}

inline macro(leftparenthesis) {
  std::wstring grp = tp.getGroup(L"\\(", L"\\)");
  return sptrOf<MathAtom>(Formula(tp, grp, false)._root, TexStyle::text);
}

inline macro(leftbracket) {
  std::wstring grp = tp.getGroup(L"\\[", L"\\]");
  return sptrOf<MathAtom>(Formula(tp, grp, false)._root, TexStyle::display);
}

inline macro(middle) {
  return sptrOf<MiddleAtom>(wide2utf8(args[1]));
}

inline macro(sqrt) {
  if (args[2].empty()) return sptrOf<NthRoot>(Formula(tp, args[1], false)._root, nullptr);
  return sptrOf<NthRoot>(
    Formula(tp, args[1], false)._root,
    Formula(tp, args[2], false)._root
  );
}


macro(left);

}

#endif //LATEX_MACRO_DELIMS_H
