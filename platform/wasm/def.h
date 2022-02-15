#ifndef TINYTEX_DEF_H
#define TINYTEX_DEF_H

#ifndef TINYTEX_WASM_API
#  ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#    define TINYTEX_WASM_API EMSCRIPTEN_KEEPALIVE
#  else
#    define TINYTEX_WASM_API
#  endif
#endif

#endif //TINYTEX_DEF_H
