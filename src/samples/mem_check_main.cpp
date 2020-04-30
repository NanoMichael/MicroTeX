#include "config.h"

#ifdef MEM_CHECK

#include "graphic/graphic.h"
#include "latex.h"

namespace tex {

class Font_none : public Font {
public:
  Font_none() {}

  float getSize() const override {
    return 1.f;
  }

  sptr<Font> deriveFont(int style) const override {
    return sptr<Font>(new Font_none());
  }

  bool operator==(const Font& f) const override {
    return false;
  }

  bool operator!=(const Font& f) const override {
    return !(*this == f);
  }

  virtual ~Font_none() {}
};

Font* Font::create(const string& file, float size) {
  return new Font_none();
}

sptr<Font> Font::_create(const string& name, int style, float size) {
  return sptr<Font>(new Font_none());
}

/**************************************************************************************************/

class TextLayout_none : public TextLayout {
public:
  TextLayout_none() {}

  void getBounds(_out_ Rect& bounds) override {
    bounds.x = bounds.y = bounds.w = bounds.h = 0.f;
  }

  void draw(Graphics2D& g2, float x, float y) override {
  }
};

sptr<TextLayout> TextLayout::create(const wstring& src, const sptr<Font>& font) {
  return sptr<TextLayout>(new TextLayout_none());
}

/**************************************************************************************************/

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

}  // namespace tex

#include "samples/samples.h"

int main(int argc, char* argv[]) {
  LaTeX::init();

  tex::Samples samples;
  for (int i = 0; i < samples.count(); i++) {
    auto r = LaTeX::parse(samples.next(), 720, 20, 20 / 3.f, black);
    Graphics2D_none g2;
    r->draw(g2, 0, 0);
    delete r;
  }

  LaTeX::release();
  Graphics2D_none::release();
  return 0;
}

#endif  // MEM_CHECK
