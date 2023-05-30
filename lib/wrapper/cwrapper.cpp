#ifdef HAVE_CWRAPPER

#include "wrapper/cwrapper.h"

#include "microtex.h"
#include "utils/log.h"
#include "wrapper/graphic_wrapper.h"

using namespace microtex;

#ifdef __cplusplus
extern "C" {
#endif

MICROTEX_CAPI const char* microtex_version() {
  // No need to copy, [MicroTeX::version] returns a static string
  return MicroTeX::version().c_str();
}

MICROTEX_CAPI void microtex_registerCallbacks(
  CBCreateTextLayout createTextLayout,
  CBGetTextLayoutBounds getTextLayoutBounds,
  CBReleaseTextLayout releaseTextLayout,
  CBIsPathExists isPathExists
) {
  microtex_createTextLayout = createTextLayout;
  microtex_getTextLayoutBounds = getTextLayoutBounds;
  microtex_releaseTextLayout = releaseTextLayout;
  microtex_isPathExists = isPathExists;
}

MICROTEX_CAPI void
microtex_setTextLayoutBounds(TextLayoutBounds* b, float width, float height, float ascent) {
  b->width = width;
  b->height = height;
  b->ascent = ascent;
}

MICROTEX_CAPI bool microtex_isBold(FontDesc* desc) {
  return desc->isBold;
}

MICROTEX_CAPI bool microtex_isItalic(FontDesc* desc) {
  return desc->isItalic;
}

MICROTEX_CAPI bool microtex_isSansSerif(FontDesc* desc) {
  return desc->isSansSerif;
}

MICROTEX_CAPI bool microtex_isMonospace(FontDesc* desc) {
  return desc->isMonospace;
}

MICROTEX_CAPI float microtex_fontSize(FontDesc* desc) {
  return desc->fontSize;
}

MICROTEX_CAPI FontMetaPtr microtex_init(unsigned long len, const unsigned char* data) {
  auto factory = std::make_unique<PlatformFactory_wrapper>();
  PlatformFactory::registerFactory("__wrapper__", std::move(factory));
  PlatformFactory::activate("__wrapper__");
  FontSrcData src{len, data};
  auto meta = MicroTeX::init(src);
  return new FontMeta(meta);
}

MICROTEX_CAPI void microtex_release() {
  MicroTeX::release();
}

MICROTEX_CAPI bool microtex_isInited() {
  return MicroTeX::isInited();
}

MICROTEX_CAPI FontMetaPtr microtex_addFont(unsigned long len, const unsigned char* data) {
  FontSrcData src{len, data};
  auto meta = MicroTeX::addFont(src);
  // create a new FontMeta from heap
  // [microtex_releaseFontMeta] must be called after this object has no usages.
  return new FontMeta(meta);
}

MICROTEX_CAPI const char* microtex_getFontFamily(FontMetaPtr ptr) {
  auto* meta = (FontMeta*)ptr;
  // no need to copy
  return meta->family.c_str();
}

MICROTEX_CAPI const char* microtex_getFontName(FontMetaPtr ptr) {
  auto* meta = (FontMeta*)ptr;
  // no need to copy
  return meta->name.c_str();
}

MICROTEX_CAPI bool microtex_isMathFont(FontMetaPtr ptr) {
  auto* meta = (FontMeta*)ptr;
  return meta->isMathFont;
}

MICROTEX_CAPI void microtex_releaseFontMeta(FontMetaPtr ptr) {
  auto* meta = (FontMeta*)ptr;
  delete meta;
}

MICROTEX_CAPI void microtex_setDefaultMathFont(const char* name) {
  MicroTeX::setDefaultMathFont(name);
}

MICROTEX_CAPI void microtex_setDefaultMainFont(const char* name) {
  MicroTeX::setDefaultMainFont(name);
}

MICROTEX_CAPI bool microtex_hasGlyphPathRender() {
  return MicroTeX::hasGlyphPathRender();
}

MICROTEX_CAPI void microtex_setRenderGlyphUsePath(bool use) {
  return MicroTeX::setRenderGlyphUsePath(use);
}

MICROTEX_CAPI bool microtex_isRenderGlyphUsePath() {
  return MicroTeX::isRenderGlyphUsePath();
}

MICROTEX_CAPI RenderPtr microtex_parseRender(
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
    tex,
    width,
    textSize,
    lineSpace,
    color,
    fillWidth,
    {enableOverrideTeXStyle, static_cast<TexStyle>(texStyle)}
  );
  return reinterpret_cast<RenderPtr>(r);
}

MICROTEX_CAPI void microtex_deleteRender(RenderPtr render) {
  auto r = reinterpret_cast<Render*>(render);
  delete r;
}

MICROTEX_CAPI DrawingData microtex_getDrawingData(RenderPtr render, int x, int y) {
  auto r = reinterpret_cast<Render*>(render);
  Graphics2D_wrapper g2;
  r->draw(g2, x, y);
  return g2.getDrawingData();
}

MICROTEX_CAPI void microtex_freeDrawingData(DrawingData data) {
  free(data);
}

MICROTEX_CAPI bool microtex_isLittleEndian() {
  int n = 1;
  return *((char*)&n) == 1;
}

MICROTEX_CAPI int microtex_getRenderWidth(RenderPtr render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getWidth();
}

MICROTEX_CAPI int microtex_getRenderHeight(RenderPtr render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getHeight();
}

MICROTEX_CAPI int microtex_getRenderDepth(RenderPtr render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->getDepth();
}

MICROTEX_CAPI bool microtex_isRenderSplit(RenderPtr render) {
  auto r = reinterpret_cast<Render*>(render);
  return r->isSplit();
}

MICROTEX_CAPI void microtex_setRenderTextSize(RenderPtr render, float size) {
  auto r = reinterpret_cast<Render*>(render);
  r->setTextSize(size);
}

MICROTEX_CAPI void microtex_setRenderForeground(RenderPtr render, color c) {
  auto r = reinterpret_cast<Render*>(render);
  r->setForeground(c);
}

#ifdef __cplusplus
}
#endif

#endif  // HAVE_CWRAPPER
