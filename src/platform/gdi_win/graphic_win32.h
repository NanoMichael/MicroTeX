#include "config.h"

#if defined(BUILD_WIN32) && !defined(MEM_CHECK)

#ifndef GRAPHIC_WIN32_H_INCLUDED
#define GRAPHIC_WIN32_H_INCLUDED

#include "common.h"
#include "graphic/graphic.h"

using namespace std;
using namespace tex;

namespace Gdiplus {

class Font;
class FontFamily;
class Graphics;
class Pen;
class Brush;
class SolidBrush;
class StringFormat;
class Bitmap;

}  // namespace Gdiplus

namespace tex {

class Font_win32 : public Font {
private:
  static const Gdiplus::FontFamily* _serif;
  static const Gdiplus::FontFamily* _sansserif;

  float _size;

  Font_win32();

public:
  int _style;
  sptr<Gdiplus::Font> _typeface;
  const Gdiplus::FontFamily* _family;

  Font_win32(const string& name, int style, float size);

  Font_win32(const string& file, float size);

  virtual float getSize() const override;

  virtual sptr<Font> deriveFont(int style) const override;

  virtual bool operator==(const Font& f) const override;

  virtual bool operator!=(const Font& f) const override;

  virtual ~Font_win32();

  static int convertStyle(int style);
};

/**************************************************************************************************/

class TextLayout_win32 : public TextLayout {
private:
  sptr<Font_win32> _font;
  wstring _txt;

public:
  static const Gdiplus::StringFormat* _format;
  static Gdiplus::Graphics* _g;
  static Gdiplus::Bitmap* _img;

  TextLayout_win32(const wstring& src, const sptr<Font_win32>& font);

  virtual void getBounds(Rect& bounds) override;

  virtual void draw(Graphics2D& g2, float x, float y) override;
};

/**************************************************************************************************/

class Graphics2D_win32 : public Graphics2D {
private:
  static const Gdiplus::StringFormat* _format;
  static const Font* _defaultFont;

  color _color;
  const Font* _font;
  Stroke _stroke;
  Gdiplus::Graphics* _g;
  Gdiplus::Pen* _pen;
  Gdiplus::SolidBrush* _brush;

  float _sx, _sy;

public:
  Graphics2D_win32(Gdiplus::Graphics* g);

  ~Graphics2D_win32();

  virtual void setColor(color c) override;

  virtual color getColor() const override;

  virtual void setStroke(const Stroke& s) override;

  virtual const Stroke& getStroke() const override;

  virtual void setStrokeWidth(float w) override;

  virtual const Font* getFont() const override;

  virtual void setFont(const Font* font) override;

  virtual void translate(float dx, float dy) override;

  virtual void scale(float sx, float sy) override;

  virtual void rotate(float angle) override;

  virtual void rotate(float angle, float px, float py) override;

  virtual void reset() override;

  virtual float sx() const override;

  virtual float sy() const override;

  virtual void drawChar(wchar_t c, float x, float y) override;

  virtual void drawText(const wstring& c, float x, float y) override;

  virtual void drawLine(float x1, float y1, float x2, float y2) override;

  virtual void drawRect(float x, float y, float w, float h) override;

  virtual void fillRect(float x, float y, float w, float h) override;

  virtual void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

  virtual void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}  // namespace tex

#endif  // GRAPHIC_WIN32_H_INCLUDED
#endif
