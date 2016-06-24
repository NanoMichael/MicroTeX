#include "render.h"

#if defined (__clang__)
#include "formula.h"
#include "atom.h"
#include "core.h"
#elif defined (__GNUC__)
#include "core/formula.h"
#include "atom/atom.h"
#include "core/core.h"
#endif // defined

#include "memcheck.h"

using namespace tex;
using namespace tex::core;
using namespace tex::port;
using namespace tex::fonts;

const color TeXRender::_defaultcolor = black;
float TeXRender::_defaultSize = -1;
float TeXRender::_magFactor = 0;

TeXRender::TeXRender(const shared_ptr<Box> b, float s, bool trueValues) {
	_box = b;
	if (_defaultSize != -1)
		_size = _defaultSize;
	if (_magFactor != 0)
		_size = s * abs(_magFactor);
	else
		_size = s;
	if (!trueValues) {
		_insets.top += (int) (0.18f * s);
		_insets.bottom += (int) (0.18f * s);
		_insets.left += (int) (0.18f * s);
		_insets.right += (int) (0.18f * s);
	}
}

float TeXRender::getSize() const {
	return _size;
}

int TeXRender::getHeight() const {
	return (int) (_box->_height * _size + 0.99f + _insets.top + _box->_depth * _size + 0.99f + _insets.bottom);
}

int TeXRender::getDepth() const {
	return (int) (_box->_depth * _size + 0.99f + _insets.bottom);
}

int TeXRender::getWidth() const {
	return (int) (_box->_width * _size + 0.99f + _insets.left + _insets.right);
}

float TeXRender::getBaseline() const {
	return ((_box->_height * _size + 0.99f + _insets.top) / ((_box->_height + _box->_depth) * _size + 0.99f + _insets.top + _insets.bottom));
}

void TeXRender::setSize(float s) {
	_size = s;
}

void TeXRender::setForeground(color fg) {
	_fg = fg;
}

Insets TeXRender::getInsets() {
	return _insets;
}

void TeXRender::setInsets(const Insets& insets, bool trueval) {
	_insets = insets;
	if (!trueval) {
		_insets.top += (int) (0.18f * _size);
		_insets.bottom += (int) (0.18f * _size);
		_insets.left += (int) (0.18f * _size);
		_insets.right += (int) (0.18f * _size);
	}
}

void TeXRender::setWidth(int w, int align) {
	float diff = w - getWidth();
	if (diff > 0)
		_box = shared_ptr<Box>(new HorizontalBox(_box, (float)w, align));
}

void TeXRender::setHeight(int h, int align) {
	float diff = h - getHeight();
	if (diff > 0)
		_box = shared_ptr<Box>(new VerticalBox(_box, diff, align));
}

void TeXRender::draw(_out_ Graphics2D& g2, int x, int y) {
	color old = g2.getColor();
	g2.scale(_size, _size);
	if (!istrans(_fg))
		g2.setColor(_fg);
	else
		g2.setColor(_defaultcolor);

	// draw formula box
	_box->draw(g2, (x + _insets.left) / _size, (y + _insets.top) / _size + _box->_height);

	// restore
	g2.reset();
	g2.setColor(old);
}

DefaultTeXFont* TeXRenderBuilder::createFont(float s, int type) {
	DefaultTeXFont* tf = new DefaultTeXFont(s);
	if (type == 0)
		tf->setSs(false);
	if ((type & ROMAN) != 0)
		tf->setRoman(true);
	if ((type & TYPEWRITER) != 0)
		tf->setTt(true);
	if ((type & SANSSERIF) != 0)
		tf->setSs(true);
	if ((type & ITALIC) != 0)
		tf->setIt(true);
	if ((type & BOLD) != 0)
		tf->setBold(true);
	return tf;
}

TeXRender* TeXRenderBuilder::build(TeXFormula& f) {
	return build(f._root);
}

TeXRender* TeXRenderBuilder::build(const shared_ptr<Atom>& fc) {
	shared_ptr<Atom> f = fc;
	if (f == nullptr)
		f = shared_ptr<Atom>(new EmptyAtom());
	if (_style == -1)
		throw ex_invalid_state("a style is required, use function setStyle");
	if (_size == -1)
		throw ex_invalid_state("a size is required, use function setSize");
	DefaultTeXFont* font = (_type == -1) ? new DefaultTeXFont(_size) : createFont(_size, _type);
	shared_ptr<TeXFont> tf(font);
	TeXEnvironment* te = nullptr;
	if (_widthUnit != -1 && _textWidth != 0)
		te = new TeXEnvironment(_style, tf, _widthUnit, _textWidth);
	else
		te = new TeXEnvironment(_style, tf);

	if (_interlineUnit != -1)
		te->setInterline(_interlineUnit, _interlineSpacing);

	auto box = f->createBox(*te);
	TeXRender* ti = nullptr;
	if (_widthUnit != -1 && _textWidth != 0) {
		HorizontalBox* hb = nullptr;
		if (_interlineUnit != -1 && _interlineSpacing != 0) {
			float il = _interlineSpacing * SpaceAtom::getFactor(_interlineUnit, *te);
			auto b = FormulaBreaker::split(box, te->getTextWidth(), il);
			hb = new HorizontalBox(b, _isMaxWidth ? b->_width : te->getTextWidth(), _align);
		} else {
			hb = new HorizontalBox(box, _isMaxWidth ? box->_width : te->getTextWidth(), _align);
		}
		ti = new TeXRender(shared_ptr<Box>(hb), _size, _trueValues);
	} else {
		ti = new TeXRender(box, _size, _trueValues);
	}
	if (!istrans(_fg))
		ti->setForeground(_fg);
	ti->_iscolored = te->_isColored;

	delete te;
	return ti;
}
