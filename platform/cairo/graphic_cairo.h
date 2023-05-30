#ifndef GRAPHIC_CAIRO_H_INCLUDED
#define GRAPHIC_CAIRO_H_INCLUDED

#include <cairo.h>
#include <pango/pango.h>

#include "graphic/graphic.h"
#include "microtexexport.h"

namespace cairopp {

typedef std::shared_ptr<cairo_font_face_t> CairoFontFacePtr;
typedef std::shared_ptr<cairo_t> CairoCtxPtr;

}  // namespace cairopp

namespace microtex {

class MICROTEX_EXPORT Font_cairo : public Font {
private:
  static std::map<std::string, cairopp::CairoFontFacePtr> _cairoFtFaces;

  cairo_font_face_t* _fface;

  void loadFont(const std::string& file);

public:
  explicit Font_cairo(const std::string& file);

  ~Font_cairo() override;

  cairo_font_face_t* getCairoFontFace() const;

  bool operator==(const Font& f) const override;
};

/**************************************************************************************************/

class MICROTEX_EXPORT TextLayout_cairo : public TextLayout {
private:
  static cairopp::CairoCtxPtr _img_context;
  PangoLayout* _layout;
  float _ascent;

public:
  TextLayout_cairo(const std::string& src, FontStyle style, float size);

  ~TextLayout_cairo() override;

  void getBounds(Rect& r) override;

  void draw(Graphics2D& g2, float x, float y) override;
};

/**************************************************************************************************/

class MICROTEX_EXPORT PlatformFactory_cairo : public PlatformFactory {
public:
  sptr<Font> createFont(const std::string& file) override;

  sptr<TextLayout> createTextLayout(const std::string& src, FontStyle style, float size) override;
};

/**************************************************************************************************/

class MICROTEX_EXPORT Graphics2D_cairo : public Graphics2D {
private:
  cairo_t* _context;
  color _color;
  Stroke _stroke;
  sptr<Font_cairo> _font;
  float _fontSize;
  float _sx, _sy;

  void roundRect(float x, float y, float w, float h, float rx, float ry);

public:
  explicit Graphics2D_cairo(cairo_t* context);

  ~Graphics2D_cairo() override;

  cairo_t* getCairoContext() const;

  void setColor(color c) override;
  color getColor() const override;
  void setStroke(const Stroke& s) override;
  const Stroke& getStroke() const override;
  void setStrokeWidth(float w) override;
  void setDash(const std::vector<float>& dash) override;
  std::vector<float> getDash() override;
  sptr<Font> getFont() const override;
  void setFont(const sptr<Font>& font) override;
  float getFontSize() const override;
  void setFontSize(float size) override;
  void translate(float dx, float dy) override;
  void scale(float sx, float sy) override;
  void rotate(float angle) override;
  void rotate(float angle, float px, float py) override;
  void reset() override;
  float sx() const override;
  float sy() const override;
  void drawGlyph(u16 glyph, float x, float y) override;
  bool beginPath(i32 id) override;
  void moveTo(float x, float y) override;
  void lineTo(float x, float y) override;
  void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) override;
  void quadTo(float x1, float y1, float x2, float y2) override;
  void closePath() override;
  void fillPath(i32 id) override;
  void drawLine(float x, float y1, float x2, float y2) override;
  void drawRect(float x, float y, float w, float h) override;
  void fillRect(float x, float y, float w, float h) override;
  void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;
  void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}  // namespace microtex

#endif  // GRAPHIC_CAIRO_H_INCLUDED
