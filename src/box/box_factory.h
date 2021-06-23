#ifndef LATEX_BOX_FACTORY_H
#define LATEX_BOX_FACTORY_H

#include "atom/atom.h"

namespace tex {

class SymbolAtom;

/** Create a delimiter with given symbol and size */
sptr<Box> createDelim(SymbolAtom& sym, Env& env, int size);

/** Create a horizontal delimiter with given symbol and height */
sptr<Box> createHorDelim(const std::string& sym, Env& env, float height);

/** Create a vertical delimiter with given symbol and width */
sptr<Box> createVerDelim(const std::string& sym, Env& env, float width);

}

#endif //LATEX_BOX_FACTORY_H
