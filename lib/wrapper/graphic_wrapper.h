#ifndef MICROTEX_GRAPHIC_WRAPPER_H
#define MICROTEX_GRAPHIC_WRAPPER_H

#ifdef HAVE_CWRAPPER

#include "graphic/graphic.h"
#include "wrapper/byte_seq.h"

namespace microtex {

class Font_wrapper : public Font {
private:
  std::string _name;

public:
  explicit Font_wrapper(std::string name);

  const std::string& name() const;

  bool operator==(const Font& f) const override;
};

/**************************************************************************************************/

class TextLayout_wrapper : public TextLayout {
private:
  const u32 _id;

public:
  explicit TextLayout_wrapper(u32 id);

  ~TextLayout_wrapper() noexcept override;

  void getBounds(Rect& bounds) override;

  void draw(Graphics2D& g2, float x, float y) override;
};

/**************************************************************************************************/

class PlatformFactory_wrapper : public PlatformFactory {
public:
  sptr<Font> createFont(const std::string& file) override;

  sptr<TextLayout> createTextLayout(const std::string& src, FontStyle style, float size) override;
};

/**************************************************************************************************/

class Graphics2D_wrapper : public Graphics2D {
private:
  color _color;
  Stroke _stroke;
  ByteSeq _cmds;
  float _sx, _sy;

  sptr<Font> _font;
  float _fontSize;

public:
  Graphics2D_wrapper();

  void* getDrawingData();

  // cmd: 0
  void setColor(color c) override;

  color getColor() const override;

  // cmd: 1
  void setStroke(const Stroke& s) override;

  const Stroke& getStroke() const override;

  // cmd: 1
  void setStrokeWidth(float w) override;

  // cmd: 2
  void setDash(const std::vector<float>& dash) override;

  std::vector<float> getDash() override;

  sptr<Font> getFont() const override;

  // cmd: 3
  void setFont(const sptr<Font>& font) override;

  float getFontSize() const override;

  // cmd: 4
  void setFontSize(float size) override;

  // cmd: 5
  void translate(float dx, float dy) override;

  // cmd: 6
  void scale(float sx, float sy) override;

  // cmd: 7
  void rotate(float angle) override;

  // cmd: 7
  void rotate(float angle, float px, float py) override;

  // cmd: 8
  void reset() override;

  float sx() const override;

  float sy() const override;

  // cmd: 9
  void drawGlyph(u16 glyph, float x, float y) override;

  // cmd: 10
  bool beginPath(i32 id) override;

  // cmd: 11
  void moveTo(float x, float y) override;

  // cmd: 12
  void lineTo(float x, float y) override;

  // cmd: 13
  void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) override;

  // cmd: 14
  void quadTo(float x1, float y1, float x2, float y2) override;

  // cmd: 15
  void closePath() override;

  // cmd: 16
  void fillPath(i32 id) override;

  // cmd: 17
  void drawLine(float x1, float y1, float x2, float y2) override;

  // cmd: 18
  void drawRect(float x, float y, float w, float h) override;

  // cmd: 19
  void fillRect(float x, float y, float w, float h) override;

  // cmd: 20
  void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

  // cmd: 21
  void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;

  // cmd: 22
  void drawTextLayout(u32 id, float x, float y);
};

}  // namespace microtex

#endif  // HAVE_CWRAPPER

#endif  // MICROTEX_GRAPHIC_WRAPPER_H
