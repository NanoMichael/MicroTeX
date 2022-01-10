#ifndef LATEX_GRAPIHC_WASM_H
#define LATEX_GRAPIHC_WASM_H

#include "graphic/graphic.h"

namespace tex {

class Font_wasm : public Font {
public:
  bool operator==(const Font& f) const override;
};

class TextLayout_wasm : public TextLayout {
public:
  void getBounds(Rect& bounds) override;

  void draw(Graphics2D& g2, float x, float y) override;
};

class PlatformFactory_wasm : public PlatformFactory {
public:
  sptr<Font> createFont(const std::string& file) override;

  sptr<TextLayout> createTextLayout(const std::string& src, FontStyle style, float size) override;
};

class Graphics2D_wasm : public Graphics2D {
private:
public:
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

  void moveTo(float x, float y) override;

  void lineTo(float x, float y) override;

  void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) override;

  void quadTo(float x1, float y1, float x2, float y2) override;

  void closePath() override;

  void fillPath() override;

  void drawLine(float x1, float y1, float x2, float y2) override;

  void drawRect(float x, float y, float w, float h) override;

  void fillRect(float x, float y, float w, float h) override;

  void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

  void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}

#endif //LATEX_GRAPIHC_WASM_H
