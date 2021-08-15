#include "macro/macro_styles.h"
#include "utils/utf.h"

namespace tex {

macro(texstyle) {
  auto g = Formula(tp, tp.getOverArgument(), false)._root;
  TexStyle style = TexStyle::text;
  if (args[0] == "displaystyle") {
    style = TexStyle::display;
  } else if (args[0] == "textstyle") {
    style = TexStyle::text;
  } else if (args[0] == "scriptstyle") {
    style = TexStyle::script;
  } else if (args[0] == "scriptscriptstyle") {
    style = TexStyle::scriptScript;
  }
  return sptrOf<StyleAtom>(style, g);
}

macro(atexstyle) {
  auto g = Formula(tp, args[1], false)._root;
  return sptrOf<AStyleAtom>(args[0], g);
}

}
