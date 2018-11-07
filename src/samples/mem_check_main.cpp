#include "config.h"

#ifdef __MEM_CHECK

#include "graphic/graphic.h"
#include "latex.h"

namespace tex {

class Font_none : public Font {
public:
    Font_none() {}

    float getSize() const override {
        return 1.f;
    }

    shared_ptr<Font> deriveFont(int style) const override {
        return shared_ptr<Font>(new Font_none());
    }

    bool operator ==(const Font& f) const override {
        return false;
    }

    bool operator !=(const Font& f) const override {
        return !(*this == f);
    }
};

Font* Font::create(const string& file, float size) {
    return new Font_none();
}

shared_ptr<Font> Font::_create(const string& name, int style, float size) {
    return shared_ptr<Font>(new Font_none());
}

/**************************************************************************/

class TextLayout_none : public TextLayout {
public:
    TextLayout_none() {}

    void getBounds(_out_ Rect& bounds) override {
        bounds.x = bounds.y = bounds.w = bounds.h = 0.f;
    }

    void draw(Graphics2D& g2, float x, float y) override {
    }
};

shared_ptr<TextLayout> TextLayout::create(const wstring& src, const shared_ptr<Font>& font) {
    return shared_ptr<TextLayout>(new TextLayout_none());
}

/**************************************************************************/

class Graphics2D_none : public Graphics2D {
private:
    static Font* _default_font;
    const Font* _font;
    Stroke _stroke;
public:
    Graphics2D_none() : _font(_default_font), _stroke() {}

    static void release() {
        delete _default_font;
    }

    void setColor(color c) override {
    }

    color getColor() const override {
        return 0;
    }

    void setStroke(const Stroke& s) override {
        _stroke = s;
    }

    const Stroke& getStroke() const override {
        return _stroke;
    }

    void setStrokeWidth(float w) override {
    }

    const Font* getFont() const override {
        return _font;
    }

    void setFont(const Font* font) override {
        _font = font;
    }

    void translate(float dx, float dy) override {
    }

    void scale(float sx, float sy) override {
    }

    void rotate(float angle) override {
    }

    void rotate(float angle, float px, float py) override {
    }

    void reset() override {
    }

    float sx() const override {
        return 1.f;
    }

    float sy() const override {
        return 1.f;
    }

    float tx() const override {
        return 0.f;
    }

    float ty() const override {
        return 0.f;
    }

    float r() const override {
        return 0.f;
    }

    float px() const override {
        return 0.f;
    }

    float py() const override {
        return 0.f;
    }

    void drawChar(wchar_t c, float x, float y) override {
    }

    void drawText(const wstring& c, float x, float y) override {
    }

    void drawLine(float x1, float y1, float x2, float y2) override {
    }

    void drawRect(float x, float y, float w, float h) override {
    }

    void fillRect(float x, float y, float w, float h) override {
    }

    void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override {
    }

    void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override {
    }
};

Font* Graphics2D_none::_default_font = new Font_none();

}

int main(int argc, char* argv[]) {
    LaTeX::init();

    TeXRender* r = LaTeX::parse(L"\\text{What a beautiful day}", 720 , 20, 20 / 3.f, 0xff000000);
    Graphics2D_none g2;
    r->draw(g2, 0, 0);
    delete r;

    LaTeX::release();
    Graphics2D_none::release();

    return 0;
}

#endif