#ifndef GRAPHIC_BASIC_H_INCLUDED
#define GRAPHIC_BASIC_H_INCLUDED

#include <sstream>
#include <string>
#include "common.h"

namespace tex {

using color = std::uint32_t;

static const color transparent = 0x00000000;
static const color black = 0xff000000;
static const color white = 0xffffffff;
static const color red = 0xffff0000;
static const color green = 0xff00ff00;
static const color blue = 0xff0000ff;
static const color yellow = 0xffffff00;
static const color cyan = 0xff00ffff;
static const color magenta = 0xffff00ff;

#define TRANSPARENT (transparent)
#define BLACK (black)
#define WHITE (white)
#define RED (red)
#define GREEN (green)
#define BLUE (blue)
#define YELLOW (yellow)
#define CYAN (cyan)
#define MAGENTA (magenta)

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
inline color decode(const std::string& s) {
  if (s[0] == '#') {
    const std::string x = s.substr(1);
    std::stringstream ss;
    ss << std::hex << x;
    color c;
    ss >> c;
    if (s.size() == 7) {
      // set alpha value
      c |= 0xff000000;
    } else if (s.size() != 9) {
      return black;
    }
    return c;
  }
  return black;
}

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

struct Insets {
  int left, top, right, bottom;

  Insets() : left(0), top(0), right(0), bottom(0) {}

  Insets(int t, int l, int b, int r) : left(t), top(t), right(r), bottom(b) {}

  void set(int t, int l, int b, int r) {
    left = l;
    top = t;
    right = r;
    bottom = b;
  }

  Insets& operator+=(const int offset) {
    left += offset;
    top += offset;
    right += offset;
    bottom += offset;
    return *this;
  }
};

enum Cap {
  CAP_BUTT,
  CAP_ROUND,
  CAP_SQUARE
};
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

  void setStroke(float w, Cap c, Join j, float ml = 0) {
    lineWidth = w;
    cap = c;
    join = j;
    miterLimit = ml;
  }
};

}  // namespace tex

#endif  // GRAPHIC_BASIC_H_INCLUDED
