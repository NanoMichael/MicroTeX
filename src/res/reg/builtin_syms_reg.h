#ifndef BUILTIN_SYMS_REG_H_INCLUDED
#define BUILTIN_SYMS_REG_H_INCLUDED

#include "fonts/symbol_reg.h"

DECL_SYMBOLS_REG(base);
DECL_SYMBOLS_REG(amssymb);
DECL_SYMBOLS_REG(amsfonts);
DECL_SYMBOLS_REG(stmaryrd);
DECL_SYMBOLS_REG(special);

namespace tex {

DECL_SYMBOLS_SET(Builtin);

}  // namespace tex

#endif
