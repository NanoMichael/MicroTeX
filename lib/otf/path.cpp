#include "otf/path.h"

microtex::u16 microtex::pathCmdArgsCount(char cmd) {
  switch (cmd) {
    case 'M':
    case 'm':
    case 'L':
    case 'l':
    case 'T':
    case 't': return 2;

    case 'H':
    case 'h':
    case 'V':
    case 'v': return 1;

    case 'Z':
    case 'z': return 0;

    case 'C':
    case 'c': return 6;

    case 'S':
    case 's':
    case 'Q':
    case 'q': return 4;

    default: return 0;
  }
}

#ifdef HAVE_GLYPH_RENDER_PATH

#include "graphic/graphic.h"
#include "utils/string_utils.h"

namespace microtex {

PathCmd::PathCmd(char cmd, const i16* args) noexcept : cmd(cmd), args(args) {}

std::string PathCmd::toString() const {
  std::string str;
  str.append(1, cmd).append(1, ' ');
  for (int i = 0; i < pathCmdArgsCount(cmd); i++) {
    str.append(microtex::toString(args[i])).append(1, ' ');
  }
  str.append(1, '\n');
  return str;
}

PathCmd::~PathCmd() {
  delete[] args;
}

Path Path::empty(-1, 0, nullptr);

Path::Path(i32 id, u16 cmdCnt, PathCmd** cmds) noexcept : _id(id), _cmdCnt(cmdCnt), _cmds(cmds) {}

void Path::draw(Graphics2D& g2) const {
  if (isEmpty()) return;
  const auto cached = g2.beginPath(_id);
  if (cached) {
    g2.fillPath(_id);
    return;
  }
  int mx = 0, my = 0;  // the first point to move to
  int px = 0, py = 0;  // the current point
  int cx = 0, cy = 0;  // the last control point
  for (u16 i = 0; i < _cmdCnt; i++) {
    const auto& cmd = *_cmds[i];
    if (!cmd.isValid()) continue;
    switch (cmd.cmd) {
      case 'M':
        mx = px = cmd[0];
        my = py = cmd[1];
        g2.moveTo(px, py);
        break;
      case 'm':
        mx = px = cmd[0] + px;
        my = py = cmd[1] + py;
        g2.moveTo(px, py);
        break;
      case 'L':
        px = cmd[0];
        py = cmd[1];
        g2.lineTo(px, py);
        break;
      case 'l':
        px += cmd[0];
        py += cmd[1];
        g2.lineTo(px, py);
        break;
      case 'H':
        px = cmd[0];
        g2.lineTo(px, py);
        break;
      case 'h':
        px += cmd[0];
        g2.lineTo(px, py);
        break;
      case 'V':
        py = cmd[0];
        g2.lineTo(px, py);
        break;
      case 'v':
        py += cmd[0];
        g2.lineTo(px, py);
        break;
      case 'C':
        cx = cmd[2];
        cy = cmd[3];
        px = cmd[4];
        py = cmd[5];
        g2.cubicTo(cmd[0], cmd[1], cx, cy, px, py);
        break;
      case 'c':
        g2.cubicTo(px + cmd[0], py + cmd[1], px + cmd[2], py + cmd[3], px + cmd[4], py + cmd[5]);
        cx = px + cmd[2];
        cy = py + cmd[3];
        px += cmd[4];
        py += cmd[5];
        break;
      case 'S':
        g2.cubicTo(px + px - cx, py + py - cy, cmd[0], cmd[1], cmd[2], cmd[3]);
        cx = cmd[0];
        cy = cmd[1];
        px = cmd[2];
        py = cmd[3];
        break;
      case 's':
        g2.cubicTo(px + px - cx, py + py - cy, px + cmd[0], py + cmd[1], px + cmd[2], py + cmd[3]);
        cx = cmd[0] + px;
        cy = cmd[1] + py;
        px += cmd[2];
        py += cmd[3];
        break;
      case 'Q':
        cx = cmd[0];
        cy = cmd[1];
        px = cmd[2];
        py = cmd[3];
        g2.quadTo(cx, cy, px, py);
        break;
      case 'q':
        cx = cmd[0] + px;
        cy = cmd[1] + py;
        px += cmd[2];
        py += cmd[3];
        g2.quadTo(cx, cy, px, py);
        break;
      case 'T':
        cx = px + px - cx;
        cy = py + py - cy;
        px = cmd[0];
        py = cmd[1];
        g2.quadTo(cx, cy, px, py);
        break;
      case 't':
        cx = px + px - cx;
        cy = py + py - cy;
        px += cmd[0];
        py += cmd[1];
        g2.quadTo(cx, cy, px, py);
        break;
      case 'Z':
      case 'z': g2.closePath(); break;
    }
  }
  g2.fillPath(_id);
}

std::string Path::toString() const {
  std::string str;
  for (int i = 0; i < _cmdCnt; i++) {
    str.append(_cmds[i]->toString());
  }
  return str;
}

Path::~Path() {
  for (u16 i = 0; i < _cmdCnt; i++) delete _cmds[i];
  delete[] _cmds;
}

}  // namespace microtex

#endif  // HAVE_GLYPH_RENDER_PATH
