#include "macro/macro_fonts.h"
#include "utils/string_utils.h"
#include "latex.h"

namespace tex {

macro(intertext) {
  if (!tp.isArrayMode())
    throw ex_parse("Command \\intertext must used in array environment!");

  std::string str(args[1]);
  replaceall(str, "^{\\prime}", "\'");
  replaceall(str, "^{\\prime\\prime}", "\'\'");

  auto a = Formula(tp, str, false, false)._root;
  sptr<Atom> ra = sptrOf<FontStyleAtom>(FontStyle::rm, false, a);
  ra->_type = AtomType::interText;
  tp.addAtom(ra);
  tp.addRow();

  return nullptr;
}

macro(addmathfont) {
  FontContext::addMathFont({args[1], args[2], args[3]});
  return nullptr;
}

macro(addmainfont) {
  FontContext::addMainFont(args[1], {args[2], args[3], args[4]});
  return nullptr;
}

macro(mathversion) {
  auto mathStyle = MathStyle::TeX;
  const auto& options = parseOption(args[2]);
  const auto it = options.find("math-style");
  if (it != options.end()) {
    const auto& value = it->second;
    if (value == "TeX") {
      mathStyle = MathStyle::TeX;
    } else if (value == "ISO") {
      mathStyle = MathStyle::ISO;
    } else if (value == "French") {
      mathStyle = MathStyle::French;
    } else if (value == "upright") {
      mathStyle = MathStyle::upright;
    }
  }
  LaTeX::setDefaultMathFont(args[1]);
  return sptrOf<MathFontAtom>(mathStyle, args[1]);
}

}
