#ifndef ATOM_H_INCLUDED
#define ATOM_H_INCLUDED

#undef DEBUG

#include "common.h"
#include "graphic/graphic.h"

#include <list>

namespace tex {

class TeXEnvironment;

/**
 * An abstract graphical representation of a formula, that can be painted. All
 * characters, font sizes, positions are fixed. Only special Glue boxes could
 * possibly stretch or shrink. A box has 3 dimensions (width, height and depth),
 * can be composed of other child boxes that can possibly be shifted (up, down,
 * left or right). Child boxes can also be positioned outside their parent's box
 * (defined by it's dimensions).
 * @par
 * Subclasses must implement the abstract
 * Box#draw(Graphics2D, float, float) method (that paints the box). <b>
 * This implementation must start with calling the method
 * Box#startDraw(Graphics2D, float, float) and end with calling the method
 * Box#endDraw(Graphics2D)} to set and restore the colors that must be used
 * for painting the box and to draw the background.</b> They must also implement
 * the abstract Box#getLastFontId() method (the last font that will be used later
 * when this box will be painted).
 */
class Box {
protected:
  /**
   * the color previously used
   */
  color _prevColor;

  /**
   * Stores the old color setting, draws the background of the box (if any)
   * and sets the foreground color (if any).
   *
   * @param g2
   *            the graphics (2D) context
   * @param x
   *            the x-coordinate
   * @param y
   *            the y-coordinate
   */
  void startDraw(Graphics2D& g2, float x, float y) {
    // old color
    _prevColor = g2.getColor();
    if (!istrans(_background)) {
      g2.setColor(_background);
      g2.fillRect(x, y - _height, _width, _height + _depth);
    }
    if (!istrans(_foreground)) {
      g2.setColor(_foreground);
    } else {
      g2.setColor(_prevColor);
    }
    if (DEBUG) drawDebug(g2, x, y);
  }

  void drawDebug(Graphics2D& g2, float x, float y, bool showDepth = true) {
    if (!DEBUG) return;
    const Stroke& st = g2.getStroke();
    Stroke s(std::abs(1.f / g2.sx()), CAP_BUTT, JOIN_MITER);
    g2.setStroke(s);
    if (_width < 0) {
      x += _width;
      _width = -_width;
    }
    // draw box outline
    g2.drawRect(x, y - _height, _width, _height + _depth);
    // draw depth
    if (showDepth) {
      color c = g2.getColor();
      g2.setColor(RED);
      if (_depth > 0) {
        g2.fillRect(x, y, _width, _depth);
        g2.setColor(c);
        g2.drawRect(x, y, _width, _depth);
      } else if (_depth < 0) {
        g2.fillRect(x, y + _depth, _width, -_depth);
        g2.setColor(c);
        g2.drawRect(x, y + _depth, _width, -_depth);
      } else {
        g2.setColor(c);
      }
    }
    g2.setStroke(st);
  }

  /**
   * Restores the previous graphic context setting.
   *
   * @param g2
   *            the graphics (2D) context
   */
  void endDraw(Graphics2D& g2) { g2.setColor(_prevColor); }

  /**
   * Initialize box with default options
   */
  void init() {
    _foreground = trans;
    _background = trans;
    _prevColor = trans;
    _width = _height = _depth = _shift = 0;
    _type = -1;
  }

public:
  static bool DEBUG;

  /**
   * The foreground color of the whole box. Child boxes can override this
   * color. If it's transparent and it has a parent box, the foreground color of the
   * parent will be used. If it has no parent, the foreground color of the
   * component on which it will be painted, will be used.
   */
  color _foreground;

  /**
   * The background color of the whole box. Child boxes can paint a background
   * on top of this background. If it's transparent, no background will be painted.
   */
  color _background;

  /**
   * The width of this box, i.e. the value that will be used for further
   * calculations.
   */
  float _width;

  /**
   * The height of this box, i.e. the value that will be used for further
   * calculations.
   */
  float _height;

  /**
   * The depth of this box, i.e. the value that will be used for further
   * calculations.
   */
  float _depth;

  /**
    * The shift amount: the meaning depends on the particular kind of box (up,
    * down, left, right)
    */
  float _shift;

  /**
   * The box type (default = -1, no type)
   */
  int _type;

  /**
   * Children of this box
   */
  std::vector<sptr<Box>> _children;

  /**
   * Create a new box with default options
   */
  Box() { init(); }

  /**
   * Create a new box with given foreground and background
   */
  Box(color fg, color bg) {
    init();
    _background = bg;
    _foreground = fg;
  }

  /**
   * Append the given box at the end of the list of child boxes.
   *
   * @param b
   *      the box to be inserted
   */
  virtual void add(const sptr<Box>& b) {
    _children.push_back(b);
  }

  /**
   * Inserts the given box at the given position in the list of child boxes.
   *
   * @param pos
   *      the position at which to insert the given box
   * @param b
   *      the box to be inserted
   */
  virtual void add(int pos, const sptr<Box>& b) {
    _children.insert(_children.begin() + pos, b);
  }

  /**
   * Transform the width of box to negative
   */
  inline void negWidth() { _width = -_width; }

  /**
   * Paints this box at the given coordinates using the given 2D graphics
   * context.
   *
   * @param g2
   *      the graphics (2D) context to use for painting
   * @param x
   *      the x-coordinate
   * @param y
   *      the y-coordinate
   */
  virtual void draw(Graphics2D& g2, float x, float y) = 0;

  /**
   * Get the id of the last font that will be used later when this box is to be
   * painted.
   *
   * @return the id of the last font that will be used later.
   */
  virtual int getLastFontId() = 0;

  /**
   * Get child boxes of this box, modification on the returned value by this function
   * will not effect this box's children
   */
  virtual std::vector<sptr<Box>> getChildren() const {
    return _children;
  }

  virtual ~Box() {}
};

/**
 * An abstract superclass for all logical mathematical constructions that can be
 * a part of a TeXFormula. All subclasses must implement the abstract
 * Atom#createBox(TeXEnvironment) method that transforms this logical unit
 * into a concrete box (that can be painted). They also must define their type,
 * used for determining what glue to use between adjacent atoms in a
 * "row construction". That can be one single type by assigning one of the type
 * constants to the #_type field. But they can also be defined as having
 * two types: a "left type" and a "right type". This can be done by implementing
 * the methods Atom#getLeftType() and Atom#getRightType(). The left type
 * will then be used for determining the glue between this atom and the previous
 * one (in a row, if any) and the right type for the glue between this atom and
 * the following one (in a row, if any).
 *
 */
class Atom {
public:
  /**
   * The type of the atom (default value: ordinary atom) @see TeXConstants
   */
  int _type;
  /**
   * The limits type of the atom (default value: nolimits) @see TeXConstants
   */
  int _typelimits;
  /**
   * The alignment type of the atom (default value: none) @see TeXConstants
   */
  int _alignment;

  Atom()
      : _type(TYPE_ORDINARY),
        _typelimits(SCRIPT_NOLIMITS),
        _alignment(-1) {}

  /**
   * Get the type of the leftermost child atom. Most atoms have no child
   * atoms, so the "left type" and the "right type" are the same: the atom's
   * type. This also is the default implementation. But Some atoms are
   * composed of child atoms put one after another in a horizontal row. These
   * atoms must override this method.
   *
   * @return the type of the leftermost child atom
   */
  virtual int getLeftType() const { return _type; }

  /**
   * Get the type of the rightermost child atom. Most atoms have no child
   * atoms, so the "left type" and the "right type" are the same: the atom's
   * type. This also is the default implementation. But Some atoms are
   * composed of child atoms put one after another in a horizontal row. These
   * atoms must override this method.
   *
   * @return the type of the rightermost child atom
   */
  virtual int getRightType() const { return _type; }

  /**
   * Convert this atom into a {@link Box}, using properties set by "parent"
   * atoms, like the TeX style, the last used font, color settings, ...
   *
   * @param env
   *      the current environment settings
   * @return the resulting box.
   */
  virtual sptr<Box> createBox(_out_ TeXEnvironment& env) = 0;

  /**
   * Shallow clone a atom from this atom.
   */
  virtual sptr<Atom> clone() const = 0;

  virtual ~Atom() {}

#ifndef __decl_clone
#define __decl_clone(type) \
  virtual sptr<Atom> clone() const override { return sptr<Atom>(new type(*this)); }
#endif
};

}  // namespace tex

#endif  // ATOM_H_INCLUDED
