/** A C-wrapper for microtex. */

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

/** Get the library version. */
MICROTEX_CAPI const char* microtex_version();

/**
 * Register callbacks to the engine.
 *
 * @param createTextLayout callback to create text-layout
 * @param getTextLayoutBounds callback to retrieve the bounds of text-layout created
 * by [createTextLayout]
 * @param releaseTextLayout callback to release a text-layout created by [createTextLayout]
 * @param isPathExists callback to check if a graphical-path is exists
 */
MICROTEX_CAPI void microtex_registerCallbacks(
  CBCreateTextLayout createTextLayout,
  CBGetTextLayoutBounds getTextLayoutBounds,
  CBReleaseTextLayout releaseTextLayout,
  CBIsPathExists isPathExists
);

/** Set text-layout bounds by given width, height, and ascent. For FFI bindings. */
MICROTEX_CAPI void
microtex_setTextLayoutBounds(TextLayoutBounds* b, float width, float height, float ascent);

/** Test if a font is bold. For FFI bindings. */
MICROTEX_CAPI bool microtex_isBold(FontDesc* desc);

/** Test if a font is italic. For FFI bindings. */
MICROTEX_CAPI bool microtex_isItalic(FontDesc* desc);

/** Test if a font is sans-serif. For FFI bindings. */
MICROTEX_CAPI bool microtex_isSansSerif(FontDesc* desc);

/** Test if a font is monospace. For FFI bindings. */
MICROTEX_CAPI bool microtex_isMonospace(FontDesc* desc);

/** Retrieve the font size. For FFI bindings. */
MICROTEX_CAPI float microtex_fontSize(FontDesc* desc);

/**
 * Initialize the engine by given `clm` data. The data must represents a math font.
 *
 * @param len the number of bytes of the `clm` data
 * @param data the `clm` data
 * @return the meta info of the font, you can retrieve the font family via
 * [microtex_getFontFamily], the font name via [microtex_getFontName], and to test if
 * the font is a math font by [microtex_isMathFont]. You must call [microtex_releaseFontMeta]
 * after this object has no usages.
 */
MICROTEX_CAPI FontMetaPtr microtex_init(unsigned long len, const unsigned char* data);

/** Release the engine. */
MICROTEX_CAPI void microtex_release();

/** Test if the engine is initialized. */
MICROTEX_CAPI bool microtex_isInited();

/**
 * Add a font to the engine context.
 *
 * @param len the number of bytes of the `clm` data
 * @param data the `clm` data
 * @return the font meta info. You must call [microtex_releaseFontMeta] after this object has
 * no usages.
 */
MICROTEX_CAPI FontMetaPtr microtex_addFont(unsigned long len, const unsigned char* data);

/** Get the font family name from the given FontMetaPtr. */
MICROTEX_CAPI const char* microtex_getFontFamily(FontMetaPtr ptr);

/** Get the font name from the given FontMetaPtr. */
MICROTEX_CAPI const char* microtex_getFontName(FontMetaPtr ptr);

/** Test if the given font is a math font. */
MICROTEX_CAPI bool microtex_isMathFont(FontMetaPtr ptr);

/** Release the font meta info. */
MICROTEX_CAPI void microtex_releaseFontMeta(FontMetaPtr ptr);

/**
 * Set the default math font by name.
 * See [lib/microtex.h: MicroTex::setDefaultMathFont] for details.
 */
MICROTEX_CAPI void microtex_setDefaultMathFont(const char* name);

/**
 * Set the default main font family by name.
 * See [lib/microtex.h: MicroTex::setDefaultMainFont] for details.
 */
MICROTEX_CAPI void microtex_setDefaultMainFont(const char* name);

/** Test if has the ability to use path to render glyphs. */
MICROTEX_CAPI bool microtex_hasGlyphPathRender();

/**
 * Set if use path to render glyphs, only works when compile option
 * GLYPH_RENDER_TYPE is GLYPH_RENDER_TYPE_BOTH (equals to 0, that means render
 * glyphs use font and path both), otherwise this function takes no effect.
 */
MICROTEX_CAPI void microtex_setRenderGlyphUsePath(bool use);

/**
 * Test if currently use path to render glyphs.
 *
 * See [lib/microtex.h: MicroTeX::isRenderGlyphUsePath] for details.
 */
MICROTEX_CAPI bool microtex_isRenderGlyphUsePath();

/**
 * Parse a (La)TeX string (in UTF-8 encoding) to Render. You must
 * call [microtex_deleteRender] after it has no usages.
 *
 * See [lib/microtex.h: MicroTeX::parse] for details.
 */
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

/** Delete the render created by [microtex_parseRender] before. */
MICROTEX_CAPI void microtex_deleteRender(RenderPtr render);

/**
 * Get the drawing data from the render created by [microtex_parseRender] before.
 * It generates the drawing commands from the render. You must call
 * [microtex_freeDrawingData] after it has no usages.
 *
 * The drawing data are arranged by following format:
 *
 * @code
 * bytes    desc
 * -----------------------------------------------------
 *     4    number of bytes of the drawing data
 *     1    drawing command (e.g. drawLine, setColor...)
 * @endcode
 *
 * And for each drawing command, followed by its arguments, the tables below
 * shows the drawing commands and its arguments.
 *
 * Short for data types:
 *
 * @code
 *      data type bytes  short
 * ---------------------------
 *            int     4  i32
 *   unsigned int     4  u32
 * unsigned short     2  u16
 *          float     4  f32
 *           bool     1  u8
 * @endcode
 *
 * And arguments for each drawing command:
 *
 * @code
 * cmd  args                desc
 * -----------------------------------------------------------------------------
 *   0          color: u32  Set color.
 *
 *   1    line width : f32  Set line stroke. The possible line-caps are
 *        miter limit: f32  {0: butt, 1: round, 2: square}, and the possible
 *           line cap: u32  line-joins are {0: bevel, 1: miter, 2: round}.
 *          line join: u32
 *
 *   2        has dash: u8  Set the dash pattern to draw lines, users are free
 *                          to use various dash patterns.
 *
 *   3 font family: string  Set font by font family name, the name is 0
 *                          terminated.
 *
 *   4      font size: f32  Set the font size.
 *
 *   5        delta-x: f32  Translate the context by delta-x and delta-y.
 *            delta-y: f32
 *
 *   6        scale-x: f32  Scale the context by scale-x and scale-y.
 *            scale-y: f32
 *
 *   7         radian: f32  Rotate the context by the given radian and pivot.
 *            pivot-x: f32
 *            pivot-y: f32
 *
 *   8                 N/A  Reset the transformation of the context.
 *
 *   9          glyph: u16  Draw a single glyph at given point (x, y), the glyph
 *                  x: f32  drawing should be baseline aligned.
 *                  y: f32
 *
 *  10        path-id: i32  Begin a path with id. The id will be >= 0 if the
 *                          path is cacheable, otherwise it will be < 0. The
 *                          engine will use [CBIsPathExists] to determine if the
 *                          path is in cache.
 *
 *  11              x: f32  Move to point (x, y).
 *                  y: f32
 *
 *  12              x: f32  Add a line with its end point (x, y) to path.
 *                  y: f32
 *
 *  13             x1: f32  Add a cubic Bezier spline to path, with control
 *                 y1: f32  points (x1, y1), (x2, y2) and the final point
 *                 x2: f32  (x3, y3).
 *                 y2: f32
 *                 x3: f32
 *                 y3: f32
 *
 *  14             x1: f32  Add a quadratic Bezier spline to path, with control
 *                 y1: f32  point (x1, y1) and the final point (x2, y2).
 *                 x2: f32
 *                 y2: f32
 *
 *  15                 N/A  Close the path.
 *
 *  16             id: i32  Fill the path with the given id. If the path is not
 *                          cacheable, it always is -1.
 *
 *  17             x1: f32  Draw a line from (x1, y1) to (x2, y2).
 *                 y1: f32
 *                 x2: f32
 *                 y2: f32
 *
 *  18              x: f32  Draw an outlined rectangle at point (x, y) with its
 *                  y: f32  width (w) and height (h).
 *                  w: f32
 *                  h: f32
 *
 *  19              x: f32  Draw a rectangle at point (x, y) with its width (w)
 *                  y: f32  and height (h).
 *                  w: f32
 *                  h: f32
 *
 *  20              x: f32  Draw an outlined round rectangle at point (x, y)
 *                  y: f32  with its width (w), height (h), radius in
 *                  w: f32  x-direction (rx) and radius in y-direction (ry).
 *                  h: f32
 *                 rx: f32
 *                 ry: f32
 *
 *  21              x: f32  Draw a round rectangle at point (x, y) with its
 *                  y: f32  width(w), height(h), radius in x-direction (rx) and
 *                  w: f32  radius in y-direction (ry).
 *                  h: f32
 *                 rx: f32
 *                 ry: f32
 *
 *  22             id: u32  Draw a text-layout by given id at point (x, y). The
 *                  x: f32  id was generated by [CBCreateTextLayout].
 *                  y: f32
 * @endcode
 */
MICROTEX_CAPI DrawingData microtex_getDrawingData(RenderPtr render, int x, int y);

/** Release the drawing data generated by [microtex_getDrawingData] before. */
MICROTEX_CAPI void microtex_freeDrawingData(DrawingData data);

/** Test if the host machine is little-endian. */
MICROTEX_CAPI bool microtex_isLittleEndian();

/** Get the render width. */
MICROTEX_CAPI int microtex_getRenderWidth(RenderPtr render);

/** Get the render height. */
MICROTEX_CAPI int microtex_getRenderHeight(RenderPtr render);

/** Get the render depth. */
MICROTEX_CAPI int microtex_getRenderDepth(RenderPtr render);

/** Test if the render is split (has new line). */
MICROTEX_CAPI bool microtex_isRenderSplit(RenderPtr render);

/** Set the text size to draw the render. */
MICROTEX_CAPI void microtex_setRenderTextSize(RenderPtr render, float size);

/** Set the foreground color to draw the render. */
MICROTEX_CAPI void microtex_setRenderForeground(RenderPtr render, unsigned int c);

#ifdef __cplusplus
}
#endif

#endif  // HAVE_CWRAPPER

#endif  // MICROTEX_CWRAPPER_H
