#include "graphic/graphic.h"
#include "latex.h"

namespace tex {

class Font_none : public Font {
public:
  Font_none() = default;

  bool operator==(const Font& f) const override {
    return false;
  }
};

class TextLayout_none : public TextLayout {
public:
  TextLayout_none() = default;

  void getBounds(Rect& bounds) override {}

  void draw(Graphics2D& g2, float x, float y) override {}
};

class PlatformFactory_none : public PlatformFactory {
public:
  sptr<Font> createFont(const std::string& file) override {
    return sptrOf<Font_none>();
  }

  sptr<TextLayout> createTextLayout(const std::string& src, FontStyle style, float size) override {
    return sptrOf<TextLayout_none>();
  }
};

class Graphics2D_none : public Graphics2D {
private:
  Stroke _stroke;
  float _fontSize = 100;
public:
  void setColor(color c) override {}

  color getColor() const override {
    return 0;
  }

  void setStroke(const Stroke& s) override {
    _stroke = s;
  }

  const Stroke& getStroke() const override {
    return _stroke;
  }

  void setStrokeWidth(float w) override {}

  void setDash(const std::vector<float>& dash) override {}

  std::vector<float> getDash() override {
    return std::vector<float>();
  }

  sptr<Font> getFont() const override {
    return tex::sptr<Font_none>();
  }

  void setFont(const sptr<Font>& font) override {}

  float getFontSize() const override {
    return _fontSize;
  }

  void setFontSize(float size) override {
    _fontSize = size;
  }

  void translate(float dx, float dy) override {}

  void scale(float sx, float sy) override {}

  void rotate(float angle) override {}

  void rotate(float angle, float px, float py) override {}

  void reset() override {}

  float sx() const override {
    return 1;
  }

  float sy() const override {
    return 1;
  }

  void drawGlyph(u16 glyph, float x, float y) override {}

  void moveTo(float x, float y) override {}

  void lineTo(float x, float y) override {}

  void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) override {}

  void quadTo(float x1, float y1, float x2, float y2) override {}

  void closePath() override {}

  void fillPath() override {}

  void drawLine(float x1, float y1, float x2, float y2) override {}

  void drawRect(float x, float y, float w, float h) override {}

  void fillRect(float x, float y, float w, float h) override {}

  void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override {}

  void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override {}
};

}

#include "samples.h"

int main(int argc, char* argv[]) {
  const tex::FontSpec math{
    "xits",
    "/home/nano/Downloads/xits/XITSMath-Regular.otf",
    "../../res/XITSMath-Regular.clm"
  };
  tex::LaTeX::init(math);

  tex::PlatformFactory::registerFactory("none", std::make_unique<tex::PlatformFactory_none>());
  tex::PlatformFactory::activate("none");

  tex::Samples samples("../../res/SAMPLES.tex");
  for (int i = 0; i < samples.count(); i++) {
    auto r = tex::LaTeX::parse(samples.next(), 720, 20, 20 / 3.f, tex::black);
    tex::Graphics2D_none g2;
    r->draw(g2, 0, 0);
    delete r;
  }

  tex::LaTeX::release();
  return 0;
}