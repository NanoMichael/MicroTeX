#ifndef PORT_H_INCLUDED
#define PORT_H_INCLUDED

#if defined (__clang__)
#include "port_basic.h"
#elif defined (__GNUC__)
#include "port/port_basic.h"
#endif // defined

using namespace std;
using namespace tex;

#ifdef _WIN32
// forward declaration
namespace Gdiplus {

class Font;
class FontFamily;
class Graphics;
class Pen;
class Brush;
class SolidBrush;
class StringFormat;
class Bitmap;

}
#endif // _WIN32

#ifdef __ANDROID__
#include <jni.h>
#endif
#ifdef __APPLE_CC__
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <CoreGraphics/CGContext.h>
#import <CoreText/CoreText.h>
#endif //MAC OS

/*******************************************************************************************
 *                      classes related to specific platforms                              *
 *******************************************************************************************/

namespace tex {
namespace port {

enum TypefaceStyle { PLAIN = 0, BOLD = 1, ITALIC = 2, BOLDITALIC = BOLD | ITALIC };

// class represent font information
class Font {
private:
	float _size;
#ifdef _WIN32
	static const Gdiplus::FontFamily* _serif;
	static const Gdiplus::FontFamily* _sansserif;
#endif // _WIN32

public:
#ifdef _WIN32
	int _style;
	shared_ptr<Gdiplus::Font> _typeface;
	const Gdiplus::FontFamily* _family;
#endif // _WIN32

#ifdef __APPLE_CC__
	int _style;
	UIFont *_iFont;
#endif //MAC OS

#ifdef __ANDROID__
	shared_ptr<_jobject> _java_font;
#endif // __ANDROID__

	Font();

	Font(const string &name, int style, float size);

#ifdef __APPLE_CC__
	UIFont* getFont() const;

	Font(UIFont *iFont, float size);

	static UIFont* creatIFont(int style, float size);
#endif //MAC OS

	float getSize() const;

	Font deriveFont(int style) const;

	bool operator==(const Font& f) const;

	bool operator!=(const Font& f) const;

	static Font* create(const string& file, float s);

	~Font();

#ifdef _WIN32
	static int convertStyle(int style);
#endif // _WIN32
};

enum AffineTransformIndex {
	SX, SY, TX, TY, R, PX, PY
};

/**
 * Represent a 2D graphics environment, all the TeX drawing operation would be
 * on it. Current supported affine transformation is scale, translation and rotation,
 * but note that the scaling on y direction will be selected as base if the scaling
 * on x and y direction are different when drawing a character, so you should
 * never use a different scaling unless you are really sure that the coordinate
 * is positioned correctly (i.e. draw a hyphen).
 */
class Graphics2D {
private:
	color _color;

	Stroke _stroke;

	const Font* _font;

	float* _t;

#ifdef _WIN32
	static const Font* _defaultFont;
	static const Gdiplus::StringFormat* _format;
	Gdiplus::Graphics* _g;
	Gdiplus::Pen* _pen;
	Gdiplus::SolidBrush* _brush;
#endif // _WIN32

#ifdef __APPLE_CC__
	CGContextRef _ref;
#endif // MAC OS
public:

#ifdef _WIN32
	Graphics2D(Gdiplus::Graphics* g);
#endif // _WIN32

#ifdef __APPLE_CC__
	CGFloat _cR;
	CGFloat _cG;
	CGFloat _cB;

	Graphics2D(CGContextRef ref);
#endif // MAC OS

#ifdef __ANDROID__
	jobject _java_g;

	Graphics2D(jobject java_recorder);
#endif

	~Graphics2D();

	//--------getter setter
	void setColor(color c);

	color getColor() const;

	void setStroke(const Stroke& s);

	const Stroke& getStroke() const;

	void setStrokeWidth(float w);

	const Font* getFont() const;

	void setFont(const Font* font);

	//------------transformation---------
	/**
	 * translate the graphics context with distance dx, dy
	 * @param dx
	 *      distance in x-direction to translate
	 * @param dy
	 * 		distance in y-direction to translate
	 */
	void translate(float dx, float dy);

	/**
	 * scaling the graphics context with sx, sy
	 * @param sx
	 *      scale ratio in x-direction
	 * @param sy
	 * 		scale ratio in y-direction
	 */
	void scale(float sx, float sy);

	/**
	 * rotate the graphics context with a angle (in radian), the pivot of rotation
	 * are settled to (0, 0)
	 * @param angle
	 * 		angle (in radian) amount to rotate
	 */
	void rotate(float angle);

	/**
	 * rotate the graphics context with a angle (in radian), the pivot of rotation
	 * are settled to (px, py)
	 * @param angle
	 *      angle (in radian) amount to rotate
	 * @param px
	 *      pivot in x-direction
	 * @param py
	 *      pivot in y-direction
	 */
	void rotate(float angle, float px, float py);

	/**
	 * reset the transformation of "this" graphics context
	 */
	void reset();

	/**
	 * @return the scaling in x-direction
	 */
	float sx() const;

	/**
	 * @return the scaling in y-direction
	 */
	float sy() const;

	/**
	 * @return the translation in x-direction
	 */
	float tx() const;

	/**
	 * @return the translation in y-direction
	 */
	float ty() const;

	/**
	 * @return the degree (in angle) amount rotated of "this" graphics context
	 */
	float r() const;

	/**
	 * @return the pivot of rotation in x-direction
	 */
	float px() const;

	/**
	 * @return the pivot of rotation in y-direction
	 */
	float py() const;

	//-------------draw & fill----------
	void drawChar(wchar_t c, float x, float y);

	void drawText(const wstring& c, float x, float y);

	void drawLine(float x1, float y1, float x2, float y2);

	void drawRect(float x, float y, float w, float h);

	void fillRect(float x, float y, float w, float h);

	void drawRoundRect(float x, float y, float w, float h, float rx, float ry);

	void fillRoundRect(float x, float y, float w, float h, float rx, float ry);
};

/**
 * To layout text with no breaking line
 */
class TextLayout {
private:
	Font _font;
	wstring _txt;
public:
#ifdef _WIN32
	static const Gdiplus::StringFormat* _format;
	static Gdiplus::Graphics* _g;
	static Gdiplus::Bitmap* _img;
#endif // _WIN32

	TextLayout();

	TextLayout(const wstring& src, const Font& font);

	Rect getBounds();

	void draw(Graphics2D& g2, float x, float y);

	~TextLayout();
};

}
}

#endif // PORT_H_INCLUDED
