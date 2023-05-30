#ifndef MICROTEX_SPLIT_H
#define MICROTEX_SPLIT_H

#include <stack>

#include "box/box.h"
#include "box/box_group.h"
#include "core/glue.h"

namespace microtex {

#ifdef HAVE_LOG

void printBox(const sptr<Box>& box);

#endif  // HAVE_LOG

class BoxSplitter {
public:
  struct Position {
    int _index;
    sptr<HBox> _box;

    Position(int index, const sptr<HBox>& box) : _index(index), _box(box) {}
  };

private:
  static float canBreak(std::stack<Position>& stack, const sptr<HBox>& hbox, float width);

  static int getBreakPosition(const sptr<HBox>& hb, int index);

  static std::pair<bool, sptr<Box>> split(const sptr<HBox>& hb, float width, float lineSpace);

public:
  static std::pair<bool, sptr<Box>> split(const sptr<Box>& box, float width, float lineSpace);
};

}  // namespace microtex

#endif  // MICROTEX_SPLIT_H
