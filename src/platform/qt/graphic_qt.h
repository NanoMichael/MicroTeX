#include "config.h"

#if defined(BUILD_QT) && !defined(MEM_CHECK)

#ifndef GRAPHIC_QT_H_INCLUDED
#define GRAPHIC_QT_H_INCLUDED

#include <string>
#include "graphic/graphic.h"

#include <QBrush>
#include <QFont>
#include <QMap>
#include <QPainter>
#include <QString>
#include <QTextLayout>

namespace tex {

class Font_qt : public Font {
private:
  static QMap<QString, QString> _qtFamilies;

  QFont _font;

  void loadFont(const std::string& file);

public:
  explicit Font_qt(const std::string& file);

  QFont getQFont() const;

  bool operator==(const Font& f) const override;

  ~Font_qt() override = default;
};

/**************************************************************************************************/

class TextLayout_qt : public TextLayout {
private:
  QFont _font;
  QString _text;

public:
  TextLayout_qt(const std::string& src, FontStyle style, float size);

  void getBounds(Rect& r) override;

  void draw(Graphics2D& g2, float x, float y) override;
};

/**************************************************************************************************/

class CLATEXMATH_EXPORT Graphics2D_qt : public Graphics2D {
private:
  QPainter* _painter;

  color _color;
  Stroke _stroke;
  sptr <Font_qt> _font;
  float _fontSize;
  float _sx, _sy;

  void setPen();

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

  sptr <Font> getFont() const override;

  void setFont(const sptr <Font>& font) override;

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

  void drawLine(float x, float y1, float x2, float y2) override;

  void drawRect(float x, float y, float w, float h) override;

  void fillRect(float x, float y, float w, float h) override;

  void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

  void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}

#endif   // GRAPHIC_QT_H_INCLUDED
#endif   // defined(BUILD_QT) && !defined(MEM_CHECK)
