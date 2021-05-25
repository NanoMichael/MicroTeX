#include "core/core.h"

#include "atom/atom_basic.h"
#include "box/box_group.h"
#include "common.h"

using namespace std;
using namespace tex;

#ifdef HAVE_LOG

void print_box(const sptr<Box>& b, int dep, vector<bool>& lines) {
  __print("%-4d", dep);
  if (lines.size() < dep + 1) lines.resize(dep + 1, false);

  for (int i = 0; i < dep - 1; i++) {
    if (lines[i]) {
      __print("    ");
    } else {
      __print(" │  ");
    }
  }

  if (dep > 0) {
    if (lines[dep - 1]) {
      __print(" └──");
    } else {
      __print(" ├──");
    }
  }

  if (b == nullptr) {
    __print(ANSI_COLOR_RED " NULL\n");
    return;
  }

  const vector<sptr<Box>>& children = b->descendants();
  const size_t c = children.size();
  const string& str = demangle_name(typeid(*(b)).name());
  string name = str.substr(str.find_last_of("::") + 1);
  if (c > 0) {
    __print(ANSI_COLOR_CYAN " %s\n" ANSI_RESET, name.c_str());
  } else {
    __print(" %s\n", name.c_str());
  }

  for (size_t i = 0; i < c; i++) {
    lines[dep] = i == c - 1;
    print_box(children[i], dep + 1, lines);
  }
}

void tex::print_box(const sptr<Box>& b) {
  vector<bool> lines;
  ::print_box(b, 0, lines);
  __print("\n");
}

#endif  // HAVE_LOG

sptr<Box> BoxSplitter::split(const sptr<Box>& b, float width, float lineSpace) {
  auto h = dynamic_pointer_cast<HBox>(b);
  sptr<Box> box;
  if (h != nullptr) {
    auto box = split(h, width, lineSpace);
#ifdef HAVE_LOG
    if (box != b) {
      __print("[BEFORE SPLIT]:\n");
      print_box(b);
      __print("[AFTER SPLIT]:\n");
      print_box(box);
    } else {
      __print("[BOX TREE]:\n");
      print_box(box);
    }
#endif
    return box;
  }
#ifdef HAVE_LOG
  __print("[BOX TREE]:\n");
  print_box(b);
#endif
  return b;
}

sptr<Box> BoxSplitter::split(const sptr<HBox>& hb, float width, float lineSpace) {
  if (width == 0 || hb->_width <= width) return hb;

  auto* vbox = new VBox();
  sptr<HBox> first, second;
  stack<Position> positions;
  sptr<HBox> hbox = hb;

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
    hbox = second;
  }

  if (second != nullptr) {
    vbox->add(second, lineSpace);
    return sptr<Box>(vbox);
  }

  return hbox;
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

  if (hb->_breakPositions.size() == 1 && hb->_breakPositions[0] <= i)
    return hb->_breakPositions[0];

  size_t pos = 0;
  for (; pos < hb->_breakPositions.size(); pos++) {
    if (hb->_breakPositions[pos] > i) {
      if (pos == 0) return -1;
      return hb->_breakPositions[pos - 1];
    }
  }

  return hb->_breakPositions[pos - 1];
}

/************************************* Environment implementation ******************************/

Environment::Environment(TexStyle style, const sptr<TeXFont>& tf, UnitType wu, float tw) {
  init();
  _style = style;
  _tf = tf;
  setInterline(UnitType::ex, 1.f);
  _textWidth = tw * SpaceAtom::getFactor(wu, *this);
}

float Environment::getInterline() const {
  return _interline * SpaceAtom::getFactor(_interlineUnit, *this);
}

void Environment::setTextWidth(UnitType wu, float w) {
  _textWidth = w * SpaceAtom::getFactor(wu, *this);
}

sptr<Environment>& Environment::copy() {
  Environment* t = new Environment(_style, _scaleFactor, _tf, _textStyle, _smallCap);
  _copy = sptr<Environment>(t);
  return _copy;
}

sptr<Environment>& Environment::copy(const sptr<TeXFont>& tf) {
  Environment* te = new Environment(_style, _scaleFactor, tf, _textStyle, _smallCap);
  te->_textWidth = _textWidth;
  te->_interline = _interline;
  te->_interlineUnit = _interlineUnit;
  _copytf = sptr<Environment>(te);
  return _copytf;
}

sptr<Environment>& Environment::crampStyle() {
  Environment* t = new Environment(_style, _scaleFactor, _tf, _textStyle, _smallCap);
  _cramp = sptr<Environment>(t);
  const i8 style = static_cast<i8>(_style);
  _cramp->_style = static_cast<TexStyle>(style % 2 == 1 ? style : style + 1);
  return _cramp;
}

sptr<Environment>& Environment::dnomStyle() {
  Environment* t = new Environment(_style, _scaleFactor, _tf, _textStyle, _smallCap);
  _dnom = sptr<Environment>(t);
  const i8 style = static_cast<i8>(_style);
  _dnom->_style = static_cast<TexStyle>(2 * (style / 2) + 1 + 2 - 2 * (style / 6));
  return _dnom;
}

sptr<Environment>& Environment::numStyle() {
  Environment* t = new Environment(_style, _scaleFactor, _tf, _textStyle, _smallCap);
  _num = sptr<Environment>(t);
  const i8 style = static_cast<i8>(_style);
  _num->_style = static_cast<TexStyle>(style + 2 - 2 * (style / 6));
  return _num;
}

sptr<Environment>& Environment::rootStyle() {
  Environment* t = new Environment(_style, _scaleFactor, _tf, _textStyle, _smallCap);
  _root = sptr<Environment>(t);
  _root->_style = TexStyle::scriptScript;
  return _root;
}

sptr<Environment>& Environment::subStyle() {
  Environment* t = new Environment(_style, _scaleFactor, _tf, _textStyle, _smallCap);
  _sub = sptr<Environment>(t);
  const i8 style = static_cast<i8>(_style);
  _sub->_style = static_cast<TexStyle>(2 * (style / 4) + 4 + 1);
  return _sub;
}

sptr<Environment>& Environment::supStyle() {
  Environment* t = new Environment(_style, _scaleFactor, _tf, _textStyle, _smallCap);
  _sup = sptr<Environment>(t);
  const i8 style = static_cast<i8>(_style);
  _sup->_style = static_cast<TexStyle>(2 * (style / 4) + 4 + (style % 2));
  return _sup;
}
