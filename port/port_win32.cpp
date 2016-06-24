#ifdef _WIN32

#if defined (__clang__)
#include "port.h"
#elif defined (__GNUC__)
#include "port/port.h"
#endif // defined
#include "common.h"
#include <sstream>

#include <windows.h>
#include <gdiplus.h>

using namespace std;
using namespace tex;
using namespace tex::port;

/**************** font *************/

int Font::convertStyle(int style) {
	int s;
	switch (style) {
	case PLAIN:
		s = Gdiplus::FontStyleRegular;
		break;
	case BOLD:
		s = Gdiplus::FontStyleBold;
		break;
	case ITALIC:
		s = Gdiplus::FontStyleItalic;
		break;
	case BOLDITALIC:
		s = Gdiplus::FontStyleBoldItalic;
		break;
	default:
		s = -1;
		break;
	}
	return s;
}

const Gdiplus::FontFamily* Font::_serif;
const Gdiplus::FontFamily* Font::_sansserif;

Font::~Font() {
	if (_family == _serif || _family == _sansserif)
		return;
	delete _family;
}

Font::Font() : _size(0) {}

Font::Font(const string& name, int style, float size) : _size(size) {
	if (_serif == nullptr) {
		_serif = Gdiplus::FontFamily::GenericSerif();
		_sansserif = Gdiplus::FontFamily::GenericSansSerif();
	}
	const Gdiplus::FontFamily* f = nullptr;
	if (name == "Serif" || name == "SansSerif") {
		if (name == "Serif")
			f = _serif;
		else
			f = _sansserif;
	} else {
		wstring wname = utf82wide(name.c_str());
		f = new Gdiplus::FontFamily(wname.c_str());
	}
	int s = convertStyle(style);
	if (!f->IsStyleAvailable(s)) {
		throw ex_invalid_state("specified font style not available!");
	}
	Gdiplus::Font* tf = new Gdiplus::Font(f, _size, s, Gdiplus::UnitPixel);
	_style = s;
	_family = f;
	_typeface = shared_ptr<Gdiplus::Font>(tf);
}

float Font::getSize() const {
	return _size;
}

Font Font::deriveFont(int style) const {
	if (style == _style)
		return *this;
	int s = convertStyle(style);
	if (!_family->IsStyleAvailable(s))
		throw ex_invalid_state("specified font style not available!");
	Font f;
	f._family = _family;
	f._style = s;
	f._size = _size;
	Gdiplus::Font* ff = new Gdiplus::Font(&(*_family), _size, s, Gdiplus::UnitPixel);
	f._typeface = shared_ptr<Gdiplus::Font>(ff);
	return f;
}

bool Font::operator==(const Font& f) const {
	return _typeface.get() == f._typeface.get();
}

bool Font::operator!=(const Font& f) const {
	return !(*this == f);
}

Font* Font::create(const string& file, float s) {
	Gdiplus::PrivateFontCollection c;
	wstring wfile = utf82wide(file.c_str());
	c.AddFontFile(wfile.c_str());
	Gdiplus::FontFamily* ff = new Gdiplus::FontFamily();
	int num = 0;
	c.GetFamilies(1, ff, &num);
	if (num <= 0)
		throw ex_invalid_state("cannot load font file " + file);
	// search order :
	// regular -> bold -> italic -> bold-italic
	Font* font = new Font();
	font->_size = s;
	font->_family = ff;
	Gdiplus::Font* f = nullptr;
	if (ff->IsStyleAvailable(Gdiplus::FontStyleRegular)) {
		f = new Gdiplus::Font(ff, s, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
		font->_style = Gdiplus::FontStyleRegular;
	} else if (ff->IsStyleAvailable(Gdiplus::FontStyleBold)) {
		f = new Gdiplus::Font(ff, s, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		font->_style = Gdiplus::FontStyleBold;
	} else if (ff->IsStyleAvailable(Gdiplus::FontStyleItalic)) {
		f = new Gdiplus::Font(ff, s, Gdiplus::FontStyleItalic, Gdiplus::UnitPixel);
		font->_style = Gdiplus::FontStyleItalic;
	} else if (ff->IsStyleAvailable(Gdiplus::FontStyleBoldItalic)) {
		f = new Gdiplus::Font(ff, s, Gdiplus::FontStyleBoldItalic, Gdiplus::UnitPixel);
		font->_style = Gdiplus::FontStyleBoldItalic;
	} else {
		delete font;
		throw ex_invalid_state("no available font in file " + file);
	}
	font->_typeface = shared_ptr<Gdiplus::Font>(f);
	return font;
}

/****************************************************
 *               Graphics2D implementation          *
 ****************************************************/

const Gdiplus::StringFormat* Graphics2D::_format = nullptr;
const Font*  Graphics2D::_defaultFont = nullptr;

Graphics2D::Graphics2D(Gdiplus::Graphics* g) {
	if (_format == nullptr)
		_format = Gdiplus::StringFormat::GenericTypographic();
	if (_defaultFont == nullptr)
		_defaultFont = new Font("Arial", PLAIN, 72.f);
	_t = new float[9]();
	_t[SX] = _t[SY] = 1;
	_color = black;
	_font = _defaultFont;
	_g = g;
	_g->ResetTransform();
	_g->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	_g->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	_pen = new Gdiplus::Pen(Gdiplus::Color(_color));
	_brush = new Gdiplus::SolidBrush(Gdiplus::Color(_color));
}

Graphics2D::~Graphics2D() {
	delete _t;
	delete _pen;
	delete _brush;
}

void Graphics2D::setColor(color color) {
	_color = color;
	_pen->SetColor(Gdiplus::Color(color));
	_brush->SetColor(Gdiplus::Color(color));
}

color Graphics2D::getColor() const {
	return _color;
}

void Graphics2D::setStroke(const Stroke& s) {
	_stroke = s;
	_pen->SetWidth(s.lineWidth);
	Gdiplus::LineCap c;
	switch (s.cap) {
	case CAP_BUTT:
		c = Gdiplus::LineCapFlat;
		break;
	case CAP_ROUND:
		c = Gdiplus::LineCapRound;
		break;
	case CAP_SQUARE:
		c = Gdiplus::LineCapSquare;
		break;
	}
	_pen->SetLineCap(c, c, Gdiplus::DashCapRound);
	Gdiplus::LineJoin j;
	switch (s.join) {
	case JOIN_BEVEL:
		j = Gdiplus::LineJoinBevel;
		break;
	case JOIN_ROUND:
		j = Gdiplus::LineJoinRound;
		break;
	case JOIN_MITER:
		j = Gdiplus::LineJoinMiter;
		break;
	}
	_pen->SetLineJoin(j);
	_pen->SetMiterLimit(s.miterLimit);
}

const Stroke& Graphics2D::getStroke() const {
	return _stroke;
}

void Graphics2D::setStrokeWidth(float w) {
	_stroke.lineWidth = w;
	_pen->SetWidth(w);
}

const Font* Graphics2D::getFont() const {
	return _font;
}

void Graphics2D::setFont(const Font* font) {
	_font = font;
}

void Graphics2D::translate(float dx, float dy) {
	_t[TX] += _t[SX] * dx;
	_t[TY] += _t[SY] * dy;
	_g->TranslateTransform(dx, dy);
}

void Graphics2D::scale(float sx, float sy) {
	_t[SX] *= sx;
	_t[SY] *= sy;
	_g->ScaleTransform(sx, sy);
}

void Graphics2D::rotate(float angle) {
	float r = (float) (angle / PI * 180);
	_t[R] += r;
	_g->RotateTransform(r);
}

void Graphics2D::rotate(float angle, float px, float py) {
	float r = (float) (angle / PI * 180);
	_t[R] += r;
	_t[PX] = px * _t[SX] + _t[TX];
	_t[PY] = py * _t[SY] + _t[TY];
	_g->TranslateTransform(px, py);
	_g->RotateTransform(r);
	_g->TranslateTransform(-px, -py);
}

void Graphics2D::reset() {
	_g->ResetTransform();
	memset(_t, 0, sizeof(float) * 9);
	_t[SX] = _t[SY] = 1;
}

float Graphics2D::sx() const {
	return _t[SX];
}

float Graphics2D::sy() const {
	return _t[SY];
}

float Graphics2D::tx() const {
	return _t[TX];
}

float Graphics2D::ty() const {
	return _t[TY];
}

float Graphics2D::r() const {
	return _t[R];
}

float Graphics2D::px() const {
	return _t[PX];
}

float Graphics2D::py() const {
	return _t[PY];
}

void Graphics2D::drawChar(wchar_t c, float x, float y) {
	wchar_t str[] { c, L'\0' };
	drawText(str, x, y);
}

void Graphics2D::drawText(const wstring& c, float x, float y) {
	const wchar_t* str = c.c_str();
	int em = _font->_family->GetEmHeight(_font->_style);
	int ascent = _font->_family->GetCellAscent(_font->_style);
	float ap = _font->getSize() * ascent / em;
	int len = wcslen(str);
	Gdiplus::RectF r1, r2;
	_g->MeasureString(str, len, &(*_font->_typeface), Gdiplus::PointF(0, 0), &r1);
	_g->MeasureString(str, len, &(*_font->_typeface), Gdiplus::PointF(0, 0), _format, &r2);
	float off = (r1.Width - r2.Width) / 2.f;
	_g->DrawString(str, len, &(*_font->_typeface), Gdiplus::PointF(x - off, y - ap), _brush);
}

void Graphics2D::drawLine(float x1, float y1, float x2, float y2) {
	_g->DrawLine(_pen, x1, y1, x2, y2);
}

void Graphics2D::drawRect(float x, float y, float w, float h) {
	_g->DrawRectangle(_pen, x, y, w, h);
}

void Graphics2D::fillRect(float x, float y, float w, float h) {
	_g->FillRectangle(_brush, x, y, w, h);
}

void Graphics2D::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
	// not supported
	drawRect(x, y, w, h);
}

void Graphics2D::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
	// not supported
	fillRect(x, y, w, h);
}

Gdiplus::Graphics* TextLayout::_g = nullptr;
Gdiplus::Bitmap* TextLayout::_img = nullptr;
const Gdiplus::StringFormat* TextLayout::_format = nullptr;

TextLayout::~TextLayout() {}

TextLayout::TextLayout() {
	if (_img == nullptr) {
		_img = new Gdiplus::Bitmap(1, 1, PixelFormat32bppARGB);
		_g = Gdiplus::Graphics::FromImage(_img);
		_format = Gdiplus::StringFormat::GenericTypographic();
	}
}

TextLayout::TextLayout(const wstring& src, const Font& font) : _txt(src), _font(font) {
	if (_img == nullptr) {
		_img = new Gdiplus::Bitmap(1, 1, PixelFormat32bppARGB);
		_g = Gdiplus::Graphics::FromImage(_img);
		_format = Gdiplus::StringFormat::GenericTypographic();
	}
}

Rect TextLayout::getBounds() {
	int em = _font._family->GetEmHeight(_font._style);
	int ascent = _font._family->GetCellAscent(_font._style);
	float ap = _font.getSize() * ascent / em;
	Gdiplus::RectF r1;
	_g->MeasureString(_txt.c_str(), wcslen(_txt.c_str()), &(*_font._typeface), Gdiplus::PointF(0, 0), _format, &r1);
	Rect r;
	r.x = 0;
	r.y = -ap;
	r.w = r1.Width;
	r.h = r1.Height;
	return r;
}

void TextLayout::draw(Graphics2D& g2, float x, float y) {
	const Font* prev = g2.getFont();
	g2.setFont(&_font);
	g2.drawText(_txt, x, y);
	g2.setFont(prev);
}

#endif
