#include "graphic/graphic.h"
#include "microtex.h"
#include "utils/utils.h"

namespace microtex {

class Font_none : public Font {
public:
  Font_none() = default;

  bool operator==(const Font& f) const override { return false; }
};

class TextLayout_none : public TextLayout {
public:
  TextLayout_none() = default;

  void getBounds(Rect& bounds) override {}

  void draw(Graphics2D& g2, float x, float y) override {}
};

class PlatformFactory_none : public PlatformFactory {
public:
  sptr<Font> createFont(const std::string& file) override { return sptrOf<Font_none>(); }

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

  color getColor() const override { return 0; }

  void setStroke(const Stroke& s) override { _stroke = s; }

  const Stroke& getStroke() const override { return _stroke; }

  void setStrokeWidth(float w) override {}

  void setDash(const std::vector<float>& dash) override {}

  std::vector<float> getDash() override { return std::vector<float>(); }

  sptr<Font> getFont() const override { return microtex::sptr<Font_none>(); }

  void setFont(const sptr<Font>& font) override {}

  float getFontSize() const override { return _fontSize; }

  void setFontSize(float size) override { _fontSize = size; }

  void translate(float dx, float dy) override {}

  void scale(float sx, float sy) override {}

  void rotate(float angle) override {}

  void rotate(float angle, float px, float py) override {}

  void reset() override {}

  float sx() const override { return 1; }

  float sy() const override { return 1; }

  void drawGlyph(u16 glyph, float x, float y) override {}

  bool beginPath(i32 id) override { return false; }

  void moveTo(float x, float y) override {}

  void lineTo(float x, float y) override {}

  void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) override {}

  void quadTo(float x1, float y1, float x2, float y2) override {}

  void closePath() override {}

  void fillPath(i32 id) override {}

  void drawLine(float x1, float y1, float x2, float y2) override {}

  void drawRect(float x, float y, float w, float h) override {}

  void fillRect(float x, float y, float w, float h) override {}

  void drawRoundRect(float x, float y, float w, float h, float rx, float ry) override {}

  void fillRoundRect(float x, float y, float w, float h, float rx, float ry) override {}
};

}  // namespace microtex

#include <chrono>

#include "samples.h"

template <typename F>
auto countDuration(const F& f) {
  auto start = std::chrono::high_resolution_clock::now();
  auto res = f();
  auto stop = std::chrono::high_resolution_clock::now();
  return std::make_pair(
    std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count(),
    res
  );
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    fprintf(
      stderr,
      "Required options:\n"
      "  <clm data file>\n"
      "  <math font file>\n"
      "  <samples file>\n"
    );
    return 1;
  }
  const microtex::FontSrcFile math{argv[1], argv[2]};
  microtex::MicroTeX::init(math);

  microtex::PlatformFactory::registerFactory(
    "none",
    std::make_unique<microtex::PlatformFactory_none>()
  );
  microtex::PlatformFactory::activate("none");

  long total = 0;
  microtex::Samples samples(argv[3]);
  for (int i = 0; i < samples.count(); i++) {
    auto [d, r] = countDuration([&]() {
      return microtex::MicroTeX::parse(samples.next(), 720, 20, 20 / 3.f, microtex::black);
    });
    total += d;
    printf("time: %ld(us)\n", d);
    microtex::Graphics2D_none g2;
    r->draw(g2, 0, 0);
    delete r;
  }
  printf("total time: %ld(us)\n", total);

  microtex::MicroTeX::release();
  return 0;
}
