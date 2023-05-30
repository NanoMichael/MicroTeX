#ifndef MICROTEX_PATH_H
#define MICROTEX_PATH_H

#include "otf/otfconfig.h"
#include "utils/utils.h"

namespace microtex {

/** Get the arguments count of the given path command. */
u16 pathCmdArgsCount(char cmd);

}  // namespace microtex

#ifdef HAVE_GLYPH_RENDER_PATH

#include <string>

namespace microtex {

class Graphics2D;

/** Abstract graphical path command, with its drawing command and points. */
struct PathCmd {
  /**
   * Path drawing command, one of the following:
   * @code
   * cmd  args       desc
   * M    x, y       Move to point (x, y).
   *
   * m    dx, dy     Given a current point (x0, y0), move to (x0 + dx, y0 + dy).
   *
   * L    x, y       Line to (x, y).
   *
   * l    dx, dy     Given a current point (x0, y0), line to (x0 + dx, y0 + dy).
   *
   * H    x          Given a current point (x0, y0), line to (x, y0).
   *
   * h    dx         Given a current point (x0, y0), line to (x0 + dx, y0).
   *
   * V    y          Given a current point (x0, y0), line to (x0, y).
   *
   * v    dy         Given a current point (x0, y0), line to (x0, y0 + dy).
   *
   * C    cx1, cy1,  Adds a cubic Bezier spline to path from current point to position
   *      cx2, cy2,  (x, y), using (cx1, cy1) and (cx2, cy2) as the control points, the
   *      x, y       current point will be (x, y).
   *
   * c    dx1, dy1,  Given a current point (x0, y0), c(dx1, dy1, dx2, dy2, dx, dy) is
   *      dx2, dy2,  logically equivalent to
   *      dx, dy     C(x0 + dx1, y0 + dy1, x0 + dx2, y0 + dy2, x0 + dx, y0 + dy).
   *
   * S    cx, cy,    Produces the same type of curve as 'C', but if it follows another
   *      x, y       'S' command or a 'C' command, the first control point is assumed to
   *                 be a reflection of the one used previously.
   *
   * s    cx, cy,    Given a current point (x0, y0), s(cx, cy, dx, dy) is logically
   *      dx, dy     equivalent to S(x0 + cx, y0 + cy, x0 + dx, y0 + dy).
   *
   * Q    cx, cy     Simpler curve than the cubic one. With its control point (cx, cy)
   *      x, y       and final point (x, y).
   *
   * q    cx, cy     Given a current point (x0, y0), q(cx, cy, dx, dy) is logically
   *      dx, dy     equivalent to Q(x0 + cx, y0 + cy, x0 + dx, y0 + dy).
   *
   * T    x, y       Like the command 'S', if 'T' follows another curve command, the first
   *                 control point is assumed to be a reflection of the one used previously.
   *
   * t    dx, dy     Same as 'T', but the params are given by the offset of the final point.
   *
   * Z    N/A        Close the path.
   *
   * z    N/A        Close the path.
   * @endcode
   *
   * See <a href="https://developer.mozilla.org/en-US/docs/Web/SVG/Tutorial/Paths">this
   * tutorial</a> for details.
   */
  const char cmd;
  const i16* const args;

  PathCmd(char cmd, const i16* args) noexcept;

  no_copy_assign(PathCmd);

  inline bool isValid() const { return cmd != ' '; }

  inline i16 operator[](int i) const { return args[i]; }

  std::string toString() const;

  ~PathCmd();
};

/** A group of PathCmd */
struct Path {
private:
  const i32 _id;
  const u16 _cmdCnt;
  PathCmd** _cmds;

public:
  no_copy_assign(Path);

  Path(i32 id, u16 cmdCnt, PathCmd** cmds) noexcept;

  inline bool isEmpty() const { return _cmdCnt <= 0; }

  void draw(Graphics2D& g2) const;

  std::string toString() const;

  ~Path();

  static Path empty;
};

}  // namespace microtex

#endif  // HAVE_GLYPH_RENDER_PATH
#endif  // MICROTEX_PATH_H
