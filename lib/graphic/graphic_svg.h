#ifndef LATEX_GRAPHIC_SVG_H
#define LATEX_GRAPHIC_SVG_H

#include "graphic/graphic.h"

namespace tex {

class XMLNode;
class Matrix;

class Graphics2D_svg : Graphics2D {
private:
  color _color;
  Stroke _stroke;
  XMLNode* _svg;
  Matrix* _matrix;

public:
  Graphics2D_svg();

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

#endif //LATEX_GRAPHIC_SVG_H
