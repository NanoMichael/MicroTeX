#include "def.h"
#include "tinytex.h"
#include "render/render.h"
#include "graphic_wasm.h"
#include "utils/log.h"
#include "unimath/font_meta.h"

using namespace microtex;

#ifdef __cplusplus
extern "C" {
#endif

typedef void* FontMetaPtr;

FontMetaPtr TINYTEX_WASM_API tinytex_init(
  unsigned long len,
  const unsigned char* data
) {
  FontSrcData src{len, data};
  auto meta = TinyTeX::init(src);
  return new FontMeta(meta);
}

void TINYTEX_WASM_API tinytex_release() {
  TinyTeX::release();
}

bool TINYTEX_WASM_API tinytex_isInited() {
  return TinyTeX::isInited();
}

FontMetaPtr TINYTEX_WASM_API tinytex_addFont(
  unsigned long len,
  const unsigned char* data
) {
  FontSrcData src{len, data};
  auto meta = TinyTeX::addFont(src);
  return new FontMeta(meta);
}

const char* TINYTEX_WASM_API tinytex_getFontFamily(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  // no need to copy
  return meta->family.c_str();
}

const char* TINYTEX_WASM_API tinytex_getFontName(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  // no need to copy
  return meta->name.c_str();
}

bool TINYTEX_WASM_API tinytex_isMathFont(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  return meta->isMathFont;
}

void TINYTEX_WASM_API tinytex_releaseFontMeta(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  delete meta;
}

void TINYTEX_WASM_API tinytex_setDefaultMathFont(const char* name) {
  TinyTeX::setDefaultMathFont(name);
}

void TINYTEX_WASM_API tinytex_setDefaultMainFont(const char* name) {
  TinyTeX::setDefaultMainFont(name);
}

void* TINYTEX_WASM_API tinytex_parseRender(
  const char* tex,
  int width,
  float textSize,
  float lineSpace,
  unsigned int color
) {
#ifdef HAVE_LOG
  logv("parse: %s\n", tex);
#endif
  auto r = TinyTeX::parse(tex, width, textSize, lineSpace, color);
  return reinterpret_cast<void*>(r);
}

void TINYTEX_WASM_API tinytex_deleteRender(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  delete r;
}

void* TINYTEX_WASM_API tinytex_getDrawingData(void* render, int x, int y) {
  auto r = reinterpret_cast<Render*>(render);
  Graphics2D_wasm g2;
  r->draw(g2, x, y);
  return g2.getDrawingData();
}

void TINYTEX_WASM_API tinytex_freeDrawingData(void* data) {
  free(data);
}

bool TINYTEX_WASM_API tinytex_isLittleEndian() {
  int n = 1;
  return *((char*) &n) == 1;
}

int TINYTEX_WASM_API tinytex_getRenderWidth(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getWidth();
}

int TINYTEX_WASM_API tinytex_getRenderHeight(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getHeight();
}

int TINYTEX_WASM_API tinytex_getRenderDepth(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getDepth();
}

int main(int argc, char** argv) {
#ifdef HAVE_LOG
  logv("tinytex entry run, is little endian: %d...\n", tinytex_isLittleEndian());
#endif
  auto factory = std::make_unique<PlatformFactory_wasm>();
  PlatformFactory::registerFactory("wasm", std::move(factory));
  PlatformFactory::activate("wasm");
  return 0;
}

#ifdef __cplusplus
};
#endif
