#ifndef FONT_BASIC_H_INCLUDED
#define FONT_BASIC_H_INCLUDED

#include "common.h"
#include "graphic/graphic.h"

namespace tex {

/** Contains the metrics for 1 character: width, height, depth and italic correction */
struct Metrics {
  float width, height, depth, italic, size;

  Metrics() = delete;

  Metrics(const Metrics&) = delete;

  explicit Metrics(float w, float h, float d, float i, float factor, float s)
      : width(w * factor), height(h * factor), depth(d * factor), italic(i * factor), size(s) {}
};

/** Represents a specific character in a specific font (identified by its font id) */
struct CharFont {
  wchar_t chr;
  int     fontId, boldFontId;

  CharFont() : chr(0), fontId(0), boldFontId(0) {}

  CharFont(wchar_t c, int f) : chr(c), fontId(f), boldFontId(f) {}

  CharFont(wchar_t c, int f, int bf) : chr(c), fontId(f), boldFontId(bf) {}

#ifdef HAVE_LOG
  friend std::ostream& operator<<(ostream& os, const CharFont& info);
#endif
};

/** Class represents a character together with its font, font id and metric information */
class Char {
private:
  wchar_t        _c;
  int            _fontCode;
  const Font*    _font;
  sptr<CharFont> _cf;
  sptr<Metrics>  _m;

public:
  Char() = delete;

  Char(wchar_t c, const Font* f, int fc, const sptr<Metrics>& m);

  sptr<CharFont> getCharFont() const { return _cf; }

  inline wchar_t getChar() const { return _c; }

  inline const Font* getFont() const { return _font; }

  inline int getFontCode() const { return _fontCode; }

  inline float getWidth() const { return _m->width; }

  inline float getItalic() const { return _m->italic; }

  inline float getHeight() const { return _m->height; }

  inline float getDepth() const { return _m->depth; }

  inline float getSize() const { return _m->size; }
};

/**
 * Represents an extension character that is defined by Char-objects of it's 4
 * possible parts (null means part not present).
 */
class Extension {
private:
  const Char* const _top;
  const Char* const _middle;
  const Char* const _bottom;
  const Char* const _repeat;

public:
  Extension() = delete;

  Extension(const Extension&) = delete;

  Extension(Char* t, Char* m, Char* r, Char* b)
      : _top(t), _middle(m), _repeat(r), _bottom(b) {}

  inline bool hasTop() const { return _top != nullptr; }

  inline bool hasMiddle() const { return _middle != nullptr; }

  inline bool hasBottom() const { return _bottom != nullptr; }

  inline bool hasRepeat() const { return _repeat != nullptr; }

  inline const Char& getTop() const { return *_top; }

  inline const Char& getMiddle() const { return *_middle; }

  inline const Char& getRepeat() const { return *_repeat; }

  inline const Char& getBottom() const { return *_bottom; }

  ~Extension();
};

}  // namespace tex

#endif
