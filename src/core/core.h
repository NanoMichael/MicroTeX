#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include <cstring>
#include <stack>

#include "common.h"
#include "fonts/fonts.h"
#include "core/glue.h"

namespace tex {

class Box;

#ifdef HAVE_LOG

void print_box(const sptr<Box>& box);

#endif  // HAVE_LOG

class BoxSplitter {
public:
  struct Position {
    int _index;
    sptr<HBox> _box;

    Position(int index, const sptr<HBox>& box)
      : _index(index), _box(box) {}
  };

private:
  static float canBreak(std::stack<Position>& stack, const sptr<HBox>& hbox, float width);

  static int getBreakPosition(const sptr<HBox>& hb, int index);

public:
  static sptr<Box> split(const sptr<Box>& box, float width, float lineSpace);

  static sptr<Box> split(const sptr<HBox>& hb, float width, float lineSpace);
};

/**
 * Contains the used TeXFont-object, color settings and the current style in
 * which a formula must be drawn. It's used in the createBox-methods. Contains
 * methods that apply the style changing rules for subformula's.
 */
class Environment {
private:
  // current style
  TexStyle _style;
  // TeXFont used
  sptr<TeXFont> _tf;
  // last used font
  int _lastFontId{};
  // Environment width
  float _textWidth{};

  // The text style to use
  std::string _textStyle;
  // If is small capital
  bool _smallCap{};
  float _scaleFactor{};
  // The unit of inter-line space
  UnitType _interlineUnit;
  // The inter line space
  float _interline{};

  // Member to store copies to prevent destruct
  sptr<Environment> _copy, _copytf, _cramp, _dnom;
  sptr<Environment> _num, _root, _sub, _sup;

  inline void init() {
    _style = TexStyle::display;
    _lastFontId = TeXFont::NO_FONT;
    _textWidth = POS_INF;
    _smallCap = false;
    _scaleFactor = 1.f;
    _interlineUnit = UnitType::em;
    _interline = 0;
  }

  Environment(
    TexStyle style, float scaleFactor,
    const sptr<TeXFont>& tf,
    const std::string& textstyle, bool smallCap  //
  ) {
    init();
    _style = style;
    _scaleFactor = scaleFactor;
    _tf = tf;
    _textStyle = textstyle;
    _smallCap = smallCap;
    setInterline(UnitType::ex, 1.f);
  }

public:
  Environment(TexStyle style, const sptr<TeXFont>& tf) {
    init();
    _style = style;
    _tf = tf;
    setInterline(UnitType::ex, 1.f);
  }

  Environment(TexStyle style, const sptr<TeXFont>& tf, UnitType widthUnit, float textWidth);

  inline void setInterline(UnitType unit, float len) {
    _interline = len;
    _interlineUnit = unit;
  }

  float getInterline() const;

  void setTextWidth(UnitType widthUnit, float width);

  inline float getTextWidth() const { return _textWidth; }

  inline void setScaleFactor(float f) { _scaleFactor = f; }

  inline float getScaleFactor() const { return _scaleFactor; }

  sptr<Environment>& copy();

  sptr<Environment>& copy(const sptr<TeXFont>& tf);

  /**
   * Copy of this envrionment in cramped style.
   */
  sptr<Environment>& crampStyle();

  /**
   * Style to display denominator.
   */
  sptr<Environment>& dnomStyle();

  /**
   * Style to display numerator.
   */
  sptr<Environment>& numStyle();

  /**
   * Style to display roots.
   */
  sptr<Environment>& rootStyle();

  /**
   * Style to display subscripts.
   */
  sptr<Environment>& subStyle();

  /**
   * Style to display superscripts.
   */
  sptr<Environment>& supStyle();

  inline float getSize() const { return _tf->getSize(); }

  inline TexStyle getStyle() const { return _style; }

  inline void setStyle(TexStyle style) { _style = style; }

  inline const std::string& getTextStyle() const { return _textStyle; }

  inline void setTextStyle(const std::string& style) { _textStyle = style; }

  inline bool getSmallCap() const { return _smallCap; }

  inline void setSmallCap(bool s) { _smallCap = s; }

  inline const sptr<TeXFont>& getTeXFont() const { return _tf; }

  inline float getSpace() const { return _tf->getSpace(_style) * _tf->getScaleFactor(); }

  inline void setLastFontId(int id) { _lastFontId = id; }

  inline int getLastFontId() const {
    return (_lastFontId == TeXFont::NO_FONT ? _tf->getMuFontId() : _lastFontId);
  }
};

}  // namespace tex

#endif  // CORE_H_INCLUDED
