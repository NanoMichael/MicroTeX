#ifndef LATEX_BOX_FACTORY_H
#define LATEX_BOX_FACTORY_H

#include "atom/atom.h"

namespace tex {

class Char;

struct CharFont;

class TeXFont;

class SymbolAtom;

/**
 * Responsible for creating a box containing a delimiter symbol that exists in
 * different sizes.
 */
class DelimiterFactory {
public:
  static sptr<Box> create(SymbolAtom& symbol, Environment& env, int size);

  /**
   * Create a delimiter with specified symbol name and min height
   *
   * @param symbol the name of the delimiter symbol
   * @param env the Environment in which to create the delimiter box
   * @param minHeight the minimum required total height of the box (height + depth).
   *
   * @return the box representing the delimiter variant that fits best
   *     according to the required minimum size.
   */
  static sptr<Box> create(const std::string& symbol, Environment& env, float minHeight);
};

/** Responsible for creating a box containing a delimiter symbol that exists in different sizes. */
class XLeftRightArrowFactory {
private:
  static sptr<Atom> MINUS;
  static sptr<Atom> LEFT;
  static sptr<Atom> RIGHT;

public:
  static sptr<Box> create(bool left, Environment& env, float width);

  static sptr<Box> create(Environment& env, float width);
};

}

#endif //LATEX_BOX_FACTORY_H
