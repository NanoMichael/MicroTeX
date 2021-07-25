#include "macro/macro_accent.h"

namespace tex {

macro(accentbiss) {
  std::string acc;
  switch (args[0][0]) {
    case '~':
      acc = "tilde";
      break;
    case '\'':
      acc = "acute";
      break;
    case '^':
      acc = "hat";
      break;
    case '\"':
      acc = "ddot";
      break;
    case '`':
      acc = "grave";
      break;
    case '=':
      acc = "bar";
      break;
    case '.':
      acc = "dot";
      break;
    case 'u':
      acc = "breve";
      break;
    case 'v':
      acc = "check";
      break;
    case 't':
      acc = "tie";
      break;
    case 'r':
      acc = "mathring";
      break;
  }

  return sptrOf<AccentedAtom>(Formula(tp, args[1], false)._root, acc);
}

}
