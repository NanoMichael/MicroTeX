#include "config.h"

#if defined(BUILD_GTK) && !defined(MEM_CHECK)

#ifndef GRAPHIC_CAIRO_H_INCLUDED
#define GRAPHIC_CAIRO_H_INCLUDED

#include "graphic/graphic.h"

#include <cairomm/context.h>
#include <pangomm/fontdescription.h>
#include <pangomm/layout.h>

using namespace std;

namespace tex {

class Font_cairo : public Font {
private:
  static map<string, Cairo::RefPtr<Cairo::FtFontFace>> _cairoFtFaces;
  static map<string, string> _families;

  int _style;
  double _size;
  string _family;
  Cairo::RefPtr<Cairo::FtFontFace> _fface;

  void loadFont(const string& file);

public:
  Font_cairo(const string& family = "", int style = PLAIN, float size = 1.f);

  Font_cairo(const string& file, float size);

  string getFamily() const;

  int getStyle() const;

  Cairo::RefPtr<Cairo::FtFontFace> getCairoFontFace() const;

  virtual float getSize() const override;

  virtual sptr<Font> deriveFont(int style) const override;

  virtual bool operator==(const Font& f) const override;

  virtual bool operator!=(const Font& f) const override;

  virtual ~Font_cairo() {};
};

/**************************************************************************************************/

class TextLayout_cairo : public TextLayout {
private:
  static Cairo::RefPtr<Cairo::Context> _img_context;
  Glib::RefPtr<Pango::Layout> _layout;
  float _ascent;

public:
  TextLayout_cairo(const wstring& src, const sptr<Font_cairo>& font);

  virtual void getBounds(_out_ Rect& r) override;

  virtual void draw(Graphics2D& g2, float x, float y) override;
};

/**************************************************************************************************/

class Graphics2D_cairo : public Graphics2D {
private:
  static Font_cairo _default_font;

  Cairo::RefPtr<Cairo::Context> _context;
  color _color;
  Stroke _stroke;
  const Font_cairo* _font;
  float _sx, _sy;

  void roundRect(float x, float y, float w, float h, float rx, float ry);

public:
  Graphics2D_cairo(const Cairo::RefPtr<Cairo::Context>& context);

  const Cairo::RefPtr<Cairo::Context>& getCairoContext() const;

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

  virtual void drawText(const wstring& t, float x, float y) override;

  virtual void drawLine(float x, float y1, float x2, float y2) override;

  virtual void drawRect(float x, float y, float w, float h) override;

  virtual void fillRect(float x, float y, float w, float h) override;

  virtual void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

  virtual void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}  // namespace tex

#endif  // GRAPHIC_CAIRO_H_INCLUDED
#endif  // BUILD_GTK && !MEM_CHECK
