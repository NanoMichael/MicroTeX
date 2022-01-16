#ifndef LATEX_JSAPI_H
#define LATEX_JSAPI_H

#include "def.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned int CLATEX_WASM_API js_createTextLayout(
  const char* txt,
  const char* font
);

void CLATEX_WASM_API js_getTextLayoutBounds(unsigned int id, float* data);

void CLATEX_WASM_API js_releaseTextLayout(unsigned int id);

#ifdef __cplusplus
};
#endif

#endif //LATEX_JSAPI_H
