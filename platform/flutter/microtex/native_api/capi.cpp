#include "microtex.h"
#include "dart_reg.h"
#include "graphic_flutter.h"

using namespace microtex;

#ifdef __cplusplus
extern "C" {
#endif

typedef void* FontMetaPtr;

void MICROTEX_EXPORT microtex_registerCallbacks(
  F_createTextLayout createTextLayout,
  F_getTextLayoutBounds getTextLayoutBounds,
  F_releaseTextLayout releaseTextLayout
) {
  dart_createTextLayout = createTextLayout;
  dart_getTextLayoutBounds = getTextLayoutBounds;
  dart_releaseTextLayout = releaseTextLayout;
}

FontMetaPtr MICROTEX_EXPORT microtex_init(
  unsigned long len,
  const unsigned char* data
) {
  auto factory = std::make_unique<PlatformFactory_flutter>();
  PlatformFactory::registerFactory("flutter", std::move(factory));
  PlatformFactory::activate("flutter");
  FontSrcData src{len, data};
  auto meta = MicroTeX::init(src);
  return new FontMeta(meta);
}

void MICROTEX_EXPORT microtex_release() {
  MicroTeX::release();
}

bool MICROTEX_EXPORT microtex_isInited() {
  return MicroTeX::isInited();
}

FontMetaPtr MICROTEX_EXPORT microtex_addFont(
  unsigned long len,
  const unsigned char* data
) {
  FontSrcData src{len, data};
  auto meta = MicroTeX::addFont(src);
  return new FontMeta(meta);
}

const char* MICROTEX_EXPORT microtex_getFontFamily(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  // no need to copy
  return meta->family.c_str();
}

const char* MICROTEX_EXPORT microtex_getFontName(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  // no need to copy
  return meta->name.c_str();
}

bool MICROTEX_EXPORT microtex_isMathFont(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  return meta->isMathFont;
}

void MICROTEX_EXPORT microtex_releaseFontMeta(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  delete meta;
}

void MICROTEX_EXPORT microtex_setDefaultMathFont(const char* name) {
  MicroTeX::setDefaultMathFont(name);
}

void MICROTEX_EXPORT microtex_setDefaultMainFont(const char* name) {
  MicroTeX::setDefaultMainFont(name);
}

void* MICROTEX_EXPORT microtex_parseRender(
  const char* tex,
  int width,
  float textSize,
  float lineSpace,
  unsigned int color,
  bool fillWidth,
  bool enableOverrideTeXStyle,
  unsigned int texStyle
) {
#ifdef HAVE_LOG
  logv("parse: %s\n", tex);
#endif
  auto r = MicroTeX::parse(
    tex, width, textSize, lineSpace, color, fillWidth,
    {enableOverrideTeXStyle, static_cast<TexStyle>(texStyle)}
  );
  return reinterpret_cast<void*>(r);
}

void MICROTEX_EXPORT microtex_deleteRender(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  delete r;
}

void* MICROTEX_EXPORT microtex_getDrawingData(void* render, int x, int y) {
  auto r = reinterpret_cast<Render*>(render);
  Graphics2D_flutter g2;
  r->draw(g2, x, y);
  return g2.getDrawingData();
}

void MICROTEX_EXPORT microtex_freeDrawingData(void* data) {
  free(data);
}

bool MICROTEX_EXPORT microtex_isLittleEndian() {
  int n = 1;
  return *((char*) &n) == 1;
}

int MICROTEX_EXPORT microtex_getRenderWidth(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getWidth();
}

int MICROTEX_EXPORT microtex_getRenderHeight(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getHeight();
}

int MICROTEX_EXPORT microtex_getRenderDepth(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getDepth();
}

bool MICROTEX_EXPORT microtex_isRenderSplit(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->isSplit();
}

void MICROTEX_EXPORT microtex_setRenderTextSize(void* render, float size) {
  auto r = reinterpret_cast<Render*>(render);
  r->setTextSize(size);
}

void MICROTEX_EXPORT microtex_setRenderForeground(void* render, color c) {
  auto r = reinterpret_cast<Render*>(render);
  r->setForeground(c);
}

#ifdef __cplusplus
}
#endif
