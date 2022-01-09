#ifndef LATEX_API_H
#define LATEX_API_H

#ifndef CLATEX_WASM_API
#  ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#    define CLATEX_WASM_API EMSCRIPTEN_KEEPALIVE
#  else
#    define CLATEX_WASM_API
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

void CLATEX_WASM_API add(
  unsigned long len,
  const unsigned char* data
);

void CLATEX_WASM_API clatex_init(
  const char* name,
  unsigned long len,
  const unsigned char* data
);

bool CLATEX_WASM_API clatex_isInited();

long CLATEX_WASM_API clatex_parse(
  const char* tex,
  int width,
  float textSize,
  float lineSpace,
  unsigned int color
);

void CLATEX_WASM_API clatex_delete(long render);

void CLATEX_WASM_API clatex_release();

#ifdef __cplusplus
};
#endif

#endif //LATEX_API_H
