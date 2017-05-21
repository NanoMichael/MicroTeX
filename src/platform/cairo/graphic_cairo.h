#ifdef __linux__

#ifndef GRAPHIC_CAIRO_H_INCLUDED
#define GRAPHIC_CAIRO_H_INCLUDED

#include <cairomm/context.h>
#include <pangomm/fontdescription.h>

#include "graphic/graphic.h"

using namespace std;

namespace tex {

class Font_cairo : public Font {
private:
	Pango::Style _slant;
	Pango::Weight _weight;
	double _size;
	string _family;

	void convertStyle(int style);

	void loadFont(const string& file);

	Font_cairo();

public:
	Font_cairo(const string& name, int style, float size);

	Font_cairo(const string& file, float size);

	virtual float getSize() const override;

	virtual shared_ptr<Font> deriveFont(int style) const override;

	virtual bool operator==(const Font& f) const override;

	virtual bool operator!=(const Font& f) const override;
};

class TextLayout_cairo : public TextLayout {
public:
	virtual void getBounds(_out_ Rect& r) override;

	virtual void draw(Graphics2D& g2, float x, float y) override;
};

enum AffineTransformIndex {SX, SY, TX, TY, R, PX, PY};

class Graphics2D_cairo : public Graphics2D {

private:
	Cairo::RefPtr<Cairo::Context> _context;
	color _color;
	Stroke _stroke;
	const Font* _font;
	float _t[7];

	void roundRect(float x, float y, float w, float h, float rx, float ry);
public:
	Graphics2D_cairo(const Cairo::RefPtr<Cairo::Context>& context);

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

	virtual void drawLine(float x, float y1, float x2, float y2) override;

	virtual void drawRect(float x, float y, float w, float h) override;

	virtual void fillRect(float x, float y, float w, float h) override;

	virtual void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

	virtual void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
};

}

#endif // GRAPHIC_GTKMM_H_INCLUDED
#endif // linux