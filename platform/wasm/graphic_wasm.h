#ifndef LATEX_GRAPHIC_WASM_H
#define LATEX_GRAPHIC_WASM_H

#include "graphic/graphic.h"
#include "cmd.h"

namespace tinytex {

/** EMPTY IMPL - no font support */
class Font_wasm : public Font {
public:
  bool operator==(const Font& f) const override;
};

/**********************************************************************************/

class TextLayout_wasm : public TextLayout {
private:
  // shared floats between js
  static float _bounds[3];

  const unsigned int _id;

public:
  explicit TextLayout_wasm(unsigned int id);

  ~TextLayout_wasm() noexcept override;

  void getBounds(Rect& bounds) override;

  void draw(Graphics2D& g2, float x, float y) override;
};

/**********************************************************************************/

class PlatformFactory_wasm : public PlatformFactory {
public:
  sptr<Font> createFont(const std::string& file) override;

  sptr<TextLayout> createTextLayout(
    const std::string& src,
    FontStyle style, float size
  ) override;
};

/**********************************************************************************/

class Graphics2D_wasm : public Graphics2D {
private:
  color _color;
  Stroke _stroke;
  Cmds _cmds;
  float _sx, _sy;

public:
  Graphics2D_wasm();

  void* getDrawingData();

  void setColor(color c) override; // 0

  color getColor() const override;

  void setStroke(const Stroke& s) override; // 1

  const Stroke& getStroke() const override;

  void setStrokeWidth(float w) override; // 1

  void setDash(const std::vector<float>& dash) override; // 19

  std::vector<float> getDash() override;

  // region EMPTY IMPL - no font support
  sptr<Font> getFont() const override;

  void setFont(const sptr<Font>& font) override;

  float getFontSize() const override;

  void setFontSize(float size) override;
  // endregion

  void translate(float dx, float dy) override; // 2

  void scale(float sx, float sy) override; // 3

  void rotate(float angle) override; // 4

  void rotate(float angle, float px, float py) override; // 4

  void reset() override; // 5

  float sx() const override;

  float sy() const override;

  // region EMPTY IMPL - no glyph support
  void drawGlyph(u16 glyph, float x, float y) override;
  // endregion

  void beginPath() override; // 17

  void moveTo(float x, float y) override; // 6

  void lineTo(float x, float y) override; // 7

  void cubicTo(
    float x1, float y1,
    float x2, float y2,
    float x3, float y3
  ) override; // 8

  void quadTo(float x1, float y1, float x2, float y2) override; // 9

  void closePath() override; // 10

  void fillPath() override; // 11

  void drawLine(float x1, float y1, float x2, float y2) override; // 12

  void drawRect(float x, float y, float w, float h) override; // 13

  void fillRect(float x, float y, float w, float h) override; // 14

  void drawRoundRect(
    float x, float y,
    float w, float h,
    float rx, float ry
  ) override; // 15

  void fillRoundRect(
    float x, float y,
    float w, float h,
    float rx, float ry
  ) override; // 16

  void drawTextLayout(unsigned int id, float x, float y); // 18
};

}

#endif //LATEX_GRAPHIC_WASM_H
