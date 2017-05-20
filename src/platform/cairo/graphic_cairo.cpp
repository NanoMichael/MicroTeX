#ifdef __linux__

#include "platform/cairo/graphic_cairo.h"

using namespace tex;

Graphics2D_cairo::Graphics2D_cairo(Cairo::RefPtr<Cairo::Context>& context) :
	_context(context) {}

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

	// convert abstract line cap to platform line c
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
	_font = font;
}

#endif