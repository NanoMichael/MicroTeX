#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#if defined (__clang__)
#include "port.h"
#elif defined (__GNUC__)
#include "port/port.h"
#endif // defined

using namespace std;
using namespace tex;
using namespace tex::port;

namespace tex {

namespace fonts {
class DefaultTeXFont;
}

namespace core {
class TeXFormula;
class Box;
class Atom;
}

using namespace tex::core;
using namespace tex::fonts;

class TeXRender {
private:
	static const color _defaultcolor;

	shared_ptr<Box> _box;
	float _size;
	color _fg;
	Insets _insets;
public:
	static float _defaultSize;
	static float _magFactor;
	bool _iscolored;

	TeXRender(const shared_ptr<Box> b, float s, bool trueValues = false);

	float getSize() const;

	int getHeight() const;

	int getDepth() const;

	int getWidth() const;

	float getBaseline() const;

	void setSize(float s);

	void setForeground(color fg);

	Insets getInsets();

	void setInsets(const Insets& insets, bool trueval = false);

	void setWidth(int w, int align);

	void setHeight(int h, int align);

	void draw(_out_ Graphics2D& g2, int x, int y);
};

enum TeXFontStyle {
	SERIF = 0,
	SANSSERIF = 1,
	BOLD = 2,
	ITALIC = 4,
	ROMAN = 8,
	TYPEWRITER = 16
};

class TeXRenderBuilder {
private:
	int _style, _type, _widthUnit, _align, _interlineUnit;
	float _size, _textWidth, _interlineSpacing;
	bool _trueValues, _isMaxWidth;
	color _fg;
public:
	TeXRenderBuilder() :
		_style(-1), _type(-1), _widthUnit(-1), _align(-1), _interlineUnit(-1),
		_size(0), _textWidth(0), _interlineSpacing(0),
		_trueValues(false), _isMaxWidth(false), _fg(black) {
	}

	inline TeXRenderBuilder& setStyle(int style) {
		_style = style;
		return *this;
	}

	inline TeXRenderBuilder& setSize(float s) {
		_size = s;
		return *this;
	}

	inline TeXRenderBuilder& setType(int type) {
		_type = type;
		return *this;
	}

	inline TeXRenderBuilder& setForeground(color c) {
		_fg = c;
		return *this;
	}

	inline TeXRenderBuilder& setTrueValues(bool t) {
		_trueValues = t;
		return *this;
	}

	inline TeXRenderBuilder& setWidth(int wu, float w, int align) {
		_widthUnit = wu;
		_textWidth = w;
		_align = align;
		_trueValues = true;
		return *this;
	}

	inline TeXRenderBuilder& setIsMaxWidth(bool i) {
		if (_widthUnit == -1)
			throw ex_invalid_state("cannot set 'isMaxWidth' without having specified a width!");
		if (i) {
			// Currently isMaxWidth==true does not work with
			// ALIGN_CENTER or ALIGN_RIGHT (see HorizontalBox ctor)
			// The case (1) we don't support by setting align := ALIGN_LEFT
			// here is this:
			// \text{hello world\\hello} with align=ALIGN_CENTER (but forced
			// to ALIGN_LEFT) and isMaxWidth==true results in:
			// [hello world]
			// [hello ]
			// and NOT:
			// [hello world]
			// [ hello ]
			// However, this case (2) is currently not supported anyway
			// (ALIGN_CENTER with isMaxWidth==false):
			// [ hello world ]
			// [ hello ]
			// and NOT:
			// [ hello world ]
			// [ hello ]
			// => until (2) is solved, we stick with the hack to set align
			// := ALIGN_LEFT!
			_align = ALIGN_LEFT;
		}
		_isMaxWidth = i;
		return *this;
	}

	inline TeXRenderBuilder& setInterlineSpacing(int iu, float spacing) {
		if (_widthUnit == -1)
			throw ex_invalid_state("cannot set inter-line spacing without having specified a width!");
		_interlineSpacing = spacing;
		_interlineUnit = iu;
		return *this;
	}

	TeXRender* build(const shared_ptr<Atom>& f);

	TeXRender* build(TeXFormula& f);

	static DefaultTeXFont* createFont(float s, int type);
};

}

#endif // RENDER_H_INCLUDED
