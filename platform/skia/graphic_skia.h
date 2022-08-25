#ifndef MICROTEX_GRAPHIC_SKIA_H
#define MICROTEX_GRAPHIC_SKIA_H

#include <include/core/SkCanvas.h>
#include <include/core/SkFont.h>
#include <include/core/SkPath.h>
#include <include/core/SkTypeface.h>

#include <map>
#include <string>

#include "graphic/graphic.h"
#include "microtexexport.h"

namespace skia {
namespace textlayout {
class Paragraph;
}  // namespace textlayout
}  // namespace skia

namespace microtex {

struct SkiaFontSettings {
  bool subpixel;
  SkFont::Edging edging;
  SkFontHinting hinting;
};

class MICROTEX_EXPORT Font_skia : public Font {
private:
  static std::map<std::string, sk_sp<SkTypeface>> _typefaces;
  sk_sp<SkTypeface> _typeface;

  void loadFont(const std::string &file);

public:
  explicit Font_skia(const std::string &file);

  bool operator==(const Font &f) const override;

  sk_sp<SkTypeface> getSkTypeface() const;
};

/**************************************************************************************************/

class MICROTEX_EXPORT TextLayout_skia : public TextLayout {
private:
  std::unique_ptr<skia::textlayout::Paragraph> _paragraph;
  std::size_t _textLen;

public:
  TextLayout_skia(const std::string &src, FontStyle style, float size);

  void getBounds(Rect &bounds) override;

  void draw(Graphics2D &g2, float x, float y) override;
};

/**************************************************************************************************/

class MICROTEX_EXPORT PlatformFactory_skia : public PlatformFactory {
public:
  sptr<Font> createFont(const std::string &file) override;

  sptr<TextLayout> createTextLayout(const std::string &src, FontStyle style, float size) override;
};

/**************************************************************************************************/

class MICROTEX_EXPORT Graphics2D_skia : public Graphics2D {
private:
  SkCanvas *_canvas;
  SkPaint _paint;
  color _color;
  Stroke _stroke;
  sptr<Font_skia> _font;
  float _fontSize;
  float _sx, _sy;
  SkPath _path;
  SkiaFontSettings _fontSettings;
  std::vector<float> _dash;

public:
  explicit Graphics2D_skia(SkCanvas *canvas);

  SkCanvas *getSkCanvas() const;
  const SkPaint &getSkPaint() const;
  void setSkFontSettings(const SkiaFontSettings &settings);

  void setColor(color c) override;
  color getColor() const override;
  void setStroke(const Stroke &s) override;
  const Stroke &getStroke() const override;
  void setStrokeWidth(float w) override;
  void setDash(const std::vector<float> &dash) override;
  std::vector<float> getDash() override;
  sptr<Font> getFont() const override;
  void setFont(const sptr<Font> &font) override;
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
  void drawLine(float x1, float y1, float x2, float y2) override;
  void drawRect(float x, float y, float w, float h) override;
  void fillRect(float x, float y, float w, float h) override;
  void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;
  void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}  // namespace microtex

#endif  // MICROTEX_GRAPHIC_SKIA_H
