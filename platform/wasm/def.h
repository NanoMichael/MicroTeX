#ifndef LATEX_DEF_H
#define LATEX_DEF_H

#ifndef CLATEX_WASM_API
#  ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#    define CLATEX_WASM_API EMSCRIPTEN_KEEPALIVE
#  else
#    define CLATEX_WASM_API
#  endif
#endif

#endif //LATEX_DEF_H
