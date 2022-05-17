#ifndef MICROTEX_CWRAPPER_H
#define MICROTEX_CWRAPPER_H

#ifdef HAVE_CWRAPPER

#include "microtexexport.h"
#include "wrapper/callback.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* FontMetaPtr;
typedef void* RenderPtr;
typedef void* DrawingData;

MICROTEX_CAPI const char* microtex_version();

MICROTEX_CAPI void microtex_registerCallbacks(
  F_createTextLayout createTextLayout,
  F_getTextLayoutBounds getTextLayoutBounds,
  F_releaseTextLayout releaseTextLayout,
  F_isPathExists isPathExists
);

MICROTEX_CAPI void microtex_setTextLayoutBounds(
  TextLayoutBounds* b, float width, float height, float ascent
);

MICROTEX_CAPI bool microtex_isBold(FontDesc* desc);

MICROTEX_CAPI bool microtex_isItalic(FontDesc* desc);

MICROTEX_CAPI bool microtex_isSansSerif(FontDesc* desc);

MICROTEX_CAPI bool microtex_isMonospace(FontDesc* desc);

MICROTEX_CAPI float microtex_fontSize(FontDesc* desc);

MICROTEX_CAPI FontMetaPtr microtex_init(
  unsigned long len,
  const unsigned char* data
);

MICROTEX_CAPI void microtex_release();

MICROTEX_CAPI bool microtex_isInited();

MICROTEX_CAPI FontMetaPtr microtex_addFont(
  unsigned long len,
  const unsigned char* data
);

MICROTEX_CAPI const char* microtex_getFontFamily(FontMetaPtr ptr);

MICROTEX_CAPI const char* microtex_getFontName(FontMetaPtr ptr);

MICROTEX_CAPI bool microtex_isMathFont(FontMetaPtr ptr);

MICROTEX_CAPI void microtex_releaseFontMeta(FontMetaPtr ptr);

MICROTEX_CAPI void microtex_setDefaultMathFont(const char* name);

MICROTEX_CAPI void microtex_setDefaultMainFont(const char* name);

MICROTEX_CAPI RenderPtr microtex_parseRender(
  const char* tex,
  int width,
  float textSize,
  float lineSpace,
  unsigned int color,
  bool fillWidth,
  bool enableOverrideTeXStyle,
  unsigned int texStyle
);

MICROTEX_CAPI void microtex_deleteRender(RenderPtr render);

MICROTEX_CAPI DrawingData microtex_getDrawingData(RenderPtr render, int x, int y);

MICROTEX_CAPI void microtex_freeDrawingData(DrawingData data);

MICROTEX_CAPI bool microtex_isLittleEndian();

MICROTEX_CAPI int microtex_getRenderWidth(RenderPtr render);

MICROTEX_CAPI int microtex_getRenderHeight(RenderPtr render);

MICROTEX_CAPI int microtex_getRenderDepth(RenderPtr render);

MICROTEX_CAPI bool microtex_isRenderSplit(RenderPtr render);

MICROTEX_CAPI void microtex_setRenderTextSize(RenderPtr render, float size);

MICROTEX_CAPI void microtex_setRenderForeground(RenderPtr render, unsigned int c);

#ifdef __cplusplus
}
#endif

#endif //HAVE_CWRAPPER

#endif //MICROTEX_CWRAPPER_H
