#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED

#include <map>
#include <vector>

#include "graphic/font_style.h"
#include "graphic/graphic_basic.h"

namespace microtex {

class Graphics2D;

/** Interface to represents a font. */
class Font {
public:
  /** Check if current font is equals to another */
  virtual bool operator==(const Font& f) const = 0;

  /** Check if current font is not equals to another */
  inline bool operator!=(const Font& f) const { return !(*this == f); }

  virtual ~Font() = default;
};

/** To layout the text that the program cannot recognize. */
class TextLayout {
public:
  /**
   * Get the layout bounds with current text and font
   *
   * @param bounds rectangle to retrieve the bounds. The metrics of the text layout
   * is arranged by following ways:
   *
   * - [bounds.w] takes the width of the layout
   * - [bounds.h] takes the height of the layout (ascent + descent)
   * - [bounds.y] takes the ascent (distance above the baseline, in negative) of the
   *   layout
   */
  virtual void getBounds(Rect& bounds) = 0;

  /**
   * Draw the layout
   *
   * @param g2 the graphics (2D) context
   * @param x the x coordinate
   * @param y the y coordinate, is baseline aligned
   */
  virtual void draw(Graphics2D& g2, float x, float y) = 0;

  virtual ~TextLayout() = default;
};

struct PlatformFactoryData;

/** Simple factory to create platform specific Font and TextLayout. */
class MICROTEX_EXPORT PlatformFactory {
private:
  static PlatformFactoryData* _data;

public:
  virtual ~PlatformFactory() = default;

  /**
   * Create font from file. You may cache the loaded platform specific font
   * to avoid repetitive loading.
   *
   * @param file the 'font file' doesn't have to be a real font file, it depends
   * on the font source you given before. See [lib/unimath/font_src.h: FontSrc]
   * for details.
   */
  virtual sptr<Font> createFont(const std::string& file) = 0;

  /**
   * Create a TextLayout with given text and font
   *
   * @param src the source text to layout and draw
   * @param style the font style. Unlike the fonts the engine using internally,
   * you are free to create the font you want based on the given style, the style
   * is for reference, not a constraint.
   * @param size the font size
   */
  virtual sptr<TextLayout>
  createTextLayout(const std::string& src, FontStyle style, float size) = 0;

  /** Register a factory */
  static void registerFactory(const std::string& name, std::unique_ptr<PlatformFactory> factory);

  /** Activate a factory */
  static void activate(const std::string& name);

  /** Get the current platform factory */
  static PlatformFactory* get();
};

/**
 * Abstract class to represents a graphics (2D) context, all the TeX drawing operations will on it.
 * It must have scale, translation, and rotation support.
 */
class Graphics2D {
public:
  virtual ~Graphics2D() = default;

  /**
   * Set the color of the context
   *
   * @param c required color
   */
  virtual void setColor(color c) = 0;

  /** Get the color of the context */
  virtual color getColor() const = 0;

  /** Set the stroke of the context */
  virtual void setStroke(const Stroke& s) = 0;

  /** Get the stroke of the context */
  virtual const Stroke& getStroke() const = 0;

  /** Set the stroke width of the context */
  virtual void setStrokeWidth(float w) = 0;

  /**
   * Set the dash style to draw the lines.
   * Each line can be drawn with a different pen dash. It defines the style of the line.
   * The pattern is set by the dash array, which is an array of positive floating point values.
   * They set the on and off parts of the dash pattern. We also specify the length of the array
   * and the offset value. If the length is 0, the dashing is disabled. If it is 1, a symmetric
   * pattern is assumed with alternating on and off portions of the size specified by the single
   * value in dashes.
   *
   * @param dash the dash pattern
   */
  virtual void setDash(const std::vector<float>& dash) = 0;

  /** Get the dash pattern to draw lines. */
  virtual std::vector<float> getDash() = 0;

  /** Get the current font */
  virtual sptr<Font> getFont() const = 0;

  /** Set the font of the context */
  virtual void setFont(const sptr<Font>& font) = 0;

  /** Get the font size */
  virtual float getFontSize() const = 0;

  /** Set font size */
  virtual void setFontSize(float size) = 0;

  /**
   * Translate the context by dx, dy
   *
   * @param dx distance in x-direction to translate
   * @param dy distance in y-direction to translate
   */
  virtual void translate(float dx, float dy) = 0;

  /**
   * Scale the context by sx, sy
   *
   * @param sx scale ratio in x-direction
   * @param sy scale ratio in y-direction
   */
  virtual void scale(float sx, float sy) = 0;

  /**
   * Rotate the context with the given angle (in radian) and pivot (0, 0).
   *
   * @param angle angle (in radian) amount to rotate
   */
  virtual void rotate(float angle) = 0;

  /**
   * Rotate the context with the given angle (in radian) and pivot (px, py).
   *
   * @param angle angle (in radian) amount to rotate
   * @param px pivot in x-direction
   * @param py pivot in y-direction
   */
  virtual void rotate(float angle, float px, float py) = 0;

  /** Reset transformations */
  virtual void reset() = 0;

  /**
   * Get the scale ratio of the context in x-direction
   *
   * @return the scale in x-direction
   */
  virtual float sx() const = 0;

  /**
   * Get the scale ratio of the context in y-direction
   *
   * @return the scale in y-direction
   */
  virtual float sy() const = 0;

  /**
   * Draw glyph, is baseline aligned
   *
   * @param glyph glyph index in the current font
   * @param x x-coordinate
   * @param y y-coordinate, is baseline aligned
   */
  virtual void drawGlyph(u16 glyph, float x, float y) = 0;

  // region path commands

  /**
   * Begin a new path. We promise every path begin with function [beginPath] and
   * end with [fillPath]. You may cache the path via its id.
   *
   * @param id the path id, if id < 0 that means the path is not cacheable.
   * @returns true if the path about to draw given by the id is exists, false otherwise.
   * The id is for optimizing purpose, if the engine find the path is already exists, the
   * following path drawing command will be ignored. If path caching is not supported,
   * just return false.
   */
  virtual bool beginPath(i32 id) = 0;

  /** Move to point (x, y). */
  virtual void moveTo(float x, float y) = 0;

  /** Add a line to path. */
  virtual void lineTo(float x, float y) = 0;

  /**
   * Add a cubic Bezier spline to path, with control points (x1, y1), (x2, y2)
   * and the final point (x3, y3).
   */
  virtual void cubicTo(float x1, float y1, float x2, float y2, float x3, float y3) = 0;

  /**
   * Add a quadratic Bezier spline to path, with control point (x1, y1) and the
   * final point (x2, y2).
   */
  virtual void quadTo(float x1, float y1, float x2, float y2) = 0;

  /** Close the path. */
  virtual void closePath() = 0;

  /**
   * Fill the path with the given id.
   *
   * @param id the path id. If the path is not cacheable, it always is -1.
   */
  virtual void fillPath(i32 id) = 0;

  // endregion

  // region shape commands

  /**
   * Draw line
   *
   * @param x1 start point in x-direction
   * @param y1 start point in y-direction
   * @param x2 end point in x-direction
   * @param y2 end point in y-direction
   */
  virtual void drawLine(float x1, float y1, float x2, float y2) = 0;

  /**
   * Draw rectangle
   *
   * @param x left position
   * @param y top position
   * @param w width
   * @param h height
   */
  virtual void drawRect(float x, float y, float w, float h) = 0;

  /**
   * Fill rectangle
   *
   * @param x left position
   * @param y top position
   * @param w width
   * @param h height
   */
  virtual void fillRect(float x, float y, float w, float h) = 0;

  /**
   * Draw round rectangle
   *
   * @param x left position
   * @param y top position
   * @param w width
   * @param h height
   * @param rx radius in x-direction
   * @param ry radius in y-direction
   */
  virtual void drawRoundRect(float x, float y, float w, float h, float rx, float ry) = 0;

  /**
   * Fill round rectangle
   *
   * @param x left position
   * @param y top position
   * @param w width
   * @param h height
   * @param rx radius in x-direction
   * @param ry radius in y-direction
   */
  virtual void fillRoundRect(float x, float y, float w, float h, float rx, float ry) = 0;

  // endregion
};

}  // namespace microtex

#endif  // GRAPHIC_H_INCLUDED
