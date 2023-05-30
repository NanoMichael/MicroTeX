#ifndef MICROTEX_CALLBACK_H
#define MICROTEX_CALLBACK_H

#ifdef HAVE_CWRAPPER

#ifdef __cplusplus
extern "C" {
#endif

/** Simple struct to represents the bounds of a text layout. */
typedef struct {
  float width;
  float height;
  float ascent;
} TextLayoutBounds;

/**
 * Simple struct to represents the description to create a font to
 * layout and draw a text-layout.
 *
 * Unlike the font the engine using internally, you are free to create
 * the font you want based on the given description, the description
 * is for reference, not a constraint.
 */
typedef struct {
  bool isBold;
  bool isItalic;
  bool isSansSerif;
  bool isMonospace;
  float fontSize;
} FontDesc;

/**
 * Callback to create a text-layout from given text and font description,
 * it should returns the id of the created text-layout, and then the id
 * will be used to retrieve the bounds and draw.
 *
 * See [lib/wrapper/graphic_wrapper.h: TextLayout_wrapper::draw].
 */
typedef unsigned int (*CBCreateTextLayout)(const char* txt, FontDesc* f);

/**
 * Callback to retrieve the bounds of the text-layout created by callback
 * [CBCreateTextLayout] before.
 */
typedef void (*CBGetTextLayoutBounds)(unsigned int id, TextLayoutBounds* b);

/**
 * Callback to release the text-layout created by callback [CBCreateTextLayout]
 * before.
 */
typedef void (*CBReleaseTextLayout)(unsigned int id);

/**
 * Callback to check if a (glyph) path given by id is in cache. It will be used
 * by [lib/graphic/graphic.h: Graphic2D::drawPath] to determine if a path is in
 * cache, if it does, the following path drawing command will be ignored.
 */
typedef bool (*CBIsPathExists)(unsigned int id);

extern CBCreateTextLayout microtex_createTextLayout;
extern CBGetTextLayoutBounds microtex_getTextLayoutBounds;
extern CBReleaseTextLayout microtex_releaseTextLayout;
extern CBIsPathExists microtex_isPathExists;

#ifdef __cplusplus
}
#endif

#endif

#endif  // MICROTEX_CALLBACK_H
