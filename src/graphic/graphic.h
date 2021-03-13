#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED

#include "graphic/graphic_basic.h"

namespace tex {

class Graphics2D;

enum TypefaceStyle {
  PLAIN = 0,
  BOLD = 1,
  ITALIC = 2,
  BOLDITALIC = BOLD | ITALIC
};

/** Abstract class represents a font. */
class Font {
public:
  /** Get the font size */
  virtual float getSize() const = 0;

  /**
   * Derive font from current font with given style
   *
   * @param style
   *      required style, defined in enum TypefaceStyle,
   *      we use integer type to represents style here
   */
  virtual sptr<Font> deriveFont(int style) const = 0;

  /** Check if current font equals another */
  virtual bool operator==(const Font& f) const = 0;

  /** Check if current font not equals another */
  virtual bool operator!=(const Font& f) const = 0;

  virtual ~Font() = default;;

  /**
   * Create font from file
   * 
   * @param file the path of the font file
   * @param size required font size
   */
  static Font* create(const std::string& file, float size);

  /**
   * Create font with given name, style and size
   *
   * @param name the font name
   * @param style the font style
   * @param size the font size
   */
  static sptr<Font> _create(const std::string& name, int style, float size);
};

/** To layout the text that the program cannot recognize. */
class TextLayout {
public:
  /**
   * Get the layout bounds with current text and font
   *
   * @param bounds rectangle to retrieve the bounds
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

  /**
   * Create a TextLayout with given text and font
   *
   * @param src the text to layout
   * @param font the specified font
   * @return new TextLayout
   */
  static sptr<TextLayout> create(const std::wstring& src, const sptr<Font>& font);
};

/**
 * Abstract class to represents a graphics (2D) context, all the TeX drawing operations will on it.
 * It must have scale, translation, and rotation support. You should notice that the scaling on
 * y-direction will be selected as the base if they are different on x and y-direction when drawing
 * characters. In most cases, you should never use different scalings, unless you are really sure the
 * coordinates are correct (i.e. draw a hyphen).
 */
class Graphics2D {
public:
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

  /** Get the current font */
  virtual const Font* getFont() const = 0;

  /** Set the font of the context */
  virtual void setFont(const Font* font) = 0;

  /**
   * Translate the context with distance dx, dy
   *
   * @param dx distance in x-direction to translate
   * @param dy distance in y-direction to translate
   */
  virtual void translate(float dx, float dy) = 0;

  /**
   * Scale the context with sx, sy
   *
   * @param sx scale ratio in x-direction
   * @param sy scale ratio in y-direction
   */
  virtual void scale(float sx, float sy) = 0;

  /**
   * Rotate the context with the given angle (in radian), with pivot (0, 0).
   *
   * @param angle angle (in radian) amount to rotate
   */
  virtual void rotate(float angle) = 0;

  /**
   * Rotate the context with the given angle (in radian), with pivot (px, py).
   *
   * @param angle angle (in radian) amount to rotate
   * @param px pivot in x-direction
   * @param py pivot in y-direction
   */
  virtual void rotate(float angle, float px, float py) = 0;

  /** Reset transformations */
  virtual void reset() = 0;

  /**
   * Get the scale of the context in x-direction
   *
   * @return the scale in x-direction
   */
  virtual float sx() const = 0;

  /**
   * Get the scale of the context in y-direction
   *
   * @return the scale in y-direction
   */
  virtual float sy() const = 0;

  /**
   * Draw character, is baseline aligned
   * 
   * @param c specified character
   * @param x x-coordinate
   * @param y y-coordinate, is baseline aligned
   */
  virtual void drawChar(wchar_t c, float x, float y) = 0;

  /**
   * Draw text, is baseline aligned
   * 
   * @param c specified text
   * @param x x-coordinate
   * @param y y-coordinate, is baseline aligned
   */
  virtual void drawText(const std::wstring& c, float x, float y) = 0;

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
};

}  // namespace tex

#endif  // GRAPHIC_H_INCLUDED
