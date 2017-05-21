#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED

#include "graphic/graphic_basic.h"

using namespace std;
using namespace tex;

namespace tex {

class Graphics2D;

enum TypefaceStyle { PLAIN = 0, BOLD = 1, ITALIC = 2, BOLDITALIC = BOLD | ITALIC };

/**
 * Class represent font information
 */
class Font {

public:

	/**
	 * Get the font size
	 */
	virtual float getSize() const = 0;

	/**
	 * Derive font from current font with specified style
	 *
	 * @param style
	 * 		Required style, specified in enum TypefaceStyle,
	 * 		we use int type to represents style
	 */
	virtual shared_ptr<Font> deriveFont(int style) const = 0;

	/**
	 * Check if current font equals other
	 */
	virtual bool operator==(const Font& f) const = 0;

	/**
	 * Check if current font not equals other
	 */
	virtual bool operator!=(const Font& f) const = 0;

	/**
	 * Create font from file
	 * 
	 * @param file
	 * 		Specified path of font file
	 * @param size
	 * 		The required font size
	 */
	static Font* create(const string& file, float size);

	/**
	 * Create font with specified name, style and size
	 *
	 * @param name
	 * 		The font name
	 * @param style
	 * 		The font style
	 * @param size
	 * 		The font size
	 */
	static shared_ptr<Font> _create(const string& name, int style, float size);
};

/**
 * To layout text with no new line
 */
class TextLayout {

public:

	/**
	 * Get the layout bounds
	 *
	 * @param bounds
	 * 		Rectangle to retrieve bounds
	 */
	virtual void getBounds(_out_ Rect& bounds) = 0;

	/**
	 * Draw the layout
	 *
	 * @param g2
	 * 		The graphic context
	 * @param x
	 * 		The x coordinate
	 * @param y
	 * 		The y coordinate
	 */
	virtual void draw(Graphics2D& g2, float x, float y) = 0;

	/**
	 * Create a TextLayout with specified text and font
	 *
	 * @param src
	 * 		The specified text
	 * @param font
	 * 		The specified font
	 * @return
	 * 		New TextLayout
	 */
	static shared_ptr<TextLayout> create(const wstring& src, const shared_ptr<Font>& font);
};

/**
 * Represent a 2D graphics context, all the TeX drawing operation would be
 * on it. Current supported affine transformation is scale, translation and rotation,
 * but note that the scaling on y direction will be selected as base if the scaling
 * on x and y direction are different when drawing a character, so you should
 * never use a different scaling unless you are really sure that the coordinate
 * is positioned correctly (i.e. draw a hyphen).
 */
class Graphics2D {
public:

	/**
	 * Set the color of graphic context
	 * 
	 * @param c
	 * 		Required color
	 */
	virtual void setColor(color c) = 0;

	/**
	 * Get the color of current graphic context
	 */
	virtual color getColor() const = 0;

	/**
	 * Set the stroke of graphic context
	 * 
	 * @param s
	 * 		Required stroke
	 */
	virtual void setStroke(const Stroke& s) = 0;

	/**
	 * Get the stroke of current graphic context
	 */
	virtual const Stroke& getStroke() const = 0;

	/**
	 * Set stroke width of current graphic context
	 * 
	 * @param w
	 * 		Required stroke width
	 */
	virtual void setStrokeWidth(float w) = 0;

	/**
	 * Get current font
	 */
	virtual const Font* getFont() const = 0;

	/**
	 * Set font of graphic context
	 */
	virtual void setFont(const Font* font) = 0;

	/**
	 * Translate the graphic context with distance dx, dy
	 *
	 * @param dx
	 *      Distance in x-direction to translate
	 * @param dy
	 * 		Distance in y-direction to translate
	 */
	virtual void translate(float dx, float dy) = 0;

	/**
	 * Scale the graphic context with sx, sy
	 *
	 * @param sx
	 *      scale ratio in x-direction
	 * @param sy
	 * 		scale ratio in y-direction
	 */
	virtual void scale(float sx, float sy) = 0;

	/**
	 * Rotate the graphic context with specified angle (in radian), the pivot of rotation
	 * are settled to (0, 0)
	 *
	 * @param angle
	 * 		Angle (in radian) amount to rotate
	 */
	virtual void rotate(float angle) = 0;

	/**
	 * Rotate the graphics context with a angle (in radian), the pivot of rotation
	 * are settled to (px, py)
	 *
	 * @param angle
	 *      Angle (in radian) amount to rotate
	 * @param px
	 *      Pivot in x-direction
	 * @param py
	 *      Pivot in y-direction
	 */
	virtual void rotate(float angle, float px, float py) = 0;

	/**
	 * Reset the transformation of current graphics context
	 */
	virtual void reset() = 0;

	/**
	 * @return the scaling in x-direction
	 */
	virtual float sx() const = 0;

	/**
	 * @return the scaling in y-direction
	 */
	virtual float sy() const = 0;

	/**
	 * @return the translation in x-direction
	 */
	virtual float tx() const = 0;

	/**
	 * @return the translation in y-direction
	 */
	virtual float ty() const = 0;

	/**
	 * @return the degree (in radian) amount rotated of current graphics context
	 */
	virtual float r() const = 0;

	/**
	 * @return the pivot of rotation in x-direction
	 */
	virtual float px() const = 0;

	/**
	 * @return the pivot of rotation in y-direction
	 */
	virtual float py() const = 0;

	/**
	 * Draw character
	 * 
	 * @param c
	 * 		Specified character
	 * @param x
	 * 		x coordinate
	 * @param y
	 * 		y coordinate
	 */
	virtual void drawChar(wchar_t c, float x, float y) = 0;

	/**
	 * Draw text
	 * 
	 * @param c
	 * 		Specified text
	 * @param x
	 * 		x coordinate
	 * @param y
	 * 		y coordinate
	 */
	virtual void drawText(const wstring& c, float x, float y) = 0;

	/**
	 * Draw line
	 * 
	 * @param x1
	 * 		Start point in x direction
	 * @param y1
	 * 		Start point in y direction
	 * @param x2
	 * 		End point in x direction
	 * @param y2
	 * 		End point in y direction
	 */
	virtual void drawLine(float x1, float y1, float x2, float y2) = 0;

	/**
	 * Draw rectangle
	 *
	 * @param x
	 * 		Left position
	 * @param y
	 *		Top position
	 * @param w
	 * 		Width
	 * @param h
	 * 		Height
	 */
	virtual void drawRect(float x, float y, float w, float h) = 0;

	/**
	 * Fill rectangle
	 *
	 * @param x
	 * 		Left position
	 * @param y
	 * 		Top position
	 * @param w
	 * 		Width
	 * @param h
	 * 		Height
	 */
	virtual void fillRect(float x, float y, float w, float h) = 0;

	/**
	 * Draw round rectangle
	 *
	 * @param x
	 * 		Left position
	 * @param y
	 * 		Top position
	 * @param w
	 * 		Width
	 * @param h
	 * 		Height
	 * @param rx
	 * 		Radius in x direction
	 * @param ry
	 * 		Radius in y direction
	 */
	virtual void drawRoundRect(float x, float y, float w, float h, float rx, float ry) = 0;

	/**
	 * Fill round rectangle
	 *
	 * @param x
	 * 		Left position
	 * @param y
	 * 		Top position
	 * @param w
	 * 		Width
	 * @param h
	 * 		Height
	 * @param rx
	 * 		Radius in x direction
	 * @param ry
	 * 		Radius in y direction
	 */
	virtual void fillRoundRect(float x, float y, float w, float h, float rx, float ry) = 0;
};

}

#endif // GRAPHIC_H_INCLUDED
