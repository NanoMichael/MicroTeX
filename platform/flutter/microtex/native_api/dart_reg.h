#ifndef MICROTEX_FLUTTER_DART_REG_H
#define MICROTEX_FLUTTER_DART_REG_H

#ifdef __cplusplus
extern "C" {
#endif

struct TextLayoutBounds {
  float width;
  float height;
  float ascent;
};

struct FontDesc {
  bool isBold;
  bool isItalic;
  bool isSansSerif;
  bool isMonospace;
  float fontSize;
};

typedef unsigned int (* F_createTextLayout)(const char*, FontDesc* f);
typedef void (* F_getTextLayoutBounds)(unsigned int id, TextLayoutBounds* b);
typedef void (* F_releaseTextLayout)(unsigned int id);
typedef bool (* F_isPathExists)(unsigned int id);

extern F_createTextLayout dart_createTextLayout;
extern F_getTextLayoutBounds dart_getTextLayoutBounds;
extern F_releaseTextLayout dart_releaseTextLayout;
extern F_isPathExists dart_isPathExists;

#ifdef __cplusplus
}
#endif

#endif //MICROTEX_FLUTTER_DART_REG_H
