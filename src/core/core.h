#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include <cstring>
#include <stack>

#include "common.h"
#include "core/glue.h"
#include "box/box.h"
#include "box/box_group.h"

namespace tex {

#ifdef HAVE_LOG

void print_box(const sptr<Box>& box);

#endif  // HAVE_LOG

class BoxSplitter {
public:
  struct Position {
    int _index;
    sptr<HBox> _box;

    Position(int index, const sptr<HBox>& box)
      : _index(index), _box(box) {}
  };

private:
  static float canBreak(std::stack<Position>& stack, const sptr<HBox>& hbox, float width);

  static int getBreakPosition(const sptr<HBox>& hb, int index);

public:
  static sptr<Box> split(const sptr<Box>& box, float width, float lineSpace);

  static sptr<Box> split(const sptr<HBox>& hb, float width, float lineSpace);
};

}  // namespace tex

#endif  // CORE_H_INCLUDED
