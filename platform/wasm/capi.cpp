#include "latex.h"
#include "render.h"

#ifndef CLATEX_WASM_API
#  ifdef __EMSCRIPTEN__
#    include <emscripten.h>
#    define CLATEX_WASM_API EMSCRIPTEN_KEEPALIVE
#  else
#    define CLATEX_WASM_API
#  endif
#endif

using namespace tex;

#ifdef __cplusplus
extern "C" {
#endif

void CLATEX_WASM_API clatex_init(
  const char* name,
  unsigned long len,
  const unsigned char* data
) {
  LaTeX::init(name, len, data);
}

void CLATEX_WASM_API clatex_release() {
  LaTeX::release();
}

bool CLATEX_WASM_API clatex_isInited() {
  return LaTeX::isInited();
}

void* CLATEX_WASM_API clatex_parseRender(
  const char* tex,
  int width,
  float textSize,
  float lineSpace,
  unsigned int color
) {
  auto r = LaTeX::parse(tex, width, textSize, lineSpace, color);
  return reinterpret_cast<void*>(r);
}

void CLATEX_WASM_API clatex_deleteRender(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  delete r;
}

void* CLATEX_WASM_API clatex_fuck() {
  void* m = malloc(sizeof(char) + sizeof(float));
  auto* c = (unsigned char*) m;
  *c = 33;
  auto* f = (float*) (c + 1);
  *f = 3.14159f;
  // printf("%u, %f\n", *((unsigned char*) m), *((float*) ((char*) m + 1)));
  printf("char: %p, %u, float: %p, %f\n", c, *c, f, *f);
  return m;
}

int main(int argc, char** argv) {
  printf("hello clatexmath, fuck\n");
  int n = 1;

  if (*(char*) &n == 1) {
    printf("fuck, is little endian\n");
  } else {
    printf("fuck, is big endian\n");
  }
  return 0;
}

#ifdef __cplusplus
};
#endif
