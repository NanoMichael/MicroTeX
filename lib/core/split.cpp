#include "core/split.h"

#include "box/box_group.h"
#include "utils/log.h"

using namespace std;
using namespace microtex;

#ifdef HAVE_LOG

static void printBox(const sptr<Box>& b, int dep, vector<bool>& lines, int max = 0) {
  logv("%-4d", dep);
  if (lines.size() < dep + 1) lines.resize(dep + 1, false);

  for (int i = 0; i < dep - 1; i++) {
    logv(lines[i] ? "    " : " │  ");
  }

  if (dep > 0) {
    logv(lines[dep - 1] ? " └──" : " ├──");
  }

  if (b == nullptr) {
    logv(ANSI_COLOR_RED " NULL\n" ANSI_RESET);
    return;
  }

  const vector<sptr<Box>>& children = b->descendants();
  const auto size = children.size();
  const auto& name = b->name();
  const char* fmt = (size > 0 ? ANSI_COLOR_CYAN " %-*s " ANSI_RESET : " %-*s ");
  logv(fmt, size > 0 ? name.size() : max, name.c_str());
  // show metrics and additional info
  logv(
    "[%g, (%g + %g) = %g, %g] %s\n",
    b->_width,
    b->_height,
    b->_depth,
    b->vlen(),
    b->_shift,
    b->toString().c_str()
  );
  if (size == 0) return;

  size_t limit = 0;
  for (const auto& x : children) {
    limit = std::max(limit, x->name().size());
  }

  for (size_t i = 0; i < size; i++) {
    lines[dep] = i == size - 1;
    printBox(children[i], dep + 1, lines, limit);
  }
}

void microtex::printBox(const sptr<Box>& box) {
  vector<bool> lines;
  ::printBox(box, 0, lines, box->name().size());
  logv("\n");
}

#endif  // HAVE_LOG

std::pair<bool, sptr<Box>> BoxSplitter::split(const sptr<Box>& b, float width, float lineSpace) {
  auto h = dynamic_pointer_cast<HBox>(b);
  sptr<Box> box;
  if (h != nullptr) {
    auto [splitted, box] = split(h, width, lineSpace);
#ifdef HAVE_LOG
    if (box != b) {
      logv("[BEFORE SPLIT]:\n");
      printBox(b);
      logv("[AFTER SPLIT]:\n");
      printBox(box);
    } else {
      logv("[BOX TREE]:\n");
      printBox(box);
    }
#endif
    return {splitted, box};
  }
#ifdef HAVE_LOG
  logv("[BOX TREE]:\n");
  printBox(b);
#endif
  return {false, b};
}

std::pair<bool, sptr<Box>> BoxSplitter::split(const sptr<HBox>& hb, float width, float lineSpace) {
  if (width == 0 || hb->_width <= width) return {false, hb};

  auto* vbox = new VBox();
  sptr<HBox> first, second;
  stack<Position> positions;
  sptr<HBox> hbox = hb;
  bool splitted = false;

  while (hbox->_width > width && canBreak(positions, hbox, width) != hbox->_width) {
    Position pos = positions.top();
    positions.pop();
    auto hboxes = pos._box->split(pos._index - 1);
    first = hboxes.first;
    second = hboxes.second;
    while (!positions.empty()) {
      pos = positions.top();
      positions.pop();
      hboxes = pos._box->splitRemove(pos._index);
      hboxes.first->add(first);
      hboxes.second->add(0, second);
      first = hboxes.first;
      second = hboxes.second;
    }
    vbox->add(first, lineSpace);
    splitted = true;
    hbox = second;
  }

  if (second != nullptr) {
    vbox->add(second, lineSpace);
    return {splitted, sptr<Box>(vbox)};
  }

  return {splitted, hbox};
}

float BoxSplitter::canBreak(stack<Position>& s, const sptr<HBox>& hbox, const float width) {
  const vector<sptr<Box>>& children = hbox->_children;
  const int count = children.size();
  // Cumulative width
  auto* cumWidth = new float[count + 1]();
  cumWidth[0] = 0;
  for (int i = 0; i < count; i++) {
    auto box = children[i];
    cumWidth[i + 1] = cumWidth[i] + box->_width;
    if (cumWidth[i + 1] <= width) continue;
    int pos = getBreakPosition(hbox, i);
    auto h = dynamic_pointer_cast<HBox>(box);
    if (h != nullptr) {
      stack<Position> sub;
      float w = canBreak(sub, h, width - cumWidth[i]);
      if (w != box->_width && (cumWidth[i] + w <= width || pos == -1)) {
        s.push(Position(i - 1, hbox));
        // add to stack
        vector<Position> p;
        while (!sub.empty()) {
          p.push_back(sub.top());
          sub.pop();
        }
        for (auto it = p.rbegin(); it != p.rend(); it++) s.push(*it);
        // release cum-width
        float x = cumWidth[i] + w;
        delete[] cumWidth;
        return x;
      }
    }

    if (pos != -1) {
      s.push(Position(pos, hbox));
      float x = cumWidth[pos];
      delete[] cumWidth;
      return x;
    }
  }

  delete[] cumWidth;
  return hbox->_width;
}

int BoxSplitter::getBreakPosition(const sptr<HBox>& hb, int i) {
  if (hb->_breakPositions.empty()) return -1;

  if (hb->_breakPositions.size() == 1 && hb->_breakPositions[0] <= i) {
    return hb->_breakPositions[0];
  }

  size_t pos = 0;
  for (; pos < hb->_breakPositions.size(); pos++) {
    if (hb->_breakPositions[pos] > i) {
      if (pos == 0) return -1;
      return hb->_breakPositions[pos - 1];
    }
  }

  return hb->_breakPositions[pos - 1];
}
