#ifndef TINYTEX_JSAPI_H
#define TINYTEX_JSAPI_H

#include "def.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned int TINYTEX_WASM_API js_createTextLayout(
  const char* txt,
  const char* font
);

void TINYTEX_WASM_API js_getTextLayoutBounds(unsigned int id, float* data);

void TINYTEX_WASM_API js_releaseTextLayout(unsigned int id);

#ifdef __cplusplus
};
#endif

#endif //TINYTEX_JSAPI_H
