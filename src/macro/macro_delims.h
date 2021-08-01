#ifndef LATEX_MACRO_DELIMS_H
#define LATEX_MACRO_DELIMS_H

#include "macro/macro_decl.h"
#include "atom/atom.h"
#include "atom/atom_impl.h"
#include "atom/atom_basic.h"
#include "atom/atom_delim.h"
#include "utils/string_utils.h"

namespace tex {

inline sptr<Atom> _overunder(
  TeXParser& tp,
  Args& args,
  const std::string& name,
  bool isOver
) {
  return sptrOf<OverUnderDelimiter>(Formula(tp, args[1], false)._root, name, isOver);
}

inline macro(overrightarrow) {
  return _overunder(tp, args, "Vec", true);
}

inline macro(overleftarrow) {
  return _overunder(tp, args, "overleftarrow", true);
}

inline macro(overleftrightarrow) {
  return _overunder(tp, args, "overleftrightarrow", true);
}

inline macro(underrightarrow) {
  return _overunder(tp, args, "underrightarrow", false);
}

inline macro(underleftarrow) {
  return _overunder(tp, args, "underleftarrow", false);
}

inline macro(underleftrightarrow) {
  return _overunder(tp, args, "underleftrightarrow", false);
}

inline macro(xleftarrow) {
  return sptrOf<XArrowAtom>(
    Formula(tp, args[1], false)._root,
    Formula(tp, args[2])._root,
    true
  );
}

inline macro(xrightarrow) {
  return sptrOf<XArrowAtom>(
    Formula(tp, args[1], false)._root,
    Formula(tp, args[2])._root,
    false
  );
}

inline macro(underbrace) {
  return _overunder(tp, args, "underbrace", false);
}

inline macro(overbrace) {
  return _overunder(tp, args, "overbrace", true);
}

inline macro(underbrack) {
  return _overunder(tp, args, "underbracket", false);
}

inline macro(overbrack) {
  return _overunder(tp, args, "overbracket", true);
}

inline macro(underparen) {
  return _overunder(tp, args, "underparen", false);
}

inline macro(overparen) {
  return _overunder(tp, args, "overparen", true);
}

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
  return sptrOf<MiddleAtom>(Formula(tp, args[1])._root);
}

inline sptr<Atom> _choose(
  const std::string& left, const std::string& right,
  TeXParser& tp, std::vector<std::wstring>& args
) {
  auto num = tp.popFormulaAtom();
  auto den = Formula(tp, tp.getOverArgument(), false)._root;
  if (num == nullptr || den == nullptr)
    throw ex_parse("Both numerator and denominator of choose can't be empty!");
  auto f = sptrOf<FractionAtom>(num, den, false);
  auto l = SymbolAtom::get(left);
  auto r = SymbolAtom::get(right);
  // modify its type to opening and closing
  l->_type = AtomType::opening;
  r->_type = AtomType::closing;
  return sptrOf<FencedAtom>(f, l, r);
}

inline macro(choose) {
  return _choose("lbrack", "rbrack", tp, args);
}

inline macro(brack) {
  return _choose("lsqbrack", "rsqbrack", tp, args);
}

inline macro(bangle) {
  return _choose("langle", "rangle", tp, args);
}

inline macro(brace) {
  return _choose("lbrace", "rbrace", tp, args);
}

macro(left);

}

#endif //LATEX_MACRO_DELIMS_H
