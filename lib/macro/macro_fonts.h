#ifndef MICROTEX_MACRO_FONTS_H
#define MICROTEX_MACRO_FONTS_H

#include "atom/atom_font.h"
#include "atom/atom_misc.h"
#include "macro/macro_decl.h"
#include "unimath/uni_font.h"
#include "utils/utf.h"

namespace microtex {

inline macro(text) {
  const auto atom = Formula(tp, args[1], false, false)._root;
  return sptrOf<FontStyleAtom>(FontStyle::rm, false, atom);
}

inline macro(textfont) {
  tp.skipWhiteSpace(1);
  const auto atom = Formula(tp, tp.getOverArgument(), false, tp.isMathMode())._root;
  const auto style = FontContext::mainFontStyleOf(args[0]);
  return sptrOf<FontStyleAtom>(style, tp.isMathMode(), atom);
}

inline sptr<Atom> _textfontnested(Parser& tp, Args& args, FontStyle style) {
  const auto atom = Formula(tp, args[1], false, false)._root;
  return sptrOf<FontStyleAtom>(style, false, atom, true);
}

inline macro(textit) {
  return _textfontnested(tp, args, FontStyle::it);
}

inline macro(textbf) {
  return _textfontnested(tp, args, FontStyle::bf);
}

inline macro(textsf) {
  return _textfontnested(tp, args, FontStyle::sf);
}

inline macro(texttt) {
  return _textfontnested(tp, args, FontStyle::tt);
}

inline macro(textrm) {
  return _textfontnested(tp, args, FontStyle::rm);
}

inline sptr<Atom> _mathfont(Parser& tp, Args& args, FontStyle style) {
  const auto atom = Formula(tp, args[1], false, tp.isMathMode())._root;
  return sptrOf<FontStyleAtom>(style, true, atom);
}

inline macro(mathfont) {
  return _mathfont(tp, args, FontContext::mathFontStyleOf(args[0]));
}

inline macro(Bbb) {
  return _mathfont(tp, args, FontStyle::bb);
}

inline macro(mathds) {
  return _mathfont(tp, args, FontStyle::bb);
}

inline macro(bold) {
  return _mathfont(tp, args, FontStyle::bf);
}

macro(intertext);

macro(addfont);

macro(mathversion);

}  // namespace microtex

#endif  // MICROTEX_MACRO_FONTS_H
