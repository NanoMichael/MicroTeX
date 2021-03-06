#ifndef LATEX_BOX_SINGLE_H
#define LATEX_BOX_SINGLE_H

#include "atom/atom.h"

namespace tex {

class Char;

class CharFont;

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
};

/** A box representing glue */
class GlueBox : public Box {
public:
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
};

/** A box representing a single character */
class CharBox : public Box {
private:
  sptr<CharFont> _cf;
  float _size;
  float _italic;

public:
  CharBox() = delete;

  /**
   * Create a new CharBox that will represent the character defined by the
   * given Char-object.
   *
   * @param chr a Char-object containing the character's font information.
   */
  explicit CharBox(const Char& chr);

  void addItalicCorrectionToWidth();

  void draw(Graphics2D& g2, float x, float y) override;

  int lastFontId() override;
};

/** A box representing a text rendering box */
class TextRenderingBox : public Box {
private:
  static sptr<Font> _font;
  sptr<TextLayout> _layout;
  float _size{};

  void init(const std::wstring& str, int type, float size, const sptr<Font>& font, bool kerning);

public:
  TextRenderingBox() = delete;

  TextRenderingBox(
    const std::wstring& str, int type, float size,
    const sptr<Font>& font, bool kerning
  ) {
    init(str, type, size, font, kerning);
  }

  TextRenderingBox(const std::wstring& str, int type, float size) {
    init(str, type, size, sptr<Font>(_font), true);
  }

  void draw(Graphics2D& g2, float x, float y) override;

  static void setFont(const std::string& name);

  static void _init_();

  static void _free_();
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
};

/** A box representing a line. */
class RuleBox : public Box {
private:
  color _color;
  float _speShift;

public:
  RuleBox() = delete;

  RuleBox(
    float thickness, float width, float shift,
    color c = transparent, bool trueshift = true
  );

  void draw(Graphics2D& g2, float x, float y) override;
};

class DebugBox : public Box {
public:
  explicit DebugBox(const sptr<Box>& base);

  void draw(Graphics2D& g2, float x, float y) override;
};

}

#endif //LATEX_BOX_SINGLE_H
