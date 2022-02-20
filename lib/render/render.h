#ifndef TINYTEX_RENDER_H
#define TINYTEX_RENDER_H

#include "config.h"
#include "graphic/graphic.h"
#include "utils/enums.h"

#include <functional>

namespace tinytex {

class Box;

class BoxGroup;

using BoxFilter = std::function<bool(const sptr<Box>&)>;

/** Object to paint formula, is baseline aligned. */
class TINYTEX_EXPORT Render {
private:
  static constexpr color DFT_COLOR = black;

  sptr<Box> _box;
  float _textSize;
  float _fixedScale;
  color _fg = black;

  void buildDebug(
    const sptr<BoxGroup>& parent,
    const sptr<Box>& box,
    const BoxFilter& filter
  );

  static sptr<BoxGroup> wrap(const sptr<Box>& box);

public:
  Render(const sptr<Box>& box, float textSize);

  /** Get the text size of this render */
  float getTextSize() const;

  /**
   * Get the total height of this render, equals to
   * ascent + descent (or depth, size below the baseline).
   */
  int getHeight() const;

  /** Get the size amount below the baseline, in positive. */
  int getDepth() const;

  /** Get the width of this render. */
  int getWidth() const;

  /** Get the ratio of ascent to total height. */
  float getBaseline() const;

  void setTextSize(float textSize);

  /** Set the foreground color to draw. */
  void setForeground(color fg);

  /** Draw the formula */
  void draw(Graphics2D& g2, int x, int y);
};

}  // namespace tinytex

#endif  // RENDER_H_INCLUDED
