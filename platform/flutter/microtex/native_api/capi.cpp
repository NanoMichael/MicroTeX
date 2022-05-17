#include "microtex.h"
#include "dart_reg.h"
#include "graphic_flutter.h"

using namespace microtex;

#ifdef __cplusplus
extern "C" {
#endif

typedef void* FontMetaPtr;

MICROTEX_EXPORT void microtex_registerCallbacks(
  F_createTextLayout createTextLayout,
  F_getTextLayoutBounds getTextLayoutBounds,
  F_releaseTextLayout releaseTextLayout,
  F_isPathExists isPathExists
) {
  dart_createTextLayout = createTextLayout;
  dart_getTextLayoutBounds = getTextLayoutBounds;
  dart_releaseTextLayout = releaseTextLayout;
  dart_isPathExists = isPathExists;
}

MICROTEX_EXPORT FontMetaPtr microtex_init(
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

MICROTEX_EXPORT void microtex_release() {
  MicroTeX::release();
}

MICROTEX_EXPORT bool microtex_isInited() {
  return MicroTeX::isInited();
}

MICROTEX_EXPORT FontMetaPtr microtex_addFont(
  unsigned long len,
  const unsigned char* data
) {
  FontSrcData src{len, data};
  auto meta = MicroTeX::addFont(src);
  return new FontMeta(meta);
}

MICROTEX_EXPORT const char* microtex_getFontFamily(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  // no need to copy
  return meta->family.c_str();
}

MICROTEX_EXPORT const char* microtex_getFontName(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  // no need to copy
  return meta->name.c_str();
}

MICROTEX_EXPORT bool microtex_isMathFont(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  return meta->isMathFont;
}

MICROTEX_EXPORT void microtex_releaseFontMeta(FontMetaPtr ptr) {
  auto* meta = (FontMeta*) ptr;
  delete meta;
}

MICROTEX_EXPORT void microtex_setDefaultMathFont(const char* name) {
  MicroTeX::setDefaultMathFont(name);
}

MICROTEX_EXPORT void microtex_setDefaultMainFont(const char* name) {
  MicroTeX::setDefaultMainFont(name);
}

MICROTEX_EXPORT void* microtex_parseRender(
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

MICROTEX_EXPORT void microtex_deleteRender(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  delete r;
}

MICROTEX_EXPORT void* microtex_getDrawingData(void* render, int x, int y) {
  auto r = reinterpret_cast<Render*>(render);
  Graphics2D_flutter g2;
  r->draw(g2, x, y);
  return g2.getDrawingData();
}

MICROTEX_EXPORT void microtex_freeDrawingData(void* data) {
  free(data);
}

MICROTEX_EXPORT bool microtex_isLittleEndian() {
  int n = 1;
  return *((char*) &n) == 1;
}

MICROTEX_EXPORT int microtex_getRenderWidth(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getWidth();
}

MICROTEX_EXPORT int microtex_getRenderHeight(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getHeight();
}

MICROTEX_EXPORT int microtex_getRenderDepth(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getDepth();
}

MICROTEX_EXPORT bool microtex_isRenderSplit(void* render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->isSplit();
}

MICROTEX_EXPORT void microtex_setRenderTextSize(void* render, float size) {
  auto r = reinterpret_cast<Render*>(render);
  r->setTextSize(size);
}

MICROTEX_EXPORT void microtex_setRenderForeground(void* render, color c) {
  auto r = reinterpret_cast<Render*>(render);
  r->setForeground(c);
}

#ifdef __cplusplus
}
#endif
