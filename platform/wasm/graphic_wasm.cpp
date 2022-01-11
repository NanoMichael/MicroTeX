#include "grapihc_wasm.h"

using namespace tex;

bool Font_wasm::operator==(const Font& f) const {
  return true;
}

/**********************************************************************************/

void TextLayout_wasm::getBounds(Rect& bounds) {
  // todo
}

void TextLayout_wasm::draw(Graphics2D& g2, float x, float y) {
  // todo
}

/**********************************************************************************/

sptr<Font> PlatformFactory_wasm::createFont(const std::string& file) {
  // EMPTY IMPL
  return sptrOf<Font_wasm>();
}

sptr<TextLayout> PlatformFactory_wasm::createTextLayout(
  const std::string& src,
  FontStyle style, float size
) {
  // todo
  return sptrOf<TextLayout_wasm>();
}

/**********************************************************************************/

void* Graphics2D_wasm::getDrawingData() {
  return _cmds.finish();
}

void Graphics2D_wasm::setColor(color c) {
  _color = c;
  _cmds.put(0, c);
}

color Graphics2D_wasm::getColor() const { return _color; }

void Graphics2D_wasm::setStroke(const Stroke& s) {
  _stroke = s;
  _cmds.put(1, s.lineWidth, s.miterLimit, s.cap, s.join);
}

void Graphics2D_wasm::setStrokeWidth(float w) {
  _stroke.lineWidth = w;
  _cmds.put(1, _stroke.lineWidth, _stroke.miterLimit, _stroke.cap, _stroke.join);
}

const Stroke& Graphics2D_wasm::getStroke() const {
  return _stroke;
}

void Graphics2D_wasm::setDash(const std::vector<float>& dash) {
  // todo
}

std::vector<float> Graphics2D_wasm::getDash() {
  // todo
  return {};
}

sptr<Font> Graphics2D_wasm::getFont() const {
  return sptrOf<Font_wasm>();
}

void Graphics2D_wasm::setFont(const sptr<Font>& font) {}

float Graphics2D_wasm::getFontSize() const {
  return 1.f;
}

void Graphics2D_wasm::setFontSize(float size) {}

void Graphics2D_wasm::translate(float dx, float dy) {
  _cmds.put(2, dx, dy);
}

void Graphics2D_wasm::scale(float sx, float sy) {
  _sx *= sx;
  _sy *= sy;
  _cmds.put(3, sx, sy);
}

void Graphics2D_wasm::rotate(float angle) {
  _cmds.put(4, angle, 0.f, 0.f);
}

void Graphics2D_wasm::rotate(float angle, float px, float py) {
  _cmds.put(4, angle, px, py);
}

void Graphics2D_wasm::reset() {
  _sx = _sy = 1.f;
  _cmds.put(5);
}

float Graphics2D_wasm::sx() const { return _sx; }

float Graphics2D_wasm::sy() const { return _sy; }

void Graphics2D_wasm::drawGlyph(u16 glyph, float x, float y) {}

void Graphics2D_wasm::moveTo(float x, float y) {
  _cmds.put(6, x, y);
}

void Graphics2D_wasm::lineTo(float x, float y) {
  _cmds.put(7, x, y);
}

void Graphics2D_wasm::cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) {
  _cmds.put(8, x1, y1, x2, y2, x3, y3);
}

void Graphics2D_wasm::quadTo(float x1, float y1, float x2, float y2) {
  _cmds.put(9, x1, y1, x2, y2);
}

void Graphics2D_wasm::closePath() {
  _cmds.put(10);
}

void Graphics2D_wasm::fillPath() {
  _cmds.put(11);
}

void Graphics2D_wasm::drawLine(float x1, float y1, float x2, float y2) {
  _cmds.put(12, x1, y1, x2, y2);
}

void Graphics2D_wasm::drawRect(float x, float y, float w, float h) {
  _cmds.put(13, x, y, w, h);
}

void Graphics2D_wasm::fillRect(float x, float y, float w, float h) {
  _cmds.put(14, x, y, w, h);
}

void Graphics2D_wasm::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _cmds.put(15, x, y, w, h, rx, ry);
}

void Graphics2D_wasm::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _cmds.put(16, x, y, w, h, rx, ry);
}
