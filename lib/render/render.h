#ifndef MICROTEX_RENDER_H
#define MICROTEX_RENDER_H

#include "graphic/graphic.h"
#include "microtexexport.h"

namespace microtex {

class Box;

struct RenderData;

/** Object to paint formula, is baseline aligned. */
class MICROTEX_EXPORT Render {
private:
  RenderData* _data;

public:
  Render(const sptr<Box>& box, float textSize, bool isSplit = false);

  ~Render();

  /** Get the text size of this render */
  float getTextSize() const;

  /**
   * Get the total height of this render, equals to
   * ascent + descent (or namely "depth", distance below the baseline).
   */
  int getHeight() const;

  /** Get the distance below the baseline, in positive. */
  int getDepth() const;

  /** Get the width of this render. */
  int getWidth() const;

  /** Get the ratio of ascent to total height. */
  float getBaseline() const;

  /** Test if is split to multi-lines. */
  bool isSplit() const;

  /**
   * Set text size to draw. The dimension (width, height, depth) will be
   * changed following the text size change.
   */
  void setTextSize(float textSize);

  /** Set the foreground color to draw. */
  void setForeground(color fg);

  /** Draw the formula */
  void draw(Graphics2D& g2, int x, int y);
};

}  // namespace microtex

#endif  // MICROTEX_RENDER_H
