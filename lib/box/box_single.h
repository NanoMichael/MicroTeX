#ifndef MICROTEX_BOX_SINGLE_H
#define MICROTEX_BOX_SINGLE_H

#include "atom/atom.h"
#include "graphic/font_style.h"
#include "graphic/graphic_basic.h"
#include "unimath/uni_char.h"

namespace microtex {

class TextLayout;

/** A box representing whitespace */
class StrutBox : public Box {
public:
  StrutBox() = delete;

  explicit StrutBox(const sptr<Box>& box) noexcept {
    copyMetrics(box);
    _shift = _shift;
  }

  StrutBox(float width, float height, float depth, float shift) noexcept {
    _width = width;
    _height = height;
    _depth = depth;
    _shift = shift;
  }

  void draw(Graphics2D& g2, float x, float y) override {
    // no visual effect
  }

  bool isSpace() const override { return true; }

  boxname(StrutBox);

  static sptr<StrutBox> empty() noexcept { return sptrOf<StrutBox>(0.f, 0.f, 0.f, 0.f); }

  static sptr<StrutBox> create(float width) noexcept {
    return sptrOf<StrutBox>(width, 0.f, 0.f, 0.f);
  }
};

/** A box representing glue */
class GlueBox : public Box {
public:
  // Not used by now
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

  bool isSpace() const override { return true; }

  boxname(GlueBox);
};

/** A box representing a single character */
class CharBox : public Box {
private:
  Char _chr;

public:
  CharBox() = delete;

  /**
   * Create a new CharBox that will represent the character defined by the
   * given Char-object.
   *
   * @param chr a Char-object containing the character's font information.
   */
  explicit CharBox(const Char& chr);

  inline float italic() const { return _chr.italic(); }

  void draw(Graphics2D& g2, float x, float y) override;

  int lastFontId() override;

  std::string toString() const override;

  boxname(CharBox);
};

/**
 * Box to draw text. The metrics measurement and drawing is
 * delegated to rendering backend.
 */
class TextBox : public Box {
private:
  sptr<TextLayout> _layout;

public:
  TextBox() = delete;

  TextBox(const std::string& str, FontStyle style, float size);

  void draw(Graphics2D& g2, float x, float y) override;

  boxname(TextBox);
};

/** Class represents several lines */
class LineBox : public Box {
private:
  // Every 4 elements represent a line, thus (x1, y1, x2, y2)
  std::vector<float> _lines;
  float _thickness;

public:
  LineBox() = delete;

  LineBox(const std::vector<float>& lines, float thickness);

  void draw(Graphics2D& g2, float x, float y) override;

  boxname(LineBox);
};

/** A box representing a line. */
class RuleBox : public Box {
private:
  color _color;
  float _speShift;

public:
  RuleBox() = delete;

  RuleBox(float thickness, float width, float shift, color c = transparent, bool trueshift = true);

  void draw(Graphics2D& g2, float x, float y) override;

  boxname(RuleBox);
};

class DebugBox : public Box {
public:
  explicit DebugBox(const sptr<Box>& base);

  void draw(Graphics2D& g2, float x, float y) override;

  boxname(DebugBox);
};

}  // namespace microtex

#endif  // MICROTEX_BOX_SINGLE_H
