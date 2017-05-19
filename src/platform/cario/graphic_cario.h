#ifdef linux

#ifndef GRAPHIC_GTKMM_H_INCLUDED
#define GRAPHIC_GTKMM_H_INCLUDED

#include <cariomm/context.h>

namespace tex {

class Font_cario : public Font {
public:
	virtual float getSize() const override;

	virtual shared_ptr<Font> deriveFont(int style) const override;

	virtual bool operator==(const Font& f) const override;

	virtual bool operator!=(const Font& f) const override;
}

class TextLayout_cario : public TextLayout {
public:
	virtual void getBounds(_out_ Rect& r) override;

	virtual void draw(Graphics2D& g2, float x, float y) override;
}

class Graphics2D_cario : public Graphics2D {
private:
	Cairo::RefPtr<Cairo::Context> _context;
public:
	Graphics2D_cario(Cairo::RefPtr<Cairo::Context> context);

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

	virtual void drawChar(wchat_t c, float x, float y) override;

	virtual void drawText(const wstring& c, float x, float y) override;

	virtual void drawLine(float x, float y1, float x2, float y2) override;

	virtual void drawRect(float x, float y, float w, float h) override;

	virtual void fillRect(float x, float y, float w, float h) override;

	virtual void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override;

	virtual void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override;
}

}

#endif // GRAPHIC_GTKMM_H_INCLUDED

#endif // linux