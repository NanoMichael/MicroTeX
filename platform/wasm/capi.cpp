#include "def.h"
#include "microtex.h"
#include "render/render.h"
#include "graphic_wasm.h"
#include "utils/log.h"
#include "unimath/font_meta.h"

using namespace microtex;

#ifdef __cplusplus
extern "C" {
#endif

typedef void* FontMetaPtr;

FontMetaPtr MICROTEX_WASM_API microtex_init(
  unsigned long len,
  const unsigned char* data
) {
  FontSrcData src{len, data};
  auto meta = MicroTeX::init(src);
  return new FontMeta(meta);
}

void MICROTEX_WASM_API microtex_release() {
  MicroTeX::release();
}

bool MICROTEX_WASM_API microtex_isInited() {
  return MicroTeX::isInited();
}

FontMetaPtr MICROTEX_WASM_API microtex_addFont(
  unsigned long len,
  const unsigned char* data
) {
  FontSrcData src{len, data};
  auto meta = MicroTeX::addFont(src);
  return new FontMeta(meta);
}

const char* MICROTEX_WASM_API microtex_getFontFamily(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  // no need to copy
  return meta->family.c_str();
}

const char* MICROTEX_WASM_API microtex_getFontName(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  // no need to copy
  return meta->name.c_str();
}

bool MICROTEX_WASM_API microtex_isMathFont(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  return meta->isMathFont;
}

void MICROTEX_WASM_API microtex_releaseFontMeta(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  delete meta;
}

void MICROTEX_WASM_API microtex_setDefaultMathFont(const char* name) {
  MicroTeX::setDefaultMathFont(name);
}

void MICROTEX_WASM_API microtex_setDefaultMainFont(const char* name) {
  MicroTeX::setDefaultMainFont(name);
}

void* MICROTEX_WASM_API microtex_parseRender(
  const char* tex,
  int width,
  float textSize,
  float lineSpace,
  unsigned int color
) {
#ifdef HAVE_LOG
  logv("parse: %s\n", tex);
#endif
  auto r = MicroTeX::parse(tex, width, textSize, lineSpace, color);
  return reinterpret_cast<void*>(r);
}

void MICROTEX_WASM_API microtex_deleteRender(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  delete r;
}

void* MICROTEX_WASM_API microtex_getDrawingData(void* render, int x, int y) {
  auto r = reinterpret_cast<Render*>(render);
  Graphics2D_wasm g2;
  r->draw(g2, x, y);
  return g2.getDrawingData();
}

void MICROTEX_WASM_API microtex_freeDrawingData(void* data) {
  free(data);
}

bool MICROTEX_WASM_API microtex_isLittleEndian() {
  int n = 1;
  return *((char*) &n) == 1;
}

int MICROTEX_WASM_API microtex_getRenderWidth(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getWidth();
}

int MICROTEX_WASM_API microtex_getRenderHeight(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getHeight();
}

int MICROTEX_WASM_API microtex_getRenderDepth(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getDepth();
}

int main(int argc, char** argv) {
#ifdef HAVE_LOG
  logv("microtex entry run, is little endian: %d...\n", microtex_isLittleEndian());
#endif
  auto factory = std::make_unique<PlatformFactory_wasm>();
  PlatformFactory::registerFactory("wasm", std::move(factory));
  PlatformFactory::activate("wasm");
  return 0;
}

#ifdef __cplusplus
};
#endif
