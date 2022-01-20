#include "def.h"
#include "latex.h"
#include "render.h"
#include "graphic_wasm.h"
#include "utils/log.h"

using namespace tex;

#ifdef __cplusplus
extern "C" {
#endif

void CLATEX_WASM_API clatex_init(
  const char* name,
  unsigned long len,
  const unsigned char* data
) {
  FontSrcData src{name, len, data};
  LaTeX::init(src);
}

void CLATEX_WASM_API clatex_release() {
  LaTeX::release();
}

bool CLATEX_WASM_API clatex_isInited() {
  return LaTeX::isInited();
}

void CLATEX_WASM_API clatex_addMathFont(
  const char* name,
  unsigned long len,
  const unsigned char* data
) {
  FontSrcData src{name, len, data};
  LaTeX::addMathFont(src);
}

void CLATEX_WASM_API clatex_addMainFont(
  const char* familyName,
  const char* styleName,
  unsigned long len,
  const unsigned char* data
) {
  auto src = std::make_unique<FontSrcData>(styleName, len, data);
  FontSrcList list;
  list.push_back(std::move(src));
  LaTeX::addMainFont(familyName, list);
}

void CLATEX_WASM_API clatex_setDefaultMathFont(const char* name) {
  LaTeX::setDefaultMathFont(name);
}

void CLATEX_WASM_API clatex_setDefaultMainFont(const char* name) {
  LaTeX::setDefaultMainFont(name);
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

int CLATEX_WASM_API clatex_getRenderWidth(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getWidth();
}

int CLATEX_WASM_API clatex_getRenderHeight(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getHeight();
}

int CLATEX_WASM_API clatex_getRenderDepth(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getDepth();
}

int main(int argc, char** argv) {
#ifdef HAVE_LOG
  logv("clatex entry run, is little endian: %d...\n", clatex_isLittleEndian());
#endif
  auto factory = std::make_unique<PlatformFactory_wasm>();
  PlatformFactory::registerFactory("wasm", std::move(factory));
  PlatformFactory::activate("wasm");
  return 0;
}

#ifdef __cplusplus
};
#endif
