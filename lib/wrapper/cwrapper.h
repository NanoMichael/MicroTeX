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

MICROTEX_EXPORT const char* microtex_version();

MICROTEX_EXPORT void microtex_registerCallbacks(
  F_createTextLayout createTextLayout,
  F_getTextLayoutBounds getTextLayoutBounds,
  F_releaseTextLayout releaseTextLayout,
  F_isPathExists isPathExists
);

MICROTEX_EXPORT FontMetaPtr microtex_init(
  unsigned long len,
  const unsigned char* data
);

MICROTEX_EXPORT void microtex_release();

MICROTEX_EXPORT bool microtex_isInited();

MICROTEX_EXPORT FontMetaPtr microtex_addFont(
  unsigned long len,
  const unsigned char* data
);

MICROTEX_EXPORT const char* microtex_getFontFamily(FontMetaPtr ptr);

MICROTEX_EXPORT const char* microtex_getFontName(FontMetaPtr ptr);

MICROTEX_EXPORT bool microtex_isMathFont(FontMetaPtr ptr);

MICROTEX_EXPORT void microtex_releaseFontMeta(FontMetaPtr ptr);

MICROTEX_EXPORT void microtex_setDefaultMathFont(const char* name);

MICROTEX_EXPORT void microtex_setDefaultMainFont(const char* name);

MICROTEX_EXPORT RenderPtr microtex_parseRender(
  const char* tex,
  int width,
  float textSize,
  float lineSpace,
  unsigned int color,
  bool fillWidth,
  bool enableOverrideTeXStyle,
  unsigned int texStyle
);

MICROTEX_EXPORT void microtex_deleteRender(RenderPtr render);

MICROTEX_EXPORT DrawingData microtex_getDrawingData(RenderPtr render, int x, int y);

MICROTEX_EXPORT void microtex_freeDrawingData(DrawingData data);

MICROTEX_EXPORT bool microtex_isLittleEndian();

MICROTEX_EXPORT int microtex_getRenderWidth(RenderPtr render);

MICROTEX_EXPORT int microtex_getRenderHeight(RenderPtr render);

MICROTEX_EXPORT int microtex_getRenderDepth(RenderPtr render);

MICROTEX_EXPORT bool microtex_isRenderSplit(RenderPtr render);

MICROTEX_EXPORT void microtex_setRenderTextSize(RenderPtr render, float size);

MICROTEX_EXPORT void microtex_setRenderForeground(RenderPtr render, unsigned int c);

#ifdef __cplusplus
}
#endif

#endif //HAVE_CWRAPPER

#endif //MICROTEX_CWRAPPER_H
