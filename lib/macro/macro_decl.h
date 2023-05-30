#ifndef MICROTEX_MACRO_DECL_H
#define MICROTEX_MACRO_DECL_H

#include "atom/atom.h"
#include "core/formula.h"
#include "core/parser.h"

namespace microtex {

#ifndef macro
#define macro(name) sptr<Atom> macro_##name(Parser& tp, Args& args)
#endif

}  // namespace microtex

#endif  // MICROTEX_MACRO_DECL_H
