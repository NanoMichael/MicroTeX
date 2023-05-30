#ifdef HAVE_CWRAPPER

#include "wrapper/graphic_wrapper.h"

#include "wrapper/callback.h"

using namespace microtex;

Font_wrapper::Font_wrapper(std::string name) : _name(std::move(name)) {}

const std::string& Font_wrapper::name() const {
  return _name;
}

bool Font_wrapper::operator==(const Font& f) const {
  return _name == static_cast<const Font_wrapper&>(f)._name;
}

/**************************************************************************************************/

TextLayout_wrapper::TextLayout_wrapper(u32 id) : _id(id) {}

TextLayout_wrapper::~TextLayout_wrapper() noexcept {
  microtex_releaseTextLayout(_id);
}

void TextLayout_wrapper::getBounds(Rect& bounds) {
  TextLayoutBounds b{0, 0, 0};
  microtex_getTextLayoutBounds(_id, &b);
  bounds.x = 0;
  bounds.w = b.width;
  bounds.h = b.height;
  bounds.y = b.ascent;
}

void TextLayout_wrapper::draw(Graphics2D& g2, float x, float y) {
  auto& g = static_cast<Graphics2D_wrapper&>(g2);
  g.drawTextLayout(_id, x, y);
}

/**************************************************************************************************/

sptr<TextLayout>
PlatformFactory_wrapper::createTextLayout(const std::string& src, FontStyle style, float size) {
  FontDesc f{
    microtex::isBold(style),
    microtex::isItalic(style),
    microtex::isSansSerif(style),
    microtex::isMono(style),
    size,
  };
  const auto id = microtex_createTextLayout(src.c_str(), &f);
  return sptrOf<TextLayout_wrapper>(id);
}

sptr<Font> PlatformFactory_wrapper::createFont(const std::string& file) {
  return sptrOf<Font_wrapper>(file);
}

/**************************************************************************************************/

Graphics2D_wrapper::Graphics2D_wrapper() : _color(black), _sx(1), _sy(1) {}

void* Graphics2D_wrapper::getDrawingData() {
  return _cmds.finish();
}

void Graphics2D_wrapper::setColor(color c) {
  if (_color == c) return;
  _color = c;
  _cmds.put((u8)0, c);
}

color Graphics2D_wrapper::getColor() const {
  return _color;
}

void Graphics2D_wrapper::setStroke(const Stroke& s) {
  if (_stroke == s) return;
  _stroke = s;
  _cmds.put((u8)1, s.lineWidth, s.miterLimit, (u32)s.cap, (u32)s.join);
}

void Graphics2D_wrapper::setStrokeWidth(float w) {
  if (_stroke.lineWidth == w) return;
  _stroke.lineWidth = w;
  _cmds.put((u8)1, _stroke.lineWidth, _stroke.miterLimit, (u32)_stroke.cap, (u32)_stroke.join);
}

const Stroke& Graphics2D_wrapper::getStroke() const {
  return _stroke;
}

void Graphics2D_wrapper::setDash(const std::vector<float>& dash) {
  // TODO only support command, no pattern was given
  const auto hasDash = !dash.empty();
  _cmds.put((u8)2, hasDash ? (u8)1 : (u8)0);
}

std::vector<float> Graphics2D_wrapper::getDash() {
  return {};
}

sptr<Font> Graphics2D_wrapper::getFont() const {
  return _font;
}

void Graphics2D_wrapper::setFont(const sptr<Font>& font) {
  if (_font != nullptr && *_font == *font) return;
  _font = font;
  _cmds.put((u8)3, std::static_pointer_cast<Font_wrapper>(_font)->name().c_str());
}

float Graphics2D_wrapper::getFontSize() const {
  return _fontSize;
}

void Graphics2D_wrapper::setFontSize(float size) {
  if (_fontSize == size) return;
  _cmds.put((u8)4, size);
}

void Graphics2D_wrapper::translate(float dx, float dy) {
  _cmds.put((u8)5, dx, dy);
}

void Graphics2D_wrapper::scale(float sx, float sy) {
  _sx *= sx;
  _sy *= sy;
  _cmds.put((u8)6, sx, sy);
}

void Graphics2D_wrapper::rotate(float angle) {
  _cmds.put((u8)7, angle, 0.f, 0.f);
}

void Graphics2D_wrapper::rotate(float angle, float px, float py) {
  _cmds.put((u8)7, angle, px, py);
}

void Graphics2D_wrapper::reset() {
  _sx = _sy = 1.f;
  _cmds.put((u8)8);
}

float Graphics2D_wrapper::sx() const {
  return _sx;
}

float Graphics2D_wrapper::sy() const {
  return _sy;
}

void Graphics2D_wrapper::drawGlyph(u16 glyph, float x, float y) {
  _cmds.put((u8)9, glyph, x, y);
}

bool Graphics2D_wrapper::beginPath(i32 id) {
  _cmds.put((u8)10, id);
  return microtex_isPathExists(id);
}

void Graphics2D_wrapper::moveTo(float x, float y) {
  _cmds.put((u8)11, x, y);
}

void Graphics2D_wrapper::lineTo(float x, float y) {
  _cmds.put((u8)12, x, y);
}

void Graphics2D_wrapper::cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) {
  _cmds.put((u8)13, x1, y1, x2, y2, x3, y3);
}

void Graphics2D_wrapper::quadTo(float x1, float y1, float x2, float y2) {
  _cmds.put((u8)14, x1, y1, x2, y2);
}

void Graphics2D_wrapper::closePath() {
  _cmds.put((u8)15);
}

void Graphics2D_wrapper::fillPath(i32 id) {
  _cmds.put((u8)16, id);
}

void Graphics2D_wrapper::drawLine(float x1, float y1, float x2, float y2) {
  _cmds.put((u8)17, x1, y1, x2, y2);
}

void Graphics2D_wrapper::drawRect(float x, float y, float w, float h) {
  _cmds.put((u8)18, x, y, w, h);
}

void Graphics2D_wrapper::fillRect(float x, float y, float w, float h) {
  _cmds.put((u8)19, x, y, w, h);
}

void Graphics2D_wrapper::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _cmds.put((u8)20, x, y, w, h, rx, ry);
}

void Graphics2D_wrapper::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
  _cmds.put((u8)21, x, y, w, h, rx, ry);
}

void Graphics2D_wrapper::drawTextLayout(u32 id, float x, float y) {
  _cmds.put((u8)22, id, x, y);
}

#endif  // HAVE_CWRAPPER
