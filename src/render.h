#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include "graphic/graphic.h"

namespace tex {

class DefaultTeXFont;
class TeXFormula;
class Box;
class Atom;

class TeXRender {
private:
  static const color _defaultcolor;

  sptr<Box> _box;
  float _textSize;
  color _fg;
  Insets _insets;

public:
  static float _defaultSize;
  static float _magFactor;
  bool _iscolored;

  TeXRender(const sptr<Box> box, float textSize, bool trueValues = false);

  float getTextSize() const;

  int getHeight() const;

  int getDepth() const;

  int getWidth() const;

  float getBaseline() const;

  void setTextSize(float textSize);

  void setForeground(color fg);

  Insets getInsets();

  void setInsets(const Insets& insets, bool trueval = false);

  void setWidth(int width, int align);

  void setHeight(int height, int align);

  void draw(_out_ Graphics2D& g2, int x, int y);
};

class TeXRenderBuilder {
private:
  int _style, _type, _widthUnit, _align, _lineSpaceUnit;
  float _textSize, _textWidth, _lineSpace;
  bool _trueValues, _isMaxWidth;
  color _fg;

public:
  // TODO declaration conflict with TypefaceStyle defined in graphic/graphic.h
  enum TeXFontStyle {
    SERIF = 0,
    SANSSERIF = 1,
    BOLD = 2,
    ITALIC = 4,
    ROMAN = 8,
    TYPEWRITER = 16
  };

  TeXRenderBuilder()
      : _style(-1),
        _type(-1),
        _widthUnit(-1),
        _align(-1),
        _lineSpaceUnit(-1),
        _textSize(0),
        _textWidth(0),
        _lineSpace(0),
        _trueValues(false),
        _isMaxWidth(false),
        _fg(black) {}

  inline TeXRenderBuilder& setStyle(int style) {
    _style = style;
    return *this;
  }

  inline TeXRenderBuilder& setTextSize(float textSize) {
    _textSize = textSize;
    return *this;
  }

  inline TeXRenderBuilder& setType(int type) {
    _type = type;
    return *this;
  }

  inline TeXRenderBuilder& setForeground(color c) {
    _fg = c;
    return *this;
  }

  inline TeXRenderBuilder& setTrueValues(bool t) {
    _trueValues = t;
    return *this;
  }

  inline TeXRenderBuilder& setWidth(int unit, float width, int align) {
    _widthUnit = unit;
    _textWidth = width;
    _align = align;
    _trueValues = true;
    return *this;
  }

  inline TeXRenderBuilder& setIsMaxWidth(bool i) {
    if (_widthUnit == -1)
      throw ex_invalid_state("Cannot set 'isMaxWidth' without having specified a width!");
    if (i) {
      // Currently isMaxWidth==true does not work with
      // ALIGN_CENTER or ALIGN_RIGHT (see HorizontalBox constructor)
      //
      // The case (1) we don't support by setting align := ALIGN_LEFT
      // here is this:
      //      \text{hello world\\hello} with align=ALIGN_CENTER (but forced
      //      to ALIGN_LEFT) and isMaxWidth==true results in:
      //      [hello world]
      //      [hello ]
      // and NOT:
      //      [hello world]
      //      [ hello ]
      //
      // However, this case (2) is currently not supported anyway
      // (ALIGN_CENTER with isMaxWidth==false):
      //      [ hello world ]
      //      [ hello ]
      // and NOT:
      //      [ hello world ]
      //      [ hello ]
      //
      // => until (2) is solved, we stick with the hack to set align
      // := ALIGN_LEFT!
      _align = ALIGN_LEFT;
    }
    _isMaxWidth = i;
    return *this;
  }

  inline TeXRenderBuilder& setLineSpace(int unit, float space) {
    if (_widthUnit == -1) {
      throw ex_invalid_state("Cannot set line space without having specified a width!");
    }
    _lineSpace = space;
    _lineSpaceUnit = unit;
    return *this;
  }

  TeXRender* build(const sptr<Atom>& f);

  TeXRender* build(TeXFormula& f);

  static DefaultTeXFont* createFont(float size, int type);
};

}  // namespace tex

#endif  // RENDER_H_INCLUDED
