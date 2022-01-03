#ifndef LATEX_PATH_H
#define LATEX_PATH_H

#include <string>
#include "utils/utils.h"

namespace tex {

class Graphics2D;

/** Abstract graphical path command, with its drawing command and points. */
struct PathCmd {
  /**
   * Path drawing command, one of the following:
   * @code
   * cmd  args      desc
   * M    x, y      move to point (x, y)
   * m    dx, dy    move to point (dx, dy)
   * @endcode
   */
  const char cmd;
  const i16* const args;

  PathCmd(char cmd, const i16* args) noexcept;

  no_copy_assign(PathCmd);

  inline bool isValid() const { return cmd != ' '; }

  inline i16 operator[](int i) const { return args[i]; }

  std::string toString() const;

  ~PathCmd();

  static u16 argsCount(char cmd);
};

struct Path {
private:
  const u16 _cmdCnt;
  PathCmd** _cmds;

public:
  no_copy_assign(Path);

  Path(u16 cmdCnt, PathCmd** cmds) noexcept;

  inline u16 cmdCnt() const { return _cmdCnt; }

  inline bool isEmpty() const { return _cmdCnt <= 0; }

  void draw(Graphics2D& g2) const;

  std::string toString() const;

  ~Path();

  static Path empty;
};

}

#endif //LATEX_PATH_H
