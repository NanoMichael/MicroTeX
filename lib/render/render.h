#ifndef TINYTEX_RENDER_H
#define TINYTEX_RENDER_H

#include "config.h"
#include "graphic/graphic.h"

#include <functional>

namespace tinytex {

class Box;

struct RenderConfig;

/** Object to paint formula, is baseline aligned. */
class TINYTEX_EXPORT Render {
private:
  RenderConfig* _config;

public:
  Render(const sptr<Box>& box, float textSize);

  ~Render();

  /** Get the text size of this render */
  float getTextSize() const;

  /**
   * Get the total height of this render, equals to
   * ascent + descent (or namely "depth", height below the baseline).
   */
  int getHeight() const;

  /** Get the height below the baseline, in positive. */
  int getDepth() const;

  /** Get the width of this render. */
  int getWidth() const;

  /** Get the ratio of ascent to total height. */
  float getBaseline() const;

  /**
   * Set the text size to draw. Notice that the dimension (width, height, depth)
   * will change follow the text size change.
   */
  void setTextSize(float textSize);

  /** Set the foreground color to draw. */
  void setForeground(color fg);

  /** Draw the formula */
  void draw(Graphics2D& g2, int x, int y);
};

}  // namespace tinytex

#endif  // RENDER_H_INCLUDED
