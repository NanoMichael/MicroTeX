#ifdef _WIN32

#include "graphic/graphic.h"
#include "common.h"

#include <sstream>

#include <windows.h>
#include <gdiplush.h>

using namespace std;
using namespace tex;


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

namespace tex {

class Font_win32 {
private:
	static const Gdiplus::FontFamily* _serif;
	static const Gdiplus::FontFamily* _sansserif;

	float _size;

public:
	int _style;
	shared_ptr<Gdiplus::Font> _typeface;
	const Gdiplus::FontFamily* _family;

	virtual float getSize() const override;

	virtual shared_ptr<Font> deriveFont(int style) const override;

	virtual bool operator==(const Font& f) const override;

	virtual bool operator!=(const Font& f) const override;

	virtual ~Font_win32();

	static int convertStyle(int style);
}

class TextLayout_win32 {
private:
	Font_win32 _font;
	wstring _txt;

public:
	static const Gdiplus::StringFormat* _format;
	static Gdiplus::Graphics* _g;
	static Gdiplus::Bitmap* _img;

	virtual void getBounds(_out_ Rect& bounds) override;

	virtual void draw(Graphics2D& g2, float x, float y) override;

	virtual ~TextLayout_win32();
}

class Graphics2D_win32 {

public:
	virtual void setColor(color c) override;

	virtual color getColor() const override;

	virtual void setStroke(const Stroke& s) override;

	virtual const Stroke& getStroke() const override;

	virtual void setStrokeWidth(float w) override;

	virtual const Font* getFont() const override;

	virtual void setFont(const Font* font) override;

	virtual void translate(float dx, float dy) override;

	virtual void scale(float sx, float sy) override;

	virtual void rotate(float angle) override;

	virtual void rotate(float angle, float px, float py) override;

	virtual void reset() override;

	virtual float sx() const override;

	virtual float sy() const override;

	virtual float tx() const override;

	virtual float ty() const override;

	virtual float r() const override;

	virtual float px() const override;

	virtual float py() const override;

	virtual void drawChar(wchar_t c, float x, float y) override;

	virtual void drawText(const wstring& c, float x, float y) override;

	virtual void drawLine(float x1, float y1, float x2, float y2) override;

	virtual void drawRect(float x, float y, float w, float h) override;

	virtual void fillRect(float x, float y, float w, float h) override;

	virtual void drawRoundRect(float x, float y, float w, float h) override;

	virtual void fillRoundRect(float x, float y, float w, float h) override;
}

}

#endif