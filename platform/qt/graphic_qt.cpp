#include "graphic_qt.h"
#include "utils/log.h"
#include "utils/utils.h"

#include <QDebug>

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QFontDatabase>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QTransform>
#include <QtMath>
#include <QFile>
#include <QGlyphRun>

using namespace microtex;
using namespace std;

QMap<QString, QString> Font_qt::_qtFamilies;

void Font_qt::loadFont(const std::string& file) {
  QString filename(QString::fromStdString(file));
  if (!QFile::exists(filename)) {
    filename.prepend(":/");
  }
  // find from cache first
  if (_qtFamilies.contains(filename)) {
    _font.setFamily(_qtFamilies.value(filename));
    return;
  }
  // load to font database
  int id = QFontDatabase::addApplicationFont(filename);
  if (id == -1) {
#ifdef HAVE_LOG
    logv("failed to load font: %s\n", file.c_str());
#endif
  } else {
    auto families = QFontDatabase::applicationFontFamilies(id);
    if (families.size() >= 0) {
      auto first = families.first();
      _qtFamilies[filename] = first;
      _font.setFamily(families.first());
    } else {
#ifdef HAVE_LOG
      logv("no font families found: %s\n", file.c_str());
#endif
    }
  }
}

Font_qt::Font_qt(const std::string& file) {
  loadFont(file);
}

QFont Font_qt::getQFont() const {
  return _font;
}

bool Font_qt::operator==(const Font& f) const {
  const auto& cast = static_cast<const Font_qt&>(f);
  return _font == cast._font;
}

/**************************************************************************************************/

TextLayout_qt::TextLayout_qt(const std::string& src, FontStyle style, float size) {
  _text = QString::fromStdString(src);
  // _font.setPointSizeF(size);
  _font.setPixelSize(size);
  _font.setFamily("Serif");
  if (microtex::isSansSerif(style)) {
    _font.setFamily("Sans-Serif");
  }
  if (microtex::isMono(style)) {
    _font.setFamily("Monospace");
  }
  // todo fallback font families
  _font.setFamily("Noto Color Emoji");
  _font.setBold(microtex::isBold(style));
  _font.setItalic(microtex::isItalic(style));
}

void TextLayout_qt::getBounds(Rect& r) {
  QFontMetricsF fm(_font);
  QRectF br(fm.boundingRect(_text));
  r.x = br.left();
  r.y = br.top();
  r.w = br.width();
  r.h = br.height();
}

void TextLayout_qt::draw(Graphics2D& g2, float x, float y) {
  auto& g = static_cast<Graphics2D_qt&>(g2);
  g.getQPainter()->setFont(_font);
  g.getQPainter()->drawText(QPointF(x, y), _text);
}

/**************************************************************************************************/

sptr<Font> PlatformFactory_qt::createFont(const std::string& file) {
  return sptrOf<Font_qt>(file);
}

sptr<TextLayout> PlatformFactory_qt::createTextLayout(const std::string& src, FontStyle style, float size) {
  return sptrOf<TextLayout_qt>(src, style, size);
}

/**************************************************************************************************/

Graphics2D_qt::Graphics2D_qt(QPainter* painter) : _painter(painter) {
  _sx = _sy = 1.f;
  _fontSize = 1.f;
  setColor(BLACK);
  setStroke(Stroke());
}

QPainter* Graphics2D_qt::getQPainter() const {
  return _painter;
}

QBrush Graphics2D_qt::getQBrush() const {
  return QBrush(
    QColor(
      color_r(_color), color_g(_color),
      color_b(_color), color_a(_color)
    )
  );
}

void Graphics2D_qt::setPen() {

  QBrush brush(getQBrush());

  Qt::PenCapStyle cap;
  switch (_stroke.cap) {
    case CAP_ROUND:
      cap = Qt::RoundCap;
      break;
    case CAP_SQUARE:
      cap = Qt::SquareCap;
      break;
    case CAP_BUTT:
    default:
      cap = Qt::FlatCap;
      break;
  }

  Qt::PenJoinStyle join;
  switch (_stroke.join) {
    case JOIN_BEVEL:
      join = Qt::BevelJoin;
      break;
    case JOIN_ROUND:
      join = Qt::RoundJoin;
      break;
    case JOIN_MITER:
    default:
      join = Qt::MiterJoin;
      break;
  }

  QPen pen(brush, _stroke.lineWidth, Qt::SolidLine, cap, join);
  pen.setMiterLimit(_stroke.miterLimit);
  _painter->setPen(pen);
}

void Graphics2D_qt::setColor(color c) {
  _color = c;
  setPen();
}

color Graphics2D_qt::getColor() const {
  return _color;
}

void Graphics2D_qt::setStroke(const Stroke& s) {
  _stroke = s;
  setPen();
}

const Stroke& Graphics2D_qt::getStroke() const {
  return _stroke;
}

void Graphics2D_qt::setStrokeWidth(float w) {
  _stroke.lineWidth = w;
  setPen();
}

void Graphics2D_qt::setDash(const std::vector<float>& dash) {
  // todo
}

std::vector<float> Graphics2D_qt::getDash() {
  // todo
  return {};
}

sptr<Font> Graphics2D_qt::getFont() const {
  return _font;
}

void Graphics2D_qt::setFont(const sptr<Font>& font) {
  _font = static_pointer_cast<Font_qt>(font);
}

float Graphics2D_qt::getFontSize() const {
  return _fontSize;
}

void Graphics2D_qt::setFontSize(float size) {
  _fontSize = size;
}

void Graphics2D_qt::translate(float dx, float dy) {
  _painter->translate(dx, dy);
}

void Graphics2D_qt::scale(float sx, float sy) {
  _sx *= sx;
  _sy *= sy;
  _painter->scale(sx, sy);
}

void Graphics2D_qt::rotate(float angle) {
  _painter->rotate(qRadiansToDegrees(angle));
}

void Graphics2D_qt::rotate(float angle, float px, float py) {
  _painter->translate(px, py);
  _painter->rotate(qRadiansToDegrees(angle));
  _painter->translate(-px, -py);
}

void Graphics2D_qt::reset() {
  _painter->setTransform(QTransform());
  _sx = _sy = 1.f;
}

float Graphics2D_qt::sx() const {
  return _sx;
}

float Graphics2D_qt::sy() const {
  return _sy;
}

void Graphics2D_qt::drawGlyph(u16 glyph, float x, float y) {
  auto f = _font->getQFont();
  // f.setPointSizeF(_fontSize);
  f.setPixelSize(_fontSize);
  auto rf = QRawFont::fromFont(f);
  QGlyphRun g;
  g.setRawFont(rf);
  g.setGlyphIndexes({glyph});
  g.setPositions({{0, 0}});
  _painter->drawGlyphRun({x, y}, g);
}

bool Graphics2D_qt::beginPath(i32 id) {
  _path = QPainterPath();
  return false;
}

void Graphics2D_qt::moveTo(float x, float y) {
  _path.moveTo(x, y);
}

void Graphics2D_qt::lineTo(float x, float y) {
  _path.lineTo(x, y);
}

void Graphics2D_qt::cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) {
  _path.cubicTo(x1, y1, x2, y2, x3, y3);
}

void Graphics2D_qt::quadTo(float x1, float y1, float x2, float y2) {
  _path.quadTo(x1, y1, x2, y2);
}

void Graphics2D_qt::closePath() {
  _path.closeSubpath();
}

void Graphics2D_qt::fillPath(i32 id) {
  _painter->fillPath(_path, getQBrush());
}

void Graphics2D_qt::drawLine(float x1, float y1, float x2, float y2) {
  _painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
}

void Graphics2D_qt::drawRect(float x, float y, float w, float h) {
  _painter->drawRect(QRectF(x, y, w, h));
}

void Graphics2D_qt::fillRect(float x, float y, float w, float h) {
  _painter->fillRect(QRectF(x, y, w, h), getQBrush());
}

void Graphics2D_qt::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _painter->drawRoundedRect(QRectF(x, y, w, h), rx, ry);
}

void Graphics2D_qt::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _painter->setPen(QPen(Qt::NoPen));
  _painter->setBrush(getQBrush());

  _painter->drawRoundedRect(QRectF(x, y, w, h), rx, ry);

  setPen();
  _painter->setBrush(QBrush());
}
