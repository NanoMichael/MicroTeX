#ifndef TINYTEX_MACRO_STYLES_H
#define TINYTEX_MACRO_STYLES_H

#include "macro/macro_decl.h"
#include "macro/macro.h"
#include "core/parser.h"
#include "core/formula.h"
#include "atom/atom_misc.h"

namespace tinytex {

TexStyle texStyleOf(const std::string& str);

macro(everymath);

macro(texstyle);

macro(atexstyle);

}

#endif //TINYTEX_MACRO_STYLES_H
