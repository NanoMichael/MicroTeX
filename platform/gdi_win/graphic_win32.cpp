#include "graphic_win32.h"

#include "utils/exceptions.h"
#include "utils/nums.h"
#include "utils/utils.h"
// fix error C4430: missing type specifier - int assumed. Note: C++ does not support default-int
#include <comdef.h>
#include <gdiplus.h>
#include <windows.h>

using namespace std;
using namespace microtex;

std::wstring microtex::win32ToWideString(const std::string& str) {
  std::wstring wstr;
  auto l = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
  wstr.resize(l + 10);
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], (int)wstr.size());
  return wstr;
}

/**************************************************************************************************/

std::map<std::string, sptr<Font_win32>> Font_win32::_win32Faces;

sptr<Font_win32> Font_win32::getOrCreate(const std::string& file) {
  auto it = _win32Faces.find(file);
  if (it != _win32Faces.end()) {
    return it->second;
  }
  // add font to font collection, some like fontconfig
  Gdiplus::PrivateFontCollection c;
  const auto& wfile = win32ToWideString(file);
  c.AddFontFile(wfile.c_str());
  auto ff = new Gdiplus::FontFamily();
  int num = 0;
  c.GetFamilies(1, ff, &num);
  if (num <= 0) {
    throw microtex::ex_invalid_state("Cannot load font file " + file);
  }
  // search order:
  // regular -> bold -> italic -> bold-italic
  int style;
  if (ff->IsStyleAvailable(Gdiplus::FontStyleRegular)) {
    style = Gdiplus::FontStyleRegular;
  } else if (ff->IsStyleAvailable(Gdiplus::FontStyleBold)) {
    style = Gdiplus::FontStyleBold;
  } else if (ff->IsStyleAvailable(Gdiplus::FontStyleItalic)) {
    style = Gdiplus::FontStyleItalic;
  } else if (ff->IsStyleAvailable(Gdiplus::FontStyleBoldItalic)) {
    style = Gdiplus::FontStyleBoldItalic;
  } else {
    throw microtex::ex_invalid_state("No available font in file " + file);
  }
  auto f = sptrOf<Font_win32>(ff, style);
  _win32Faces[file] = f;
  return f;
}

Font_win32::Font_win32(const Gdiplus::FontFamily* family, int style, bool isSystem)
    : _family(family), _style(style), _isSystem(isSystem) {}

sptr<Gdiplus::Font> Font_win32::getFont(float size) {
  auto f = new Gdiplus::Font(_family, size, _style, Gdiplus::UnitPixel);
  return sptr<Gdiplus::Font>(f);
}

int Font_win32::getEmHeight() {
  return _family->GetEmHeight(_style);
}

int Font_win32::getCellAscent() {
  return _family->GetCellAscent(_style);
}

bool Font_win32::operator==(const Font& ft) const {
  const auto& f = static_cast<const Font_win32&>(ft);
  return f._family == _family;
}

Font_win32::~Font_win32() noexcept {
  if (!_isSystem) delete _family;
}

/**************************************************************************************************/

Gdiplus::Graphics* TextLayout_win32::_g = nullptr;
Gdiplus::Bitmap* TextLayout_win32::_img = nullptr;
const Gdiplus::StringFormat* TextLayout_win32::_format = nullptr;

TextLayout_win32::TextLayout_win32(const std::string& src, FontStyle style, float size)
    : _fontSize(size) {
  if (_img == nullptr) {
    _img = new Gdiplus::Bitmap(1, 1, PixelFormat32bppARGB);
    _g = Gdiplus::Graphics::FromImage(_img);
    _format = Gdiplus::StringFormat::GenericTypographic();
  }
  const auto* f = Gdiplus::FontFamily::GenericSerif();
  if (microtex::isSansSerif(style)) {
    f = Gdiplus::FontFamily::GenericSansSerif();
  }
  if (microtex::isMono(style)) {
    f = Gdiplus::FontFamily::GenericMonospace();
  }
  int s = Gdiplus::FontStyleRegular;
  if (microtex::isBold(style)) {
    s |= Gdiplus::FontStyleBold;
  }
  if (microtex::isItalic(style)) {
    s |= Gdiplus::FontStyleItalic;
  }
  if (!f->IsStyleAvailable(s)) {
    s = Gdiplus::FontStyleRegular;
  }
  _font = sptrOf<Font_win32>(f, s, true);
  _txt = win32ToWideString(src);
}

void TextLayout_win32::getBounds(Rect& r) {
  int em = _font->getEmHeight();
  int ascent = _font->getCellAscent();
  float ap = _fontSize * ascent / em;
  auto f = _font->getFont(_fontSize);
  Gdiplus::RectF r1;
  _g->MeasureString(
    _txt.c_str(),
    (int)wcslen(_txt.c_str()),
    f.get(),
    Gdiplus::PointF(0, 0),
    _format,
    &r1
  );
  r.x = 0;
  r.y = -ap;
  r.w = r1.Width;
  r.h = r1.Height;
}

void TextLayout_win32::draw(Graphics2D& g2, float x, float y) {
  auto& g = static_cast<Graphics2D_win32&>(g2);
  auto prev = g2.getFont();
  auto prevSize = g2.getFontSize();
  g.setFont(_font);
  g.setFontSize(_fontSize);
  g.drawText(_txt, x, y);
  g.setFontSize(prevSize);
  g.setFont(prev);
}

/**************************************************************************************************/

sptr<microtex::Font> PlatformFactory_gdi::createFont(const std::string& file) {
  return Font_win32::getOrCreate(file);
}

sptr<TextLayout>
PlatformFactory_gdi::createTextLayout(const std::string& src, FontStyle style, float size) {
  return sptrOf<TextLayout_win32>(src, style, size);
}

/**************************************************************************************************/

const Gdiplus::StringFormat* Graphics2D_win32::_format;

Graphics2D_win32::Graphics2D_win32(Gdiplus::Graphics* g) {
  if (_format == nullptr) {
    _format = Gdiplus::StringFormat::GenericTypographic();
  }
  _fontSize = 0.f;
  _sx = _sy = 1.f;
  _color = black;
  _g = g;
  _g->ResetTransform();
  _g->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
  _g->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
  _pen = new Gdiplus::Pen(Gdiplus::Color(_color));
  _brush = new Gdiplus::SolidBrush(Gdiplus::Color(_color));
}

Graphics2D_win32::~Graphics2D_win32() {
  delete _pen;
  delete _brush;
}

void Graphics2D_win32::setColor(color color) {
  _color = color;
  _pen->SetColor(Gdiplus::Color(color));
  _brush->SetColor(Gdiplus::Color(color));
}

color Graphics2D_win32::getColor() const {
  return _color;
}

void Graphics2D_win32::setStroke(const Stroke& s) {
  _stroke = s;
  _pen->SetWidth(s.lineWidth);
  Gdiplus::LineCap c;
  switch (s.cap) {
    case CAP_BUTT: c = Gdiplus::LineCapFlat; break;
    case CAP_ROUND: c = Gdiplus::LineCapRound; break;
    case CAP_SQUARE: c = Gdiplus::LineCapSquare; break;
  }
  _pen->SetLineCap(c, c, Gdiplus::DashCapRound);
  Gdiplus::LineJoin j;
  switch (s.join) {
    case JOIN_BEVEL: j = Gdiplus::LineJoinBevel; break;
    case JOIN_ROUND: j = Gdiplus::LineJoinRound; break;
    case JOIN_MITER: j = Gdiplus::LineJoinMiter; break;
  }
  _pen->SetLineJoin(j);
  _pen->SetMiterLimit(s.miterLimit);
}

const Stroke& Graphics2D_win32::getStroke() const {
  return _stroke;
}

void Graphics2D_win32::setStrokeWidth(float w) {
  _stroke.lineWidth = w;
  _pen->SetWidth(w);
}

void Graphics2D_win32::setDash(const std::vector<float>& dash) {
  // todo
}

std::vector<float> Graphics2D_win32::getDash() {
  // todo
  return {};
}

sptr<microtex::Font> Graphics2D_win32::getFont() const {
  return _font;
}

void Graphics2D_win32::setFont(const sptr<Font>& font) {
  _font = std::static_pointer_cast<Font_win32>(font);
}

float Graphics2D_win32::getFontSize() const {
  return _fontSize;
}

void Graphics2D_win32::setFontSize(float size) {
  _fontSize = size;
}

void Graphics2D_win32::translate(float dx, float dy) {
  _g->TranslateTransform(dx, dy);
}

void Graphics2D_win32::scale(float sx, float sy) {
  _sx *= sx;
  _sy *= sy;
  _g->ScaleTransform(sx, sy);
}

void Graphics2D_win32::rotate(float angle) {
  _g->RotateTransform(angle / PI * 180);
}

void Graphics2D_win32::rotate(float angle, float px, float py) {
  _g->TranslateTransform(px, py);
  _g->RotateTransform(angle / PI * 180);
  _g->TranslateTransform(-px, -py);
}

void Graphics2D_win32::reset() {
  _g->ResetTransform();
  _sx = _sy = 1.f;
}

float Graphics2D_win32::sx() const {
  return _sx;
}

float Graphics2D_win32::sy() const {
  return _sy;
}

void Graphics2D_win32::drawGlyph(u16 glyph, float x, float y) {
  if (_font == nullptr) return;
  auto f = _font->getFont(_fontSize);
  auto p = Gdiplus::PointF(x, y);
  _g->DrawDriverString(&glyph, 1, f.get(), _brush, &p, 0, nullptr);
}

bool Graphics2D_win32::beginPath(i32 id) {
  // not supported
  return false;
}

void Graphics2D_win32::moveTo(float x, float y) {
  // not supported
}

void Graphics2D_win32::lineTo(float x, float y) {
  // not supported
}

void Graphics2D_win32::cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) {
  // not supported
}

void Graphics2D_win32::quadTo(float x1, float y1, float x2, float y2) {
  // not supported
}

void Graphics2D_win32::closePath() {
  // not supported
}

void Graphics2D_win32::fillPath(i32 id) {
  // not supported
}

void Graphics2D_win32::drawText(const std::wstring& src, float x, float y) {
  auto f = std::static_pointer_cast<Font_win32>(_font);
  int em = f->getEmHeight();
  int ascent = f->getCellAscent();
  float ap = _fontSize * ascent / em;
  int len = src.length();
  auto font = _font->getFont(_fontSize);
  Gdiplus::RectF r1, r2;
  _g->MeasureString(src.c_str(), len, font.get(), Gdiplus::PointF(0, 0), &r1);
  _g->MeasureString(src.c_str(), len, font.get(), Gdiplus::PointF(0, 0), _format, &r2);
  float off = (r1.Width - r2.Width) / 2.f;
  _g->DrawString(src.c_str(), len, font.get(), Gdiplus::PointF(x - off, y - ap), _brush);
}

void Graphics2D_win32::drawLine(float x1, float y1, float x2, float y2) {
  _g->DrawLine(_pen, x1, y1, x2, y2);
}

void Graphics2D_win32::drawRect(float x, float y, float w, float h) {
  _g->DrawRectangle(_pen, x, y, w, h);
}

void Graphics2D_win32::fillRect(float x, float y, float w, float h) {
  _g->FillRectangle(_brush, x, y, w, h);
}

void Graphics2D_win32::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
  // not supported
  drawRect(x, y, w, h);
}

void Graphics2D_win32::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
  // not supported
  fillRect(x, y, w, h);
}
