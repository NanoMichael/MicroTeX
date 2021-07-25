#ifndef LATEX_MACRO_FONTS_H
#define LATEX_MACRO_FONTS_H

#include "macro/macro_decl.h"
#include "atom/atom_impl.h"
#include "atom/atom_font.h"
#include "unimath/uni_font.h"
#include "utils/utf.h"

namespace tex {

using namespace std;

inline macro(text) {
  const auto atom = Formula(tp, args[1], false, false)._root;
  return sptrOf<FontStyleAtom>(FontStyle::rm, false, atom);
}

inline macro(textfont) {
  tp.skipWhiteSpace(1);
  const auto atom = Formula(tp, tp.getOverArgument(), false, tp.isMathMode())._root;
  const auto style = FontContext::mainFontStyleOf(wide2utf8(args[0]));
  return sptrOf<FontStyleAtom>(style, tp.isMathMode(), atom);
}

inline sptr<Atom> _textfontnested(TeXParser& tp, Args& args, FontStyle style) {
  const auto atom = Formula(tp, args[1], false, false)._root;
  return sptrOf<FontStyleAtom>(style, false, atom);
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

inline sptr<Atom> _mathfont(TeXParser& tp, Args& args, FontStyle style) {
  const auto atom = Formula(tp, args[1], false, tp.isMathMode())._root;
  return sptrOf<FontStyleAtom>(style, true, atom);
}

inline macro(mathfont) {
  return _mathfont(tp, args, FontContext::mathFontStyleOf(wide2utf8(args[0])));
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

inline macro(intertext) {
  if (!tp.isArrayMode())
    throw ex_parse("Command \\intertext must used in array environment!");

  wstring str(args[1]);
  replaceall(str, L"^{\\prime}", L"\'");
  replaceall(str, L"^{\\prime\\prime}", L"\'\'");

  // TODO
  // auto ra = sptrOf<RomanAtom>(Formula(tp, str, false, false)._root);
  sptr<Atom> ra = nullptr;
  ra->_type = AtomType::interText;
  tp.addAtom(ra);
  tp.addRow();

  return nullptr;
}

}

#endif //LATEX_MACRO_FONTS_H
