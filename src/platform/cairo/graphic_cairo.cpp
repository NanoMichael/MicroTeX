#ifdef __linux__

#include "platform/cairo/graphic_cairo.h"

#include <fontconfig/fontconfig.h>

using namespace tex;
using namespace std;

map<string, string> Font_cairo::_file_name_map;

Font_cairo::Font_cairo() :
	_slant(Pango::STYLE_NORMAL), _weight(Pango::WEIGHT_NORMAL),
	_size(1.), _family("") {}

Font_cairo::Font_cairo(const string& name, int style, float size) :
	_family(name), _size((double) size) {
	convertStyle(style);
}

Font_cairo::Font_cairo(const string& file, float size) :
	_slant(Pango::STYLE_NORMAL), _weight(Pango::WEIGHT_NORMAL), 
	_size((double) size) {
	loadFont(file);
}

void Font_cairo::convertStyle(int style) {
	_slant = Pango::STYLE_NORMAL;
	_weight = Pango::WEIGHT_NORMAL;
	switch(style) {
	case BOLD:
		_weight = Pango::WEIGHT_BOLD;
		break;
	case ITALIC:
		_slant = Pango::STYLE_ITALIC;
		break;
	case BOLDITALIC:
		_weight = Pango::WEIGHT_BOLD;
		_slant = Pango::STYLE_ITALIC;
		break;
	default:
		break;
	}
}

void Font_cairo::loadFont(const string& file) {
	auto it = _file_name_map.find(file);
	if (it != _file_name_map.end()) {
#ifdef __DEBUG
		__log << file << " already loaded, skip\n";
#endif
		return;
	}
	// load font to fontconfig
	const FcChar8* f = (const FcChar8*) file.c_str();
	FcBool status = FcConfigAppFontAddFile(FcConfigGetCurrent(), f);
	if (status) {
#ifdef __DEBUG
		__log << "load " << file << " successfully\n";
#endif
	} else {
#ifdef __DEBUG
		__log << "load " << file << " failed\n";
#endif
	}
	// why get font name is so fucking diffcult?
	// FIXIT
	// Temporarily use this method, get the font file name from
	// path, the file name is the font name, we use "/" as file
	// separator here
	size_t lastFileSepIdx = file.find_last_of("/");
	if (lastFileSepIdx == string::npos) {
		lastFileSepIdx = 0;
	}
	size_t lastDotIdx = file.find_last_of(".");
	if (lastDotIdx == string::npos) {
		lastDotIdx = file.length();
	}
	size_t len = lastDotIdx - lastFileSepIdx - 1;
	string name = file.substr(lastFileSepIdx + 1, len);

	_family = name;
}

string Font_cairo::getFamily() const {
	return _family;
}

Pango::Style Font_cairo::getSlant() const {
	return _slant;
}

Pango::Weight Font_cairo::getWeight() const {
	return _weight;
}

float Font_cairo::getSize() const {
	return (float) _size;
}

shared_ptr<Font> Font_cairo::deriveFont(int style) const {
	Font_cairo* f = new Font_cairo();
	f->_family = _family;
	f->_size = _size;
	f->convertStyle(style);
	return shared_ptr<Font>(f);
}

bool Font_cairo::operator==(const Font& ft) const {
	const Font_cairo& f = static_cast<const Font_cairo&>(ft);
	return _slant == f._slant && _weight == f._weight
		&& _size == f._size && _family == f._family;
}

bool Font_cairo::operator!=(const Font& f) const {
	return !(*this == f);
}

Font* Font::create(const string& file, float size) {
	return new Font_cairo(file, size);
}

shared_ptr<Font> Font::_create(const string& name, int style, float size) {
	return shared_ptr<Font>(new Font_cairo(name, style, size));
}

/******************************************************************************/

Cairo::RefPtr<Cairo::Context> TextLayout_cairo::_img_context;

TextLayout_cairo::TextLayout_cairo(const wstring& src, const shared_ptr<Font_cairo>& f) {
	if (!_img_context) {
		auto surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 1, 1);
		_img_context = Cairo::Context::create(surface);
	}

	Pango::FontDescription fd;
	fd.set_family(f->getFamily());
	fd.set_style(f->getSlant());
	fd.set_weight(f->getWeight());
	fd.set_absolute_size(f->getSize() * Pango::SCALE);

	_layout = Pango::Layout::create(_img_context);
	_layout->set_text(wide2utf8(src.c_str()));
	_layout->set_font_description(fd);
}

void TextLayout_cairo::getBounds(_out_ Rect& r) {
	int w, h;
	_layout->get_pixel_size(w, h);
	r.x = 0;
	r.y = 0;
	r.w = (float) w;
	r.h = (float) h;
}

void TextLayout_cairo::draw(Graphics2D& g2, float x, float y) {
	g2.translate(x, y);
	Graphics2D_cairo& g = static_cast<Graphics2D_cairo&>(g2);
	_layout->show_in_cairo_context(g.getCairoContext());
	g2.translate(-x, -y);
}

shared_ptr<TextLayout> TextLayout::create(const wstring& src, const shared_ptr<Font>& font) {
	shared_ptr<Font_cairo> f = static_pointer_cast<Font_cairo>(font);
	return shared_ptr<TextLayout>(new TextLayout_cairo(src, f));
}

/******************************************************************************/

Font_cairo Graphics2D_cairo::_default_font = Font_cairo("SansSerif", PLAIN, 20.f);

Graphics2D_cairo::Graphics2D_cairo(const Cairo::RefPtr<Cairo::Context>& context) :
	_context(context), _font(&_default_font) {
	_layout = Pango::Layout::create(_context);
	memset(_t, 0, sizeof(float) * 7);
	_t[SX] = _t[SY] = 1.f;
}

const Cairo::RefPtr<Cairo::Context>& Graphics2D_cairo::getCairoContext() const {
	return _context;
}

void Graphics2D_cairo::setColor(color c) {
	_color = c;
	const double a = color_a(c) / 255.;
	const double r = color_r(c) / 255.;
	const double g = color_g(c) / 255.;
	const double b = color_b(c) / 255.;
	_context->set_source_rgba(r, g, b, a);
}

color Graphics2D_cairo::getColor() const {
	return _color;
}

void Graphics2D_cairo::setStroke(const Stroke& s) {
	_stroke = s;
	_context->set_line_width((double) s.lineWidth);

	// convert abstract line cap to platform line cap
	Cairo::LineCap c;
	switch(s.cap) {
	case CAP_BUTT:
		c = Cairo::LINE_CAP_BUTT;
		break;
	case CAP_ROUND:
		c = Cairo::LINE_CAP_ROUND;
		break;
	case CAP_SQUARE:
		c = Cairo::LINE_CAP_SQUARE;
		break;
	}
	_context->set_line_cap(c);

	// conver abstract line join to platform line join
	Cairo::LineJoin j;
	switch(s.join) {
	case JOIN_BEVEL:
		j = Cairo::LINE_JOIN_BEVEL;
		break;
	case JOIN_ROUND:
		j = Cairo::LINE_JOIN_ROUND;
		break;
	case JOIN_MITER:
		j = Cairo::LINE_JOIN_MITER;
		break;
	}
	_context->set_line_join(j);

	_context->set_miter_limit((double) s.miterLimit);
}

const Stroke& Graphics2D_cairo::getStroke() const {
	return _stroke;
}

void Graphics2D_cairo::setStrokeWidth(float w) {
	_stroke.lineWidth = w;
	_context->set_line_width((double) w);
}

const Font* Graphics2D_cairo::getFont() const {
	return _font;
}

void Graphics2D_cairo::setFont(const Font* font) {
	_font = static_cast<const Font_cairo*>(font);
}

void Graphics2D_cairo::translate(float dx, float dy) {
	_t[TX] += _t[SX] * dx;
	_t[TY] += _t[SY] * dy;
	_context->translate((double) dx, (double) dy);
}

void Graphics2D_cairo::scale(float sx, float sy) {
	_t[SX] *= sx;
	_t[SY] *= sy;
	_context->scale((double) sx, (double) sy);
}

void Graphics2D_cairo::rotate(float angle) {
	_t[R] += angle;
	_context->rotate(angle);
}

void Graphics2D_cairo::rotate(float angle, float px, float py) {
	_t[R] += angle;
	_t[PX] = px * _t[SX] + _t[TX];
	_t[PY] = py * _t[SY] + _t[TY];
	_context->translate((double) px, (double) py);
	_context->rotate(angle);
	_context->translate((double) -px, (double) -py);
}

void Graphics2D_cairo::reset() {
	_context->set_identity_matrix();
	memset(_t, 0, sizeof(float) * 7);
	_t[SX] = _t[SY] = 1.f;
}

float Graphics2D_cairo::sx() const {
	return _t[SX];
}

float Graphics2D_cairo::sy() const {
	return _t[SY];
}

float Graphics2D_cairo::tx() const {
	return _t[TX];
}

float Graphics2D_cairo::ty() const {
	return _t[TY];
}

float Graphics2D_cairo::r() const {
	return _t[R];
}

float Graphics2D_cairo::px() const {
	return _t[PX];
}

float Graphics2D_cairo::py() const {
	return _t[PY];
}

void Graphics2D_cairo::drawChar(wchar_t c, float x, float y) {
	wstring str = {c, L'\0'};
	drawText(str, x, y);
}

void Graphics2D_cairo::drawText(const wstring& t, float x, float y) {
	Pango::FontDescription f;
	f.set_family(_font->getFamily());
	f.set_style(_font->getSlant());
	f.set_weight(_font->getWeight());
	f.set_absolute_size(_font->getSize() * Pango::SCALE);

	string str = wide2utf8(t.c_str());
	_layout->set_text(str);
	_layout->set_font_description(f);

	_context->move_to(x, y);
	_layout->show_in_cairo_context(_context);
}

void Graphics2D_cairo::drawLine(float x1, float y1, float x2, float y2) {
	_context->move_to(x1, y1);
	_context->line_to(x2, y2);
	_context->stroke();
}

void Graphics2D_cairo::drawRect(float x, float y, float w, float h) {
	_context->rectangle(x, y, w, h);
	_context->stroke();
}

void Graphics2D_cairo::fillRect(float x, float y, float w, float h) {
	_context->rectangle(x, y, w, h);
	_context->fill();
}

void Graphics2D_cairo::roundRect(float x, float y, float w, float h, float rx, float ry) {
	double r = max(rx, ry);
	double d = M_PI / 180.;
	_context->begin_new_sub_path();
	_context->arc(x + r, y + r, r, 180 * d, 270 * d);
	_context->arc(x + w - r, y + r, r, -90 * d, 0);
	_context->arc(x + w - r, y + h - r, r, 0, 90 * d);
	_context->arc(x + r, y + h - r, r, 90 * d, 180 * d);
	_context->close_path();
}

void Graphics2D_cairo::drawRoundRect(float x, float y, float w, float h, float rx, float ry) {
	roundRect(x, y, w, h, rx, ry);
	_context->stroke();
}

void Graphics2D_cairo::fillRoundRect(float x, float y, float w, float h, float rx, float ry) {
	roundRect(x, y, w, h, rx, ry);
	_context->fill();
}

#endif
