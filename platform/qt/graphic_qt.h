#ifndef GRAPHIC_QT_H_INCLUDED
#define GRAPHIC_QT_H_INCLUDED

#include <QBrush>
#include <QFont>
#include <QMap>
#include <QPainter>
#include <QPainterPath>
#include <QString>
#include <QTextLayout>
#include <string>

#include "graphic/graphic.h"
#include "microtexexport.h"

namespace microtex {

class MICROTEX_EXPORT Font_qt : public Font {
private:
  QFont _font;

  void loadFont(const std::string& file);

public:
  explicit Font_qt(const std::string& file);

  QFont getQFont() const;

  bool operator==(const Font& f) const override;

  ~Font_qt() override = default;
};

/**************************************************************************************************/

class MICROTEX_EXPORT TextLayout_qt : public TextLayout {
private:
  QFont _font;
  QString _text;

public:
  TextLayout_qt(const std::string& src, FontStyle style, float size);

  static void setFallbackFontFamily(const std::string& family);

  void getBounds(Rect& r) override;

  void draw(Graphics2D& g2, float x, float y) override;
};

/**************************************************************************************************/

class MICROTEX_EXPORT PlatformFactory_qt : public PlatformFactory {
public:
  sptr<Font> createFont(const std::string& file) override;

  sptr<TextLayout> createTextLayout(const std::string& src, FontStyle style, float size) override;
};

/**************************************************************************************************/

class MICROTEX_EXPORT Graphics2D_qt : public Graphics2D {
private:
  QPainterPath _path;
  QPen _pen;
  QPainter* _painter;

  color _color;
  Stroke _stroke;
  sptr<Font_qt> _font;
  float _fontSize;
  float _sx, _sy;

  QBrush getQBrush() const;

public:
  explicit Graphics2D_qt(QPainter* painter);

  QPainter* getQPainter() const;

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

#endif  // GRAPHIC_QT_H_INCLUDED
