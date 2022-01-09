#ifndef GRAPHIC_BASIC_H_INCLUDED
#define GRAPHIC_BASIC_H_INCLUDED

#include "utils/utils.h"
#include <string>

namespace tex {

using color = std::uint32_t;

static constexpr color transparent = 0x00000000;
static constexpr color black = 0xff000000;
static constexpr color white = 0xffffffff;
static constexpr color red = 0xffff0000;
static constexpr color green = 0xff00ff00;
static constexpr color blue = 0xff0000ff;
static constexpr color yellow = 0xffffff00;
static constexpr color cyan = 0xff00ffff;
static constexpr color magenta = 0xffff00ff;

static constexpr color TRANSPARENT = transparent;
static constexpr color BLACK = black;
static constexpr color WHITE = white;
static constexpr color RED = red;
static constexpr color GREEN = green;
static constexpr color BLUE = blue;
static constexpr color YELLOW = yellow;
static constexpr color CYAN = cyan;
static constexpr color MAGENTA = magenta;

/** Construct a 32 bit true color with its alpha, red, green and blue channel*/
inline color argb(int a, int r, int g, int b) {
  return (a << 24) | (r << 16) | (g << 8) | b;
}

inline color rgb(int r, int g, int b) {
  return argb(0xff, r, g, b);
}

inline color argb(float a, float r, float g, float b) {
  return argb((int) (a * 255), (int) (r * 255), (int) (g * 255), (int) (b * 255));
}

inline color rgb(float r, float g, float b) {
  return argb(1.f, r, g, b);
}

inline color color_a(color c) {
  return c >> 24;
}

inline color color_r(color c) {
  return c >> 16 & 0x00ff;
}

inline color color_g(color c) {
  return c >> 8 & 0x0000ff;
}

inline color color_b(color c) {
  return c & 0x000000ff;
}

inline color cmyk(float c, float m, float y, float k) {
  float kk = 1.f - k;
  return rgb(kk * (1 - c), kk * (1 - m), kk * (1 - y));
}

/** Test if the given color is transparent */
inline bool isTransparent(color c) {
  return (c >> 24 == 0);
}

/** Convert #AARRGGBB or #RRGGBB formatted string into color. */
color decodeColor(const std::string& s);

/** Represents a point in 2D plane */
struct Point {
  float x, y;

  Point() : x(0), y(0) {}

  Point(float x1, float y1) : x(x1), y(y1) {}
};

struct Rect {
  float x, y, w, h;

  Rect() : x(0), y(0), w(0), h(0) {}

  Rect(float x1, float y1, float w1, float h1) : x(x1), y(y1), w(w1), h(h1) {}
};

/** Stroke cap type */
enum Cap {
  CAP_BUTT,
  CAP_ROUND,
  CAP_SQUARE
};

/** Stroke join type */
enum Join {
  JOIN_BEVEL,
  JOIN_MITER,
  JOIN_ROUND
};

struct Stroke {
  float lineWidth;
  float miterLimit;
  Cap cap;
  Join join;

  Stroke()
    : lineWidth(1.f), cap(CAP_ROUND), join(JOIN_ROUND), miterLimit(0) {}

  Stroke(float w, Cap c, Join j, float ml = 0)
    : lineWidth(w), cap(c), join(j), miterLimit(ml) {}

  inline void set(float w, Cap c, Join j, float ml = 0) {
    lineWidth = w;
    cap = c;
    join = j;
    miterLimit = ml;
  }
};

}  // namespace tex

#endif  // GRAPHIC_BASIC_H_INCLUDED
