#include "box_single.h"
#include "fonts/fonts.h"

using namespace std;
using namespace tex;

CharBox::CharBox(const Char& c) {
  _cf = c.getCharFont();
  _size = c.getSize();
  _width = c.getWidth();
  _height = c.getHeight();
  _depth = c.getDepth();
  _italic = c.getItalic();
}

void CharBox::addItalicCorrectionToWidth() {
  _width += _italic;
  _italic = 0;
}

void CharBox::draw(Graphics2D& g2, float x, float y) {
  g2.translate(x, y);
  const Font* font = FontInfo::getFont(_cf->fontId);
  if (_size != 1) g2.scale(_size, _size);
  if (g2.getFont() != font) g2.setFont(font);
  g2.drawChar(_cf->chr, 0, 0);
  // reset
  if (_size != 1) g2.scale(1.f / _size, 1.f / _size);
  g2.translate(-x, -y);
}

int CharBox::lastFontId() {
  return _cf->fontId;
}

sptr<Font> TextRenderingBox::_font(nullptr);

void TextRenderingBox::_init_() {
  _font = Font::_create("Serif", PLAIN, 10);
}

void TextRenderingBox::_free_() {
  // For memory check purpose
  // to check if has memory leak
  _font = nullptr;
}

void TextRenderingBox::setFont(const string& name) {
  _font = Font::_create(name, PLAIN, 10);
}

void TextRenderingBox::init(
  const wstring& str, int type, float size, const sptr<Font>& f, bool kerning
) {
  _size = size;
  _layout = TextLayout::create(str, f->deriveFont(type));
  Rect rect;
  _layout->getBounds(rect);
  _height = -rect.y * size / 10;
  _depth = rect.h * size / 10 - _height;
  _width = (rect.w + rect.x + 0.4f) * size / 10;
}

void TextRenderingBox::draw(Graphics2D& g2, float x, float y) {
  g2.translate(x, y);
  g2.scale(0.1f * _size, 0.1f * _size);
  _layout->draw(g2, 0, 0);
  g2.scale(10 / _size, 10 / _size);
  g2.translate(-x, -y);
}

LineBox::LineBox(const vector<float>& lines, float thickness) {
  _thickness = thickness;
  if (lines.size() % 4 != 0) throw ex_invalid_param("The vector not represent lines.");
  _lines = lines;
}

void LineBox::draw(Graphics2D& g2, float x, float y) {
  const float oldThickness = g2.getStroke().lineWidth;
  g2.setStrokeWidth(_thickness);
  g2.translate(0, -_height);
  int count = _lines.size() / 4;
  for (int i = 0; i < count; i++) {
    int j = i * 4;
    float x1 = _lines[j] + x, y1 = _lines[j + 1] + y;
    float x2 = _lines[j + 2] + x, y2 = _lines[j + 3] + y;
    g2.drawLine(x1, y1, x2, y2);
  }
  g2.translate(0, _height);
  g2.setStrokeWidth(oldThickness);
}

RuleBox::RuleBox(float thickness, float width, float shift, color c, bool trueshift)
  : _color(c), _speShift(0) {
  _height = thickness;
  _width = width;
  if (trueshift) {
    _shift = shift;
  } else {
    _shift = 0;
    _speShift = shift;
  }
}

void RuleBox::draw(Graphics2D& g2, float x, float y) {
  const color oldColor = g2.getColor();
  if (!isTransparent(_color)) g2.setColor(_color);
  const Stroke& oldStroke = g2.getStroke();
  g2.setStroke(Stroke(_height, CAP_BUTT, JOIN_BEVEL));
  y = y - _height / 2.f - _speShift;
  g2.drawLine(x, y, x + _width, y);
  g2.setStroke(oldStroke);
  g2.setColor(oldColor);
}

DebugBox::DebugBox(const sptr<Box>& base) {
  copyMetrics(base);
}

void DebugBox::draw(Graphics2D& g2, float x, float y) {
  const color prevColor = g2.getColor();
  const Stroke& prevStroke = g2.getStroke();
  g2.setColor(red);
  g2.setStrokeWidth(std::abs(1.f / g2.sx()));
  g2.drawRect(x, y - _height, _width, _height + _depth);
  g2.setColor(prevColor);
  g2.setStroke(prevStroke);
}
