#include "macro/macro_styles.h"
#include "utils/utf.h"
#include "latex.h"

namespace tex {

TexStyle texStyleOf(const std::string& str) {
  TexStyle style = TexStyle::text;
  if (str == "displaystyle") {
    style = TexStyle::display;
  } else if (str == "textstyle") {
    style = TexStyle::text;
  } else if (str == "scriptstyle") {
    style = TexStyle::script;
  } else if (str == "scriptscriptstyle") {
    style = TexStyle::scriptScript;
  }
}

macro(everymath) {
  if (args[1].empty()) {
    LaTeX::overrideTexStyle(false);
  } else {
    TexStyle style = texStyleOf(args[1].substr(1));
    LaTeX::overrideTexStyle(true, style);
  }
  return nullptr;
}

macro(texstyle) {
  auto g = Formula(tp, tp.getOverArgument(), false)._root;
  g = g == nullptr ? sptrOf<EmptyAtom>() : g;
  TexStyle style = texStyleOf(args[0]);
  return sptrOf<StyleAtom>(style, g);
}

macro(atexstyle) {
  auto g = Formula(tp, args[1], false)._root;
  return sptrOf<AStyleAtom>(args[0], g);
}

}
