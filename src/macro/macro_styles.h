#ifndef LATEX_MACRO_STYLES_H
#define LATEX_MACRO_STYLES_H

#include "macro/macro_decl.h"
#include "macro/macro.h"
#include "core/parser.h"
#include "core/formula.h"
#include "atom/atom_impl.h"

namespace tex {

inline macro(displaystyle) {
  auto g = Formula(tp, tp.getOverArgument(), false)._root;
  return sptrOf<StyleAtom>(TexStyle::display, g);
}

inline macro(scriptstyle) {
  auto g = Formula(tp, tp.getOverArgument(), false)._root;
  return sptrOf<StyleAtom>(TexStyle::script, g);
}

inline macro(textstyle) {
  auto g = Formula(tp, tp.getOverArgument(), false)._root;
  return sptrOf<StyleAtom>(TexStyle::text, g);
}

inline macro(scriptscriptstyle) {
  auto g = Formula(tp, tp.getOverArgument(), false)._root;
  return sptrOf<StyleAtom>(TexStyle::scriptScript, g);
}

}

#endif //LATEX_MACRO_STYLES_H
