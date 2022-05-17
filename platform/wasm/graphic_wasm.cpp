#include "graphic_wasm.h"
#include "jsapi.h"
#include "utils/utils.h"

using namespace microtex;

bool Font_wasm::operator==(const Font& f) const {
  return true;
}

/**********************************************************************************/

float TextLayout_wasm::_bounds[3];

TextLayout_wasm::TextLayout_wasm(unsigned int id) : _id(id) {}

TextLayout_wasm::~TextLayout_wasm() noexcept {
  js_releaseTextLayout(_id);
}

void TextLayout_wasm::getBounds(Rect& bounds) {
  js_getTextLayoutBounds(_id, _bounds);
  bounds.x = 0;
  bounds.w = _bounds[0];
  bounds.h = _bounds[1];
  bounds.y = _bounds[2];
}

void TextLayout_wasm::draw(Graphics2D& g2, float x, float y) {
  auto& g = static_cast<Graphics2D_wasm&>(g2);
  g.drawTextLayout(_id, x, y);
}

/**********************************************************************************/

sptr<TextLayout> PlatformFactory_wasm::createTextLayout(
  const std::string& src,
  FontStyle style, float size
) {
  std::string font("{");
  font
    .append("\"font-weight\":")
    .append(microtex::isBold(style) ? "\"bold\"" : "\"normal\"");
  font
    .append(1, ',')
    .append("\"font-style\":")
    .append(microtex::isItalic(style) ? "\"italic\"" : "\"normal\"");
  font
    .append(1, ',')
    .append("\"font-size\":")
    .append(std::to_string(size))
    .append(1, ',')
    .append("\"font-family\":");
  if (microtex::isSansSerif(style)) {
    font.append("\"sans-serif\"");
  } else if (microtex::isMono(style)) {
    font.append("\"monospace\"");
  } else {
    font.append("\"serif\"");
  }
  font.append(1, '}');
  const auto id = js_createTextLayout(src.c_str(), font.c_str());
  return sptrOf<TextLayout_wasm>(id);
}

sptr<Font> PlatformFactory_wasm::createFont(const std::string& file) {
  // EMPTY IMPL
  return sptrOf<Font_wasm>();
}

/**********************************************************************************/

Graphics2D_wasm::Graphics2D_wasm() : _color(black), _sx(1), _sy(1) {}

void* Graphics2D_wasm::getDrawingData() {
  return _cmds.finish();
}

void Graphics2D_wasm::setColor(color c) {
  if (_color == c) return;
  _color = c;
  _cmds.put((u8) 0, c);
}

color Graphics2D_wasm::getColor() const { return _color; }

void Graphics2D_wasm::setStroke(const Stroke& s) {
  if (_stroke == s) return;
  _stroke = s;
  _cmds.put((u8) 1, s.lineWidth, s.miterLimit, s.cap, s.join);
}

void Graphics2D_wasm::setStrokeWidth(float w) {
  if (_stroke.lineWidth == w) return;
  _stroke.lineWidth = w;
  _cmds.put((u8) 1, _stroke.lineWidth, _stroke.miterLimit, _stroke.cap, _stroke.join);
}

const Stroke& Graphics2D_wasm::getStroke() const {
  return _stroke;
}

void Graphics2D_wasm::setDash(const std::vector<float>& dash) {
  // TODO only support command, no pattern was given
  const auto hasDash = !dash.empty();
  _cmds.put((u8) 19, hasDash ? (u8) 1 : (u8) 0);
}

std::vector<float> Graphics2D_wasm::getDash() {
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
  _cmds.put((u8) 2, dx, dy);
}

void Graphics2D_wasm::scale(float sx, float sy) {
  _sx *= sx;
  _sy *= sy;
  _cmds.put((u8) 3, sx, sy);
}

void Graphics2D_wasm::rotate(float angle) {
  _cmds.put((u8) 4, angle, 0.f, 0.f);
}

void Graphics2D_wasm::rotate(float angle, float px, float py) {
  _cmds.put((u8) 4, angle, px, py);
}

void Graphics2D_wasm::reset() {
  _sx = _sy = 1.f;
  _cmds.put((u8) 5);
}

float Graphics2D_wasm::sx() const { return _sx; }

float Graphics2D_wasm::sy() const { return _sy; }

void Graphics2D_wasm::drawGlyph(u16 glyph, float x, float y) {}

bool Graphics2D_wasm::beginPath(i32 id) {
  _cmds.put((u8) 17, id);
  // TODO
  return false;
}

void Graphics2D_wasm::moveTo(float x, float y) {
  _cmds.put((u8) 6, x, y);
}

void Graphics2D_wasm::lineTo(float x, float y) {
  _cmds.put((u8) 7, x, y);
}

void Graphics2D_wasm::cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) {
  _cmds.put((u8) 8, x1, y1, x2, y2, x3, y3);
}

void Graphics2D_wasm::quadTo(float x1, float y1, float x2, float y2) {
  _cmds.put((u8) 9, x1, y1, x2, y2);
}

void Graphics2D_wasm::closePath() {
  _cmds.put((u8) 10);
}

void Graphics2D_wasm::fillPath() {
  _cmds.put((u8) 11);
}

void Graphics2D_wasm::drawLine(float x1, float y1, float x2, float y2) {
  _cmds.put((u8) 12, x1, y1, x2, y2);
}

void Graphics2D_wasm::drawRect(float x, float y, float w, float h) {
  _cmds.put((u8) 13, x, y, w, h);
}

void Graphics2D_wasm::fillRect(float x, float y, float w, float h) {
  _cmds.put((u8) 14, x, y, w, h);
}

void Graphics2D_wasm::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _cmds.put((u8) 15, x, y, w, h, rx, ry);
}

void Graphics2D_wasm::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _cmds.put((u8) 16, x, y, w, h, rx, ry);
}

void Graphics2D_wasm::drawTextLayout(unsigned int id, float x, float y) {
  _cmds.put((u8) 18, id, x, y);
}
