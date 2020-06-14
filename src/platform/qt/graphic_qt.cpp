#include "graphic_qt.h"

#include "config.h"

#if defined(BUILD_QT) && !defined(MEM_CHECK)

#include "platform/qt/graphic_qt.h"

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
#include <QStringList>
#include <QTransform>
#include <QtMath>

using namespace tex;
using namespace std;

QMap<QString, QString> Font_qt::_loaded_families;

namespace tex {
// Some wstrings arrive with a \0 at end, so we remove when converting
QString wstring_to_QString(const wstring& ws)
{
  QString out = QString::fromStdWString(ws);
  auto index = out.indexOf('\0');
  if (index != -1)
    out.truncate(index);
  return out;
}
}

Font_qt::Font_qt(const string& family, int style, float size) {

  //qInfo() << "new font" << QString::fromStdString(family) << style << size;

  _font.setFamily(QString::fromStdString(family));
  _font.setPointSizeF(size);

  _font.setBold(style & BOLD);
  _font.setItalic(style & ITALIC);
}

Font_qt::Font_qt(const string& file, float size)
{
  //qInfo() << "new font" << QString::fromStdString(file) << size;

  // set size for newly loaded and previously loaded font
  _font.setPointSizeF(size);

  QString filename(QString::fromStdString(file));

  if(_loaded_families.contains(filename)) {
    // file already loaded
    _font.setFamily(_loaded_families.value(filename));
#ifdef HAVE_LOG
    __log << file << " already loaded, skip\n";
#endif
    return;
  }

  QFontDatabase db;
  int id = db.addApplicationFont(QString::fromStdString(file));
  if( id == -1 ) {
#ifdef HAVE_LOG
    __log << file << " failed to load\n";
#endif
  } else {
    QStringList families = db.applicationFontFamilies(id);
    if( families.size() > 0 ) {
      _loaded_families[filename] = families.first();
      _font.setFamily(families.first());
    } else {
#ifdef HAVE_LOG
    __log << file << " no font families found\n";
#endif
    }
  }
}

string Font_qt::getFamily() const {
  return _font.family().toStdString();
}

int Font_qt::getStyle() const {
  int out = PLAIN;
  if(_font.bold())   out |= BOLD;
  if(_font.italic()) out |= ITALIC;
  return out;
}

QFont Font_qt::getQFont() const {
  return _font;
}

float Font_qt::getSize() const {
  return _font.pointSizeF();
}

sptr<Font> Font_qt::deriveFont(int style) const {
  return sptr<Font>(new Font_qt(getFamily(), style, getSize()));
}

bool Font_qt::operator==(const Font& ft) const {
  const Font_qt& o = static_cast<const Font_qt&>(ft);

  return getFamily()==o.getFamily() && getSize()==o.getSize() &&
    getStyle()==o.getStyle();
}

bool Font_qt::operator!=(const Font& ft) const {
  return !(*this == ft);
}

Font* Font::create(const string& file, float size) {
  return new Font_qt(file, size);
}

sptr<Font> Font::_create(const string& name, int style, float size) {
  return sptr<Font>(new Font_qt(name, style, size));
}

/**************************************************************************************************/

TextLayout_qt::TextLayout_qt(const wstring& src, const sptr<Font_qt>& f) :
  _font(f->getQFont()),
  _text(wstring_to_QString(src))
{
}

void TextLayout_qt::getBounds(_out_ Rect& r) {
  QFontMetricsF fm(_font);
  QRectF br(fm.boundingRect(_text));

  r.x = br.left();
  r.y = br.top();
  r.w = br.width();
  r.h = br.height();
}

void TextLayout_qt::draw(Graphics2D& g2, float x, float y) {
  Graphics2D_qt& g = static_cast<Graphics2D_qt&>(g2);
  g.getQPainter()->setFont(_font);
  g.getQPainter()->drawText(QPointF(x, y), _text);
}

sptr<TextLayout> TextLayout::create(const wstring& src, const sptr<Font>& font) {
  sptr<Font_qt> f = static_pointer_cast<Font_qt>(font);
  return sptr<TextLayout>(new TextLayout_qt(src, f));
}

/**************************************************************************************************/

Font_qt Graphics2D_qt::_default_font("SansSerif", PLAIN, 20.f);

Graphics2D_qt::Graphics2D_qt(QPainter* painter)
    : _painter(painter) {
  _sx = _sy = 1.f;
  setColor(BLACK);
  setStroke(Stroke());
  setFont(&_default_font);
}

QPainter* Graphics2D_qt::getQPainter() const {
  return _painter;
}

QBrush Graphics2D_qt::getQBrush() const {
  return QBrush(QColor(color_r(_color), color_g(_color),
                       color_b(_color), color_a(_color)));
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

const Font* Graphics2D_qt::getFont() const {
  return _font;
}

void Graphics2D_qt::setFont(const Font* font) {
  _font = static_cast<const Font_qt*>(font);
}

void Graphics2D_qt::translate(float dx, float dy) {
  //qInfo() << "translate" << dx << dy;
  _painter->translate(dx, dy);
}

void Graphics2D_qt::scale(float sx, float sy) {
  //qInfo() << "scale" << sx << sy;
  _sx *= sx;
  _sy *= sy;
  _painter->scale(sx, sy);
}

void Graphics2D_qt::rotate(float angle) {
  //qInfo() << "rotate" << angle;
  _painter->rotate(qRadiansToDegrees(angle));
}

void Graphics2D_qt::rotate(float angle, float px, float py) {

  //qInfo() << "translate" << px << py << "rotate" << angle;
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

void Graphics2D_qt::drawChar(wchar_t c, float x, float y) {
  wstring str = {c};
  drawText(str, x, y);
}

void Graphics2D_qt::drawText(const wstring& t, float x, float y) {

  _painter->setFont(_font->getQFont());

  QString text = wstring_to_QString(t);
  //qInfo() << "text" << x << y << text << text.toLocal8Bit();
  //for(size_t i=0; i<t.size(); ++i)
  //  qInfo() << 'v' << int(t[i]);

  _painter->drawText(QPointF(x, y), text);
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


/**************************************************************************************************/


#endif
