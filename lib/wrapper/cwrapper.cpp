#ifdef HAVE_CWRAPPER

#include "wrapper/cwrapper.h"
#include "wrapper/graphic_wrapper.h"
#include "microtex.h"
#include "utils/log.h"

using namespace microtex;

#ifdef __cplusplus
extern "C" {
#endif

MICROTEX_EXPORT void microtex_registerCallbacks(
  F_createTextLayout createTextLayout,
  F_getTextLayoutBounds getTextLayoutBounds,
  F_releaseTextLayout releaseTextLayout,
  F_isPathExists isPathExists
) {
  microtex_createTextLayout = createTextLayout;
  microtex_getTextLayoutBounds = getTextLayoutBounds;
  microtex_releaseTextLayout = releaseTextLayout;
  microtex_isPathExists = isPathExists;
}

MICROTEX_EXPORT FontMetaPtr microtex_init(
  unsigned long len,
  const unsigned char* data
) {
  auto factory = std::make_unique<PlatformFactory_wrapper>();
  PlatformFactory::registerFactory("__wrapper__", std::move(factory));
  PlatformFactory::activate("__wrapper__");
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

MICROTEX_EXPORT RenderPtr microtex_parseRender(
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
  return reinterpret_cast<RenderPtr>(r);
}

MICROTEX_EXPORT void microtex_deleteRender(RenderPtr render) {
  auto r = reinterpret_cast<Render*>(render);
  delete r;
}

MICROTEX_EXPORT DrawingData microtex_getDrawingData(RenderPtr render, int x, int y) {
  auto r = reinterpret_cast<Render*>(render);
  Graphics2D_wrapper g2;
  r->draw(g2, x, y);
  return g2.getDrawingData();
}

MICROTEX_EXPORT void microtex_freeDrawingData(DrawingData data) {
  free(data);
}

MICROTEX_EXPORT bool microtex_isLittleEndian() {
  int n = 1;
  return *((char*) &n) == 1;
}

MICROTEX_EXPORT int microtex_getRenderWidth(RenderPtr render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getWidth();
}

MICROTEX_EXPORT int microtex_getRenderHeight(RenderPtr render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getHeight();
}

MICROTEX_EXPORT int microtex_getRenderDepth(RenderPtr render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getDepth();
}

MICROTEX_EXPORT bool microtex_isRenderSplit(RenderPtr render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->isSplit();
}

MICROTEX_EXPORT void microtex_setRenderTextSize(RenderPtr render, float size) {
  auto r = reinterpret_cast<Render*>(render);
  r->setTextSize(size);
}

MICROTEX_EXPORT void microtex_setRenderForeground(RenderPtr render, color c) {
  auto r = reinterpret_cast<Render*>(render);
  r->setForeground(c);
}

#ifdef __cplusplus
}
#endif

#endif //HAVE_CWRAPPER
