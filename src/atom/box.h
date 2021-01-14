#ifndef BOX_H_INCLUDED
#define BOX_H_INCLUDED

#include "atom/atom.h"

#include <stack>

namespace tex {

class Char;
struct CharFont;
class TeXFont;
class SymbolAtom;

/***************************************************************************************************
 *                              factories to create boxes                                          *
 ***************************************************************************************************/

/**
 * Responsible for creating a box containing a delimiter symbol that exists in
 * different sizes.
 */
class DelimiterFactory {
public:
  static sptr<Box> create(_in_ SymbolAtom& symbol, _out_ TeXEnvironment& env, int size);

  /**
   * Create a delimiter with specified symbol name and min height
   *
   * @param symbol
   *      the name of the delimiter symbol
   * @param env
   *      the TeXEnvironment in which to create the delimiter box
   * @param minHeight
   *      the minimum required total height of the box (height + depth).
   * @return the box representing the delimiter variant that fits best
   *      according to the required minimum size.
   */
  static sptr<Box> create(const std::string& symbol, _out_ TeXEnvironment& env, float minHeight);
};

/**
 * Responsible for creating a box containing a delimiter symbol that exists in
 * different sizes.
 */
class XLeftRightArrowFactory {
private:
  static sptr<Atom> MINUS;
  static sptr<Atom> LEFT;
  static sptr<Atom> RIGHT;

public:
  static sptr<Box> create(bool left, _out_ TeXEnvironment& env, float width);

  static sptr<Box> create(_out_ TeXEnvironment& env, float width);
};

/***************************************************************************************************
 *                                        rule boxes                                             *
 ***************************************************************************************************/

/**
 * A box composed of a horizontal row of child boxes
 */
class HorizontalBox : public Box {
private:
  void recalculate(const Box& b);

  std::pair<sptr<HorizontalBox>, sptr<HorizontalBox>> split(int pos, int shift);

public:
  std::vector<int> _breakPositions;

  HorizontalBox() {}

  HorizontalBox(color fg, color bg) : Box(fg, bg) {}

  HorizontalBox(const sptr<Box>& b, float w, int alignment);

  HorizontalBox(const sptr<Box>& b);

  sptr<HorizontalBox> cloneBox();

  void add(const sptr<Box>& b) override;

  void add(int pos, const sptr<Box>& b) override;

  inline void addBreakPosition(int pos) {
    _breakPositions.push_back(pos);
  }

  std::pair<sptr<HorizontalBox>, sptr<HorizontalBox>> split(int pos) {
    return split(pos, 1);
  }

  std::pair<sptr<HorizontalBox>, sptr<HorizontalBox>> splitRemove(int pos) {
    return split(pos, 2);
  }

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;
};

/**
 * A box composed of other boxes, put one above the other
 */
class VerticalBox : public Box {
private:
  float _leftMostPos, _rightMostPos;

  void recalculateWidth(const Box& b);

public:
  VerticalBox() : _leftMostPos(F_MAX), _rightMostPos(F_MIN) {}

  VerticalBox(const sptr<Box>& b, float rest, int alignment);

  void add(const sptr<Box>& b) override;

  void add(const sptr<Box>& b, float interline);

  void add(int pos, const sptr<Box>& b) override;

  inline int getSize() const {
    return _children.size();
  }

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;
};

/**
 * A box representing another box with a horizontal rule above it, with
 * appropriate kerning.
 */
class OverBar : public VerticalBox {
public:
  OverBar() = delete;

  OverBar(const sptr<Box>& b, float kern, float thickness);
};

/**
 * A box representing another box with a delimiter box and a script box above or
 * under it, with script and delimiter separated by a kerning.
 */
class OverUnderBox : public Box {
private:
  // base, delimiter and script
  sptr<Box> _base, _del, _script;
  // kerning amount between the delimiter and the script
  float _kern;
  // whether the delimiter should be drawn over (<->under) the base box
  bool _over;

public:
  OverUnderBox() = delete;

  /**
   * The parameter boxes must have an equal width!!
   *
   * @param base
   *      base box to be drawn on the baseline
   * @param del
   *      delimiter box
   * @param script
   *      subscript or superscript box
   * @param kern
   *      the kerning amount to draw
   * @param over
   *      true : draws delimiter and script box above the base box,
   *      false : under the base box
   */
  OverUnderBox(
      const sptr<Box>& base, const sptr<Box>& del, const sptr<Box>& script, float kern, bool over);

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;

  std::vector<sptr<Box>> getChildren() const override;
};

/**
 * A box representing a horizontal line.
 */
class HorizontalRule : public Box {
private:
  color _color;
  float _speShift;

public:
  HorizontalRule() = delete;

  HorizontalRule(float thickness, float width, float shift);

  HorizontalRule(float thickness, float width, float shift, bool trueShift);

  HorizontalRule(float thickness, float width, float shift, color c, bool trueshift);

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;
};

/***************************************************************************************************
 *                                   operation boxes                                               *
 ***************************************************************************************************/

/**
 * A box representing a scale operation
 */
class ScaleBox : public Box {
private:
  sptr<Box> _box;
  float _sx, _sy;
  float _factor;

  void init(const sptr<Box>& b, float sx, float sy);

public:
  ScaleBox() = delete;

  ScaleBox(const sptr<Box>& b, float sx, float sy) {
    init(b, sx, sy);
  }

  ScaleBox(const sptr<Box>& b, float factor) {
    init(b, factor, factor);
    _factor = factor;
  }

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;

  std::vector<sptr<Box>> getChildren() const override;
};

/**
 * A box representing a reflected box
 */
class ReflectBox : public Box {
private:
  sptr<Box> _box;

public:
  ReflectBox() = delete;

  ReflectBox(const sptr<Box>& b);

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;

  std::vector<sptr<Box>> getChildren() const override;
};

/**
 * Enumeration representing rotation origin
 */
enum Rotation {
  // Bottom Left
  BL,
  // Bottom Center
  BC,
  // Bottom Right
  BR,
  // Top Left
  TL,
  // Top Center
  TC,
  // Top Right
  TR,
  // Bottom Bottom Left
  BBL,
  // Bottom Bottom Right
  BBR,
  // Bottom Bottom Center
  BBC,
  // Center Left
  CL,
  // Center Center
  CC,
  // Center Right
  CR
};

/**
 * A box representing a rotate operation
 */
class RotateBox : public Box {
private:
  sptr<Box> _box;
  float _angle;
  float _xmax, _xmin, _ymax, _ymin;
  int _option;
  float _shiftX, _shiftY;

  void init(const sptr<Box>& b, float angle, float x, float y);

  static Point calculateShift(const Box& b, int option);

public:
  RotateBox() = delete;

  RotateBox(const sptr<Box>& b, float angle, float x, float y) {
    init(b, angle, x, y);
  }

  RotateBox(const sptr<Box>& b, float angle, const Point& origin) {
    init(b, angle, origin.x, origin.y);
  }

  RotateBox(const sptr<Box>& b, float angle, int option) {
    const Point& p = calculateShift(*b, option);
    init(b, angle, p.x, p.y);
  }

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;

  std::vector<sptr<Box>> getChildren() const override;

  static int getOrigin(std::string option);
};

/***************************************************************************************************
 *                                  wrapped boxes                                                  *
 ***************************************************************************************************/

/**
 * A box representing a wrapped box by square frame
 */
class FramedBox : public Box {
public:
  sptr<Box> _box;
  float _thickness;
  float _space;
  color _line;
  color _bg;

  void init(const sptr<Box>& box, float thickness, float space);

public:
  FramedBox() = delete;

  FramedBox(const sptr<Box>& box, float thickness, float space) {
    init(box, thickness, space);
  }

  FramedBox(const sptr<Box>& box, float thickness, float space, color line, color bg) {
    init(box, thickness, space);
    _line = line;
    _bg = bg;
  }

  virtual void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;

  std::vector<sptr<Box>> getChildren() const override;
};

/**
 * A box representing a wrapped box by oval frame
 */
class OvalBox : public FramedBox {
private:
  float _multiplier, _diameter;

public:
  OvalBox() = delete;

  OvalBox(
      const sptr<FramedBox>& fbox,
      float multiplier = 0.5f,
      float diameter = 0.f)
      : FramedBox(fbox->_box, fbox->_thickness, fbox->_space),
        _multiplier(multiplier),
        _diameter(diameter) {}

  void draw(Graphics2D& g2, float x, float y) override;
};

/**
 * A box representing a wrapped box by shadowed frame
 */
class ShadowBox : public FramedBox {
private:
  float _shadowRule;

public:
  ShadowBox() = delete;

  ShadowBox(const sptr<FramedBox>& fbox, float shadowRule)
      : FramedBox(fbox->_box, fbox->_thickness, fbox->_space) {
    _shadowRule = shadowRule;
    _depth += shadowRule;
    _width += shadowRule;
  }

  void draw(Graphics2D& g2, float x, float y) override;
};

/***************************************************************************************************
 *                                      basic boxes                                                *
 ***************************************************************************************************/

/**
 * A box representing whitespace
 */
class StrutBox : public Box {
public:
  StrutBox() = delete;

  StrutBox(float w, float h, float d, float s) {
    _width = w;
    _height = h;
    _depth = d;
    _shift = s;
  }

  void draw(Graphics2D& g2, float x, float y) override {
    // no visual effect
  }

  int getLastFontId() override;
};

/**
 * A box representing glue
 */
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

  int getLastFontId() override;
};

/**
 * A box representing a single character
 */
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
   * @param c
   *      a Char-object containing the character's font information.
   */
  CharBox(const Char& c);

  void addItalicCorrectionToWidth();

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;
};

/**
 * A box representing a text rendering box
 */
class TextRenderingBox : public Box {
private:
  static sptr<Font> _font;
  sptr<TextLayout> _layout;
  float _size;

  void init(const std::wstring& str, int type, float size, const sptr<Font>& f, bool kerning);

public:
  TextRenderingBox() = delete;

  TextRenderingBox(const std::wstring& str, int type, float size, const sptr<Font>& f, bool kerning) {
    init(str, type, size, f, kerning);
  }

  TextRenderingBox(const std::wstring& str, int type, float size) {
    init(str, type, size, sptr<Font>(_font), true);
  }

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;

  static void setFont(const std::string& name);

  static void _init_();

  static void _free_();
};

/**
 * A box representing 'wrapper' that with insets in left, top, right and bottom
 */
class WrapperBox : public Box {
private:
  sptr<Box> _base;
  float _l;

public:
  WrapperBox() = delete;

  WrapperBox(const sptr<Box>& base) : _base(base), _l(0) {
    _height = _base->_height;
    _depth = _base->_depth;
    _width = _base->_width;
  }

  WrapperBox(const sptr<Box>& base, float width, float rowheight, float rowdepth, float align)
      : _base(base), _l(0) {
    _height = rowheight;
    _depth = rowdepth;
    _width = width;
    if (base->_width < 0) _width += base->_width;
    if (align == ALIGN_RIGHT) {
      _l = width - _base->_width;
    } else if (align == ALIGN_CENTER) {
      _l = (width - _base->_width) / 2.f;
    }
  }

  void setInsets(float l, float t, float r, float b);

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;

  std::vector<sptr<Box>> getChildren() const override;
};

/**
 * Class representing a box that shifted up or down (when shift is negative)
 */
class ShiftBox : public Box {
private:
  float _sf;
  sptr<Box> _base;

public:
  ShiftBox() = delete;

  ShiftBox(const sptr<Box>& base, float shift) : _base(base), _sf(shift) {}

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;

  std::vector<sptr<Box>> getChildren() const override;
};

/**
 * Class represents several lines
 */
class LineBox : public Box {
private:
  // Every 4 elements represent a line, thus (x1, y1, x2, y2)
  std::vector<float> _lines;
  float _thickness;
  int _lineCount;

public:
  LineBox() = delete;

  LineBox(const std::vector<float> lines, float thickness);

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;
};

/**
 * Class representing a box covered by another box
 */
class OverlappedBox : public Box {
private:
  sptr<Box> _base;
  sptr<Box> _overlap;

public:
  OverlappedBox() = delete;

  OverlappedBox(const sptr<Box> base, const sptr<Box> overlap)
      : _base(base), _overlap(overlap) {
    _width = base->_width;
    _height = base->_height;
    _depth = base->_depth;
    _shift = base->_shift;
    _type = base->_type;
  }

  void draw(Graphics2D& g2, float x, float y) override;

  int getLastFontId() override;

  std::vector<sptr<Box>> getChildren() const override;
};

}  // namespace tex

#endif  // BOX_H_INCLUDED
