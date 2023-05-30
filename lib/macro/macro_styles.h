#ifndef MICROTEX_MACRO_STYLES_H
#define MICROTEX_MACRO_STYLES_H

#include "atom/atom_misc.h"
#include "core/formula.h"
#include "core/parser.h"
#include "macro/macro.h"
#include "macro/macro_decl.h"

namespace microtex {

TexStyle texStyleOf(const std::string& str);

macro(everymath);

macro(texstyle);

macro(atexstyle);

}  // namespace microtex

#endif  // MICROTEX_MACRO_STYLES_H
