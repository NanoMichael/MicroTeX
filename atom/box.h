#ifndef BOX_BASIC_H_INCLUDED
#define BOX_BASIC_H_INCLUDED

#if defined (__clang__)
#include "atom.h"
#elif defined (__GNUC__)
#include "atom/atom.h"
#endif // defined

#include <stack>

// forward declare
namespace tex {
namespace fonts {

class Char;
class CharFont;
class TeXFont;

}
}

using namespace tex::core;
using namespace tex::port;
using namespace tex::fonts;

namespace tex {
namespace core {

class SymbolAtom;

/*******************************************************************************
 *                      factories to create boxes                              *
 *******************************************************************************/

/**
 * Responsible for creating a box containing a delimiter symbol that exists in
 * different sizes.
 */
class DelimiterFactory {
public:
	static shared_ptr<Box> create(_in_ SymbolAtom& symbol, _out_ TeXEnvironment& env, int size);

	/**
	 *
	 * @param symbol
	 * 		the name of the delimiter symbol
	 * @param env
	 *      the TeXEnvironment in which to create the delimiter box
	 * @param minHeight
	 *      the minimum required total height of the box (height + depth).
	 * @return the box representing the delimiter variant that fits best
	 *      according to the required minimum size.
	 */
	static shared_ptr<Box> create(const string& symbol, _out_ TeXEnvironment& env, float minHeight);
};

/**
 * Responsible for creating a box containing a delimiter symbol that exists in
 * different sizes.
 */
class XLeftRightArrowFactory {
private:
	static shared_ptr<Atom> MINUS;
	static shared_ptr<Atom> LEFT;
	static shared_ptr<Atom> RIGHT;
public:
	static shared_ptr<Box> create(bool left, _out_ TeXEnvironment& env, float width);

	static shared_ptr<Box> create(_out_ TeXEnvironment& env, float width);
};

/**********************************************************************************
 *                                rule boxes                                      *
 **********************************************************************************/

/**
 * A box composed of a horizontal row of child boxes
 */
class HorizontalBox : public Box {

private:
	void recalculate(const Box& b);

	pair<shared_ptr<HorizontalBox>, shared_ptr<HorizontalBox>> split(int pos, int shift);
public:
	vector<int> _breakPositions;

	HorizontalBox() {}

	HorizontalBox(color fg, color bg) : Box(fg, bg) {}

	HorizontalBox(const shared_ptr<Box>& b, float w, int alignment);

	HorizontalBox(const shared_ptr<Box>& b);

	shared_ptr<HorizontalBox> cloneBox();

	void add(const shared_ptr<Box>& b) override;

	void add(int pos, const shared_ptr<Box>& b) override;

	inline void addBreakPosition(int pos) {
		_breakPositions.push_back(pos);
	}

	pair<shared_ptr<HorizontalBox>, shared_ptr<HorizontalBox>> split(int pos) {
		return split(pos, 1);
	}

	pair<shared_ptr<HorizontalBox>, shared_ptr<HorizontalBox>> splitRemove(int pos) {
		return split(pos, 2);
	}

	void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;
};

/**
 * A box composed of other boxes, put one above the other
 */
class VerticalBox : public Box {

private:
	float _leftMostPos, _rightMostPos;

	void recalculateWidth(const Box& b);
public:
	VerticalBox() :
		_leftMostPos(F_MAX), _rightMostPos(F_MIN) {
	}

	VerticalBox(const shared_ptr<Box>& b, float rest, int alignment);

	void add(const shared_ptr<Box>& b) override;

	void add(const shared_ptr<Box>& b, float interline);

	void add(int pos, const shared_ptr<Box>& b) override;

	inline int getSize() const {
		return _children.size();
	}

	void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;
};

/**
 * A box representing another box with a horizontal rule above it, with
 * appropriate kerning.
 */
class OverBar : public VerticalBox {

public:
	OverBar() = delete;

	OverBar(const shared_ptr<Box>& b, float kern, float thickness);
};

/**
 * A box representing another box with a delimiter box and a script box above or
 * under it, with script and delimiter separated by a kerning.
 */
class OverUnderBox : public Box {

private:
	// base, delimiter and script
	shared_ptr<Box> _base, _del, _script;
	// kerning amount between the delimiter and the script
	float _kern;
	// whether the delimiter should be drawn over (<->under) the base box
	bool _over;
public:
	OverUnderBox() = delete;

	/**
	 * the parameter boxes must have an equal width!!
	 *
	 * @param b
	 * 		base box to be drawn on the baseline
	 * @param d
	 *      delimiter box
	 * @param script
	 *      subscript or superscript box
	 * @param kern
	 *      the kerning amount to draw
	 * @param over
	 *      true : draws delimiter and script box above the base box,
	 *      false : under the base box
	 */
	OverUnderBox(const shared_ptr<Box>& b, const shared_ptr<Box>& d, const shared_ptr<Box>& script, float kern, bool over);

	void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;
};

/**
 * A box representing a horizontal line.
 */
class HorizontalRule : public Box {

private:
	color _color;
	float _speShift;
public:
	HorizontalRule() = delete;

	HorizontalRule(float thickness, float width, float s);

	HorizontalRule(float thickness, float width, float s, bool trueShift);

	HorizontalRule(float thickness, float withd, float s, color c, bool trueshift);

	void draw(Graphics2D& g2, float s, float y) override;

	int getLastFontId() override;
};

/**********************************************************************************
 *                            operation boxes                                     *
 **********************************************************************************/

/**
 * A box representing a scale operation
 */
class ScaleBox : public Box {

private:
	shared_ptr<Box> _box;
	float _sx, _sy;
	float _factor;

	void init(const shared_ptr<Box>& b, float sx, float sy);
public:
	ScaleBox() = delete;

	ScaleBox(const shared_ptr<Box>& b, float sx, float sy) {
		init(b, sx, sy);
	}

	ScaleBox(const shared_ptr<Box>& b, float factor) {
		init(b, factor, factor);
		_factor = factor;
	}

	void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;
};

/**
 * A box representing a reflected box
 */
class ReflectBox : public Box {

private:
	shared_ptr<Box> _box;
public:
	ReflectBox() = delete;

	ReflectBox(const shared_ptr<Box>& b);

	void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;
};

/**
 * enumeration representing rotation origin
 */
enum Rotation {
	BL, BC, BR, TL, TC, TR, BBL, BBR, BBC, CL, CC, CR
};

/**
 * A box representing a rotate operation
 */
class RotateBox : public Box {

private:
	shared_ptr<Box> _box;
	float _angle;
	float _xmax, _xmin, _ymax, _ymin;
	int _option;
	float _shiftX, _shiftY;

	void init(const shared_ptr<Box>& b, float angle, float x, float y);

	static port::Point calculateShift(const Box& b, int option);
public:
	RotateBox() = delete;

	RotateBox(const shared_ptr<Box>& b, float angle, float x, float y)  {
		init(b, angle, x, y);
	}

	RotateBox(const shared_ptr<Box>& b, float angle, const port::Point& origin) {
		init(b, angle, origin.x, origin.y);
	}

	RotateBox(const shared_ptr<Box>& b, float angle, int option) {
		const port::Point& p = calculateShift(*b, option);
		init(b, angle, p.x, p.y);
	}

	void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;

	static int getOrigin(string option);
};

/*********************************************************************************
 *                              wrapped boxes
 *********************************************************************************/

/**
 * A box representing a wrapped box by square frame
 */
class FramedBox : public Box {
public:
	shared_ptr<Box> _box;
	float _thickness;
	float _space;
	color _line;
	color _bg;

	void init(const shared_ptr<Box>& box, float thickness, float space);
public:
	FramedBox() = delete;

	FramedBox(const shared_ptr<Box>& box, float thickness, float space) {
		init(box, thickness, space);
	}

	FramedBox(const shared_ptr<Box>& box, float thickness, float space, color line, color bg) {
		init(box, thickness, space);
		_line = line;
		_bg = bg;
	}

	virtual void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;
};

/**
 * A box representing a wrapped box by oval frame
 */
class OvalBox : public FramedBox {
public:
	OvalBox() = delete;

	OvalBox(const shared_ptr<FramedBox>& fbox) :
		FramedBox(fbox->_box, fbox->_thickness, fbox->_space) {
	}

	void draw(Graphics2D& g2, float x, float y) override;
};

/**
 * A box representing a wrapped box by shadowed frame
 */
class ShadowBox : public FramedBox {
private:
	float _shadowRule;
public:
	ShadowBox() = delete;

	ShadowBox(const shared_ptr<FramedBox>& fbox, float shadowRule) :
		FramedBox(fbox->_box, fbox->_thickness, fbox->_space) {
		_shadowRule = shadowRule;
		_depth += shadowRule;
		_width += shadowRule;
	}

	void draw(Graphics2D& g2, float x, float y) override;
};

/*********************************************************************************
 *                               basic boxes                                     *
 *********************************************************************************/

/**
 * a box representing whitespace
 */
class StrutBox : public Box {
public:
	StrutBox() = delete;

	StrutBox(float w, float h, float d, float s) {
		_width = w;
		_height = h;
		_depth = d;
		_shift = s;
	}

	void draw(Graphics2D& g2, float x, float y) override {
		// no visual effect
	}

	int getLastFontId() override;
};

/**
 * A box representing glue
 */
class GlueBox : public Box {

public:
	float _stretch, _shrink;

	GlueBox() = delete;

	GlueBox(float space, float stretch, float shrink) {
		_width = space;
		_stretch = stretch;
		_shrink = shrink;
	}

	void draw(Graphics2D& g2, float x, float y) override {
		// no visual effect
	}

	int getLastFontId() override;
};

/**
 * A box representing a single character
 */
class CharBox : public Box {

private:
	shared_ptr<CharFont> _cf;
	float _size;
public:

	CharBox() = delete;

	/**
	 * Create a new CharBox that will represent the character defined by the
	 * given Char-object.
	 *
	 * @param c
	 * 		a Char-object containing the character's font information.
	 */
	CharBox(const Char& c);

	void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;
};

/**
 * A box representing a text rendering box
 */
class TextRenderingBox : public Box {
private:
	static Font* _font;
	// text layout
	TextLayout _layout;
	float _size;

	void init(const wstring& str, int type, float size, const Font& f, bool kerning);
public:
	TextRenderingBox() = delete;

	TextRenderingBox(const wstring& str, int type, float size, const Font& f, bool kerning) {
		init(str, type, size, f, kerning);
	}

	TextRenderingBox(const wstring& str, int type, float size) {
		init(str, type, size, *_font, true);
	}

	void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;

	static void setFont(const string& name);

	static void _init_();

	static void _free_();
};

/**
 * A box act as wrapper that with insets in left, top, right and bottom
 */
class WrapperBox : public Box {
private:
	shared_ptr<Box> _base;
	float _l;
public:
	WrapperBox() = delete;

	WrapperBox(const shared_ptr<Box>& base) :
		_base(base), _l(0) {
		_height = _base->_height;
		_depth = _base->_depth;
		_width = _base->_width;
	}

	WrapperBox(const shared_ptr<Box>& base, float width, float rowheight, float rowdepth, float align) :
		_base(base), _l(0) {
		_height = rowheight;
		_depth = rowdepth;
		_width = width;
		if (align == ALIGN_RIGHT)
			_l = width - _base->_width;
		else if (align == ALIGN_CENTER)
			_l = (width - _base->_width) / 2.f;
	}

	void setInsets(float l, float t, float r, float b);

	void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;
};

/**
 * class representing a box that shifted up or down (when shift is negative)
 */
class ShiftBox : public Box {
private:
	float _sf;
	shared_ptr<Box> _base;
public:
	ShiftBox() = delete;

	ShiftBox(const shared_ptr<Box>& base, float shift) :
		_base(base), _sf(shift) {
	}

	void draw(Graphics2D& g2, float x, float y) override;

	int getLastFontId() override;
};

}
}

#endif // BOX_BASIC_H_INCLUDED
