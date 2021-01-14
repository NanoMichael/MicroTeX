#ifndef SYMBOL_DEF_H_INCLUDED
#define SYMBOL_DEF_H_INCLUDED

#include "fonts/fonts.h"
#include "fonts/symbol_reg.h"

#define __len(x) ((int)(sizeof(x) / sizeof((x)[0])))

#define DEF_SYMBOLS(name)      \
  void __symbols_reg(name)() { \
    const tex::__symbol_component x[] = {
#define END_DEF_SYMBOLS                        \
  }                                            \
  ;                                            \
  tex::DefaultTeXFont::__push_symbols(x, __len(x)); \
  }

/**
 * Define a symbol mapping.
 * 
 * Consists of 3 component:
 * 
 *      [font-id, code, symbol-name]
 */
#define E(fontId, code, name) \
  {fontId, code, #name},

#endif
