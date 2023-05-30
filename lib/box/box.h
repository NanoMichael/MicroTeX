#ifndef MICROTEX_BOX_H
#define MICROTEX_BOX_H

#include <string>
#include <vector>

#include "utils/types.h"

namespace microtex {

class Graphics2D;

/**
 * An abstract graphical representation of a formula, that can be painted. All
 * characters, font sizes, positions are fixed. Only special Glue boxes could
 * possibly stretch or shrink (not supported by now). A box has 3 dimensions
 * (width, height and depth), will be used for further layout calculations.
 * <p>
 * Subclasses must implement the abstract
 * Box#draw(Graphics2D, float, float) method (that paints the box).
 * <p>
 * They must also implement the abstract Box#lastFontId() method (the last font
 * that will be used later when this box will be painted).
 */
class Box {
protected:
  /** Initialize box with default options */
  void init() {
    _width = _height = _depth = _shift = 0;
    _type = AtomType::none;
  }

public:
  /**
   * The width of this box, i.e. the value that will be used for further
   * calculations.
   */
  float _width = 0;

  /**
   * The height of this box, i.e. the value that will be used for further
   * calculations.
   */
  float _height = 0;

  /**
   * The depth of this box, i.e. the value that will be used for further
   * calculations.
   */
  float _depth = 0;

  /**
   * The shift amount: the meaning depends on the particular kind of box (up,
   * down, left, right)
   */
  float _shift = 0;

  /** The box type (default = none, no type) */
  AtomType _type = AtomType::none;

  /** Create a new box with default options */
  Box() { init(); }

  /** Copy the metrics from another box */
  void copyMetrics(const sptr<Box>& box);

  /** Transform the width of box to negative */
  inline void negWidth() { _width = -_width; }

  /** The length in vertical direction, basically equals to #_height + #_depth */
  inline float vlen() const { return _height + _depth; }

  /**
   * Paints this box at the given coordinates using the given 2D graphics
   * context.
   *
   * @param g2 the graphics (2D) context to use for painting
   * @param x the x-coordinate
   * @param y the y-coordinate
   */
  virtual void draw(Graphics2D& g2, float x, float y) = 0;

  /**
   * Get the id of the last font that will be used later when this box is to be
   * painted.
   *
   * @return the id of the last font that will be used later.
   */
  virtual int lastFontId();

  /** Get child boxes of this box. */
  virtual std::vector<sptr<Box>> descendants() const { return {}; }

  /** Replace child box */
  virtual void replaceFirst(const sptr<Box>& from, const sptr<Box>& to);

  /** Test if this box represents a space that only has metrics and has no visual effect. */
  virtual bool isSpace() const { return false; }

  /** Test if this box is a single box */
  virtual bool isSingle() const { return true; }

  /** The box name */
  virtual std::string name() const;

  /** String to print this box */
  virtual std::string toString() const;

  virtual ~Box() = default;

#ifndef boxname
#define boxname(x) \
  virtual std::string name() const override { return #x; }
#endif
};

/**
 * A box composed of other child boxes that can possibly be shifted (up, down,
 * left of right). Child boxes can also be positioned outside their parent's box
 * (defined by it's dimensions).
 */
class BoxGroup : public Box {
public:
  /** Children of this box */
  std::vector<sptr<Box>> _children{};

  /**
   * Append the given box to the end of the list of the child boxes.
   *
   * @param box the box to be append
   */
  virtual void add(const sptr<Box>& box);

  /**
   * Inserts the given box at the given position in the list of child boxes.
   *
   * @param pos the position at which to insert the given box
   * @param box the box to be inserted
   */
  virtual void add(int pos, const sptr<Box>& box);

  /**
   * Append the given box to the end of the list of the child boxes,
   * and the given box will not participate in the metrics calculation
   * of this composed box.
   * <p>
   * You shall not use this method to compose child boxes, it is for
   * special uses (for add debug information).
   *
   * @param box the box to be append
   */
  void addOnly(const sptr<Box>& box) { _children.push_back(box); }

  /** Child count of this box */
  inline int size() const { return _children.size(); }

  bool isSingle() const override { return false; }

  std::vector<sptr<Box>> descendants() const override { return _children; }

  int lastFontId() override;
};

/**
 * A box contains another box that will add some decorations when to draw
 * <p>
 * e.g. rotation, scale and so on
 */
class DecorBox : public Box {
public:
  sptr<Box> _base;

  explicit DecorBox(const sptr<Box>& base) : _base(base) {}

  bool isSingle() const override { return false; }

  int lastFontId() override;

  std::vector<sptr<Box>> descendants() const override { return {_base}; }
};

}  // namespace microtex

#endif  // MICROTEX_BOX_H
