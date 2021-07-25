#include "macro/macro_delims.h"

namespace tex {

using namespace std;

macro(left) {
  wstring grep = tp.getGroup(L"\\left", L"\\right");

  auto left = Formula(tp, args[1], false)._root;
  auto* big = dynamic_cast<BigDelimiterAtom*>(left.get());
  if (big != nullptr) left = big->_delim;

  auto right = tp.getArgument();
  big = dynamic_cast<BigDelimiterAtom*>(right.get());
  if (big != nullptr) right = big->_delim;

  auto sl = dynamic_pointer_cast<SymbolAtom>(left);
  auto sr = dynamic_pointer_cast<SymbolAtom>(right);
  if (sl != nullptr && sr != nullptr) {
    Formula tf(tp, grep, false);
    return sptrOf<FencedAtom>(tf._root, sl, tf._middle, sr);
  }

  auto* ra = new RowAtom();
  ra->add(left);
  ra->add(Formula(tp, grep, false)._root);
  ra->add(right);

  return sptr<Atom>(ra);
}

}
