#ifndef MICROTEX_DEF_H
#define MICROTEX_DEF_H

#ifndef MICROTEX_WASM_API
#  ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#    define MICROTEX_WASM_API EMSCRIPTEN_KEEPALIVE
#  else
#    define MICROTEX_WASM_API
#  endif
#endif

#endif //MICROTEX_DEF_H
