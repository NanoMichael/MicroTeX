#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include <functional>

#include "utils/enums.h"
#include "box/box.h"
#include "graphic/graphic.h"

namespace tex {

class DefaultTeXFont;

class Formula;

class Box;

class Atom;

using BoxFilter = std::function<bool(const sptr<Box>&)>;

class TeXRender {
private:
  static const color _defaultcolor;

  sptr<Box> _box;
  float _textSize;
  color _fg = black;
  Insets _insets;

  void buildDebug(
    const sptr<BoxGroup>& parent,
    const sptr<Box>& box,
    BoxFilter&& filter
  );

  static sptr<BoxGroup> wrap(const sptr<Box>& box);

public:
  static float _defaultSize;
  static float _magFactor;

  TeXRender(const sptr<Box>& box, float textSize, bool trueValues = false);

  float getTextSize() const;

  int getHeight() const;

  int getDepth() const;

  int getWidth() const;

  float getBaseline() const;

  void setTextSize(float textSize);

  void setForeground(color fg);

  Insets getInsets();

  void setInsets(const Insets& insets, bool trueval = false);

  void setWidth(int width, Alignment align);

  void setHeight(int height, Alignment align);

  void draw(Graphics2D& g2, int x, int y);
};

class TeXRenderBuilder {
private:
  TexStyle _style = TexStyle::display;
  int _type = -1;
  UnitType _widthUnit = UnitType::none;
  UnitType _lineSpaceUnit = UnitType::none;
  float _textSize = 0, _textWidth = 0, _lineSpace = 0;
  bool _trueValues = false, _isMaxWidth = false;
  color _fg = black;
  Alignment _align = Alignment::none;

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

  TeXRenderBuilder() {}

  inline TeXRenderBuilder& setStyle(TexStyle style) {
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

  inline TeXRenderBuilder& setWidth(UnitType unit, float width, Alignment align) {
    _widthUnit = unit;
    _textWidth = width;
    _align = align;
    _trueValues = true;
    return *this;
  }

  inline TeXRenderBuilder& setIsMaxWidth(bool i) {
    if (_widthUnit == UnitType::none) {
      throw ex_invalid_state("Cannot set 'isMaxWidth' without having specified a width!");
    }
    if (i) {
      // Currently isMaxWidth==true does not work with
      // Alignment::center or Alignment::right (see HBox constructor)
      //
      // The case (1) we don't support by setting align := Alignment::left
      // here is this:
      //      \text{hello world\\hello} with align=Alignment::center (but forced
      //      to Alignment::left) and isMaxWidth==true results in:
      //      [hello world]
      //      [hello ]
      // and NOT:
      //      [hello world]
      //      [ hello ]
      //
      // However, this case (2) is currently not supported anyway
      // (Alignment::center with isMaxWidth==false):
      //      [ hello world ]
      //      [ hello ]
      // and NOT:
      //      [ hello world ]
      //      [ hello ]
      //
      // => until (2) is solved, we stick with the hack to set align
      // := Alignment::left!
      _align = Alignment::left;
    }
    _isMaxWidth = i;
    return *this;
  }

  inline TeXRenderBuilder& setLineSpace(UnitType unit, float space) {
    if (_widthUnit == UnitType::none) {
      throw ex_invalid_state("Cannot set line space without having specified a width!");
    }
    _lineSpace = space;
    _lineSpaceUnit = unit;
    return *this;
  }

  TeXRender* build(const sptr<Atom>& f);

  TeXRender* build(Formula& f);

  static DefaultTeXFont* createFont(float size, int type);
};

}  // namespace tex

#endif  // RENDER_H_INCLUDED
