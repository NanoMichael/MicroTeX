#ifndef MICROTEX_CALLBACK_H
#define MICROTEX_CALLBACK_H

#ifdef HAVE_CWRAPPER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  float width;
  float height;
  float ascent;
} TextLayoutBounds;

typedef struct {
  bool isBold;
  bool isItalic;
  bool isSansSerif;
  bool isMonospace;
  float fontSize;
} FontDesc;

typedef unsigned int (* F_createTextLayout)(const char*, FontDesc* f);
typedef void (* F_getTextLayoutBounds)(unsigned int id, TextLayoutBounds* b);
typedef void (* F_releaseTextLayout)(unsigned int id);
typedef bool (* F_isPathExists)(unsigned int id);

extern F_createTextLayout microtex_createTextLayout;
extern F_getTextLayoutBounds microtex_getTextLayoutBounds;
extern F_releaseTextLayout microtex_releaseTextLayout;
extern F_isPathExists microtex_isPathExists;

#ifdef __cplusplus
}
#endif

#endif

#endif //MICROTEX_CALLBACK_H
