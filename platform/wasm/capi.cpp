#include "latex.h"
#include "render.h"
#include "grapihc_wasm.h"
#include "utils/log.h"

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
#ifdef HAVE_LOG
  logv("parse: %s\n", tex);
#endif
  auto r = LaTeX::parse(tex, width, textSize, lineSpace, color);
  return reinterpret_cast<void*>(r);
}

void CLATEX_WASM_API clatex_deleteRender(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  delete r;
}

void* CLATEX_WASM_API clatex_getDrawingData(void* render, int x, int y) {
  auto r = reinterpret_cast<Render*>(render);
  Graphics2D_wasm g2;
  r->draw(g2, x, y);
  return g2.getDrawingData();
}

void CLATEX_WASM_API clatex_freeDrawingData(void* data) {
  free(data);
}

bool CLATEX_WASM_API clatex_isLittleEndian() {
  int n = 1;
  return *((char*) &n) == 1;
}

int main(int argc, char** argv) {
#ifdef HAVE_LOG
  logv("clatex entry run, is little endian: %d...\n", clatex_isLittleEndian());
#endif
  return 0;
}

#ifdef __cplusplus
};
#endif
