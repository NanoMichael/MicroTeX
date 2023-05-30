#ifndef MICROTEX_MACRO_FRAC_H
#define MICROTEX_MACRO_FRAC_H

#include "atom/atom_char.h"
#include "atom/atom_frac.h"
#include "macro/macro_decl.h"

namespace microtex {

macro(binom);

sptr<Atom> _choose(const std::string& left, const std::string& right, Parser& tp, Args& args);

inline macro(choose) {
  return _choose("lparen", "rparen", tp, args);
}

inline macro(brack) {
  return _choose("lbrack", "rbrack", tp, args);
}

inline macro(bangle) {
  return _choose("langle", "rangle", tp, args);
}

inline macro(brace) {
  return _choose("lbrace", "rbrace", tp, args);
}

macro(frac);

macro(above);

macro(atop);

macro(over);

macro(abovewithdelims);

macro(atopwithdelims);

macro(overwithdelims);

macro(cfrac);

macro(genfrac);

}  // namespace microtex

#endif  // MICROTEX_MACRO_FRAC_H
