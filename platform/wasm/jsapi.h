#ifndef MICROTEX_JSAPI_H
#define MICROTEX_JSAPI_H

#include "def.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned int MICROTEX_WASM_API js_createTextLayout(
  const char* txt,
  const char* font
);

void MICROTEX_WASM_API js_getTextLayoutBounds(unsigned int id, float* data);

void MICROTEX_WASM_API js_releaseTextLayout(unsigned int id);

void MICROTEX_WASM_API js_isPathExists(unsigned int id);

#ifdef __cplusplus
};
#endif

#endif //MICROTEX_JSAPI_H
