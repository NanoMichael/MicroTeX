#include "config.h"

#if defined(BUILD_SKIA) && !defined(MEM_CHECK)

#ifndef GRAPHIC_SKIA_H_INCLUDED
#define GRAPHIC_SKIA_H_INCLUDED

#include <string>
#include "graphic/graphic.h"
#include <core/SkFont.h>
#include <core/SkCanvas.h>
#include <map>
#include <QtCore/QString>

namespace tex {

class Font_skia : public Font {

private:
  SkFont _font{};

  static std::map<std::pair<std::string, int>, int> _test;
  static std::map<std::pair<std::string, int>, sk_sp<SkTypeface>> _named_typefaces;
  static std::map<std::string, sk_sp<SkTypeface>> _file_typefaces;

  static sk_sp<SkTypeface> loadTypefaceFromName(const std::string &family, int style = PLAIN);

  static sk_sp<SkTypeface> loadTypefaceFromFile(const std::string &file);

  Font_skia(sk_sp<SkTypeface> typeface, float size);

public:

  // Set hinting and edging for rendering on LCD screen
  static SkFont::Edging Edging;
  static SkFontHinting Hinting;

  Font_skia(const std::string &family = "", int style = PLAIN, float size = 1.f);

  Font_skia(const std::string &file, float size);

  std::string getFamily() const;

  int getStyle() const;

  SkFont getSkFont() const;

  virtual float getSize() const override;

  virtual sptr<Font> deriveFont(int style) const override;

  virtual bool operator==(const Font &f) const override;

  virtual bool operator!=(const Font &f) const override;

  virtual ~Font_skia() {};

};


/**************************************************************************************************/

class TextLayout_skia : public TextLayout {
private:
  SkFont _font;
  std::string _text;

public:
  TextLayout_skia(const std::wstring &src, const sptr<Font_skia> &font);

  virtual void getBounds(_out_ Rect &r) override;

  virtual void draw(Graphics2D &g2, float x, float y) override;
};

/**************************************************************************************************/

class Graphics2D_skia : public Graphics2D {
private:
  static Font_skia _default_font;

  SkCanvas *_canvas;
  SkPaint _paint;
  color _color;
  Stroke _stroke;
  const Font_skia *_font;
  float _sx, _sy;

public:
  Graphics2D_skia(SkCanvas *painter);

  SkCanvas *getSkCanvas() const;

  const SkPaint &getSkPaint() const;

  virtual void setColor(color c) override;

  virtual color getColor() const override;

  virtual void setStroke(const Stroke &s) override;

  virtual const Stroke &getStroke() const override;

  virtual void setStrokeWidth(float w) override;

  virtual const Font *getFont() const override;

  virtual void setFont(const Font *font) override;

  virtual void translate(float dx, float dy) override;

  virtual void scale(float sx, float sy) override;

  virtual void rotate(float angle) override;

  virtual void rotate(float angle, float px, float py) override;

  virtual void reset() override;

  virtual float sx() const override;

  virtual float sy() const override;

  virtual void drawChar(wchar_t c, float x, float y) override;

  virtual void drawText(const std::wstring &t, float x, float y) override;

  virtual void drawLine(float x, float y1, float x2, float y2) override;

  virtual void drawRect(float x, float y, float w, float h) override;

  virtual void fillRect(float x, float y, float w, float h) override;

  virtual void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

  virtual void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}

#endif   // GRAPHIC_SKIA_H_INCLUDED
#endif   // defined(BUILD_SKIA) && !defined(MEM_CHECK)
