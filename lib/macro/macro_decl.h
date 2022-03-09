#ifndef TINYTEX_MACRO_DECL_H
#define TINYTEX_MACRO_DECL_H

#include "core/formula.h"
#include "core/parser.h"
#include "atom/atom.h"

namespace microtex {

#ifndef macro
#define macro(name) sptr<Atom> macro_##name(Parser& tp, Args& args)
#endif

}

#endif //TINYTEX_MACRO_DECL_H
