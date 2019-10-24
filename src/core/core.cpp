#include "core/core.h"
#include "atom/atom_basic.h"
#include "atom/box.h"
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

  vector<sptr<Box>> children = b->getChildren();
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
  auto h = dynamic_pointer_cast<HorizontalBox>(b);
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

sptr<Box> BoxSplitter::split(const sptr<HorizontalBox>& hb, float width, float lineSpace) {
  if (width == 0 || hb->_width <= width) return hb;

  VerticalBox* vbox = new VerticalBox();
  sptr<HorizontalBox> first, second;
  stack<Position> positions;
  sptr<HorizontalBox> hbox = hb;

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

float BoxSplitter::canBreak(stack<Position>& s, const sptr<HorizontalBox>& hbox, const float width) {
  const vector<sptr<Box>>& children = hbox->_children;
  const int count = children.size();
  // Cumulative width
  float* cumWidth = new float[count + 1]();
  cumWidth[0] = 0;
  for (int i = 0; i < count; i++) {
    auto box = children[i];
    cumWidth[i + 1] = cumWidth[i] + box->_width;
    if (cumWidth[i + 1] <= width) continue;
    int pos = getBreakPosition(hbox, i);
    auto h = dynamic_pointer_cast<HorizontalBox>(box);
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

int BoxSplitter::getBreakPosition(const sptr<HorizontalBox>& hb, int i) {
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

/************************************* TeXEnvironment implementation ******************************/

TeXEnvironment::TeXEnvironment(int style, const sptr<TeXFont>& tf, int wu, float tw) {
  init();
  _style = style;
  _tf = tf;
  setInterline(TeXConstants::UNIT_EX, 1.f);
  _textWidth = tw * SpaceAtom::getFactor(wu, *this);
}

float TeXEnvironment::getInterline() const {
  return _interline * SpaceAtom::getFactor(_interlineUnit, *this);
}

void TeXEnvironment::setTextWidth(int wu, float w) {
  _textWidth = w * SpaceAtom::getFactor(wu, *this);
}

sptr<TeXEnvironment>& TeXEnvironment::copy() {
  TeXEnvironment* t = new TeXEnvironment(
      _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
  _copy = sptr<TeXEnvironment>(t);
  return _copy;
}

sptr<TeXEnvironment>& TeXEnvironment::copy(const sptr<TeXFont>& tf) {
  TeXEnvironment* te = new TeXEnvironment(
      _style, _scaleFactor, tf, _background, _color, _textStyle, _smallCap);
  te->_textWidth = _textWidth;
  te->_interline = _interline;
  te->_interlineUnit = _interlineUnit;
  _copytf = sptr<TeXEnvironment>(te);
  return _copytf;
}

sptr<TeXEnvironment>& TeXEnvironment::crampStyle() {
  TeXEnvironment* t = new TeXEnvironment(
      _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
  _cramp = sptr<TeXEnvironment>(t);
  _cramp->_style = (_style % 2 == 1 ? _style : _style + 1);
  return _cramp;
}

sptr<TeXEnvironment>& TeXEnvironment::dnomStyle() {
  TeXEnvironment* t = new TeXEnvironment(
      _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
  _dnom = sptr<TeXEnvironment>(t);
  _dnom->_style = 2 * (_style / 2) + 1 + 2 - 2 * (_style / 6);
  return _dnom;
}

sptr<TeXEnvironment>& TeXEnvironment::numStyle() {
  TeXEnvironment* t = new TeXEnvironment(
      _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
  _num = sptr<TeXEnvironment>(t);
  _num->_style = _style + 2 - 2 * (_style / 6);
  return _num;
}

sptr<TeXEnvironment>& TeXEnvironment::rootStyle() {
  TeXEnvironment* t = new TeXEnvironment(
      _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
  _root = sptr<TeXEnvironment>(t);
  _root->_style = STYLE_SCRIPT_SCRIPT;
  return _root;
}

sptr<TeXEnvironment>& TeXEnvironment::subStyle() {
  TeXEnvironment* t = new TeXEnvironment(
      _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
  _sub = sptr<TeXEnvironment>(t);
  _sub->_style = 2 * (_style / 4) + 4 + 1;
  return _sub;
}

sptr<TeXEnvironment>& TeXEnvironment::supStyle() {
  TeXEnvironment* t = new TeXEnvironment(
      _style, _scaleFactor, _tf, _background, _color, _textStyle, _smallCap);
  _sup = sptr<TeXEnvironment>(t);
  _sup->_style = 2 * (_style / 4) + 4 + (_style % 2);
  return _sup;
}

/************************************* Glue implementation ****************************************/

#ifdef HAVE_LOG
ostream& tex::operator<<(ostream& out, const Glue& glue) {
  out << "Glue { space: " << glue._space << ", stretch: " << glue._stretch << ", shrink: ";
  out << glue._shrink << ", name: " << glue._name << " }";
  return out;
}
#endif  // HAVE_LOG

void Glue::_init_() {
#ifdef HAVE_LOG
  // print glue types
  __log << "elements in _glueTypes" << endl;
  for (auto x : _glueTypes) __log << "\t" << x << endl;
  __log << endl;
#endif  // HAVE_LOG
}

void Glue::_free_() {
  // delete glue-types
  for (size_t i = 0; i < _glueTypes.size(); i++) {
    Glue* g = _glueTypes[i];
    delete g;
    _glueTypes[i] = nullptr;
  }
}

float Glue::getFactor(const TeXEnvironment& env) const {
  auto tf = env.getTeXFont();
  // use "quad" from a font marked as an "mu font"
  float quad = tf->getQuad(env.getStyle(), tf->getMuFontId());
  return quad / 18.f;
}

sptr<Box> Glue::createBox(const TeXEnvironment& env) const {
  float factor = getFactor(env);
  auto x = new GlueBox(_space * factor, _stretch * factor, _shrink * factor);
  return sptr<Box>(x);
}

int Glue::getGlueIndex(int ltype, int rtype, const TeXEnvironment& env) {
  // types > INNER are considered of type ORD for glue calculations
  int l = (ltype > TYPE_INNER ? TYPE_ORDINARY : ltype);
  int r = (rtype > TYPE_INNER ? TYPE_ORDINARY : rtype);
  return _table[l][r][env.getStyle() / 2] - '0';
}

sptr<Box> Glue::get(int ltype, int rtype, const TeXEnvironment& env) {
  int i = getGlueIndex(ltype, rtype, env);
  return _glueTypes[i]->createBox(env);
}

Glue* Glue::getGlue(int skipType) {
  int st = skipType < 0 ? -skipType : skipType;
  string name;
  switch (st) {
    case THINMUSKIP:
      name = "thin";
      break;
    case MEDMUSKIP:
      name = "med";
      break;
    default:
      name = "thick";
      break;
  }
  auto it = find_if(_glueTypes.begin(), _glueTypes.end(), [&name](const Glue* g) {
    return g->_name == name;
  });
  return *it;
}

sptr<Box> Glue::get(int skipType, const TeXEnvironment& env) {
  auto glue = getGlue(skipType);
  if (glue == nullptr) return sptr<Box>(new GlueBox(0, 0, 0));
  auto b = glue->createBox(env);
  if (skipType < 0) b->negWidth();
  return b;
}

float Glue::getSpace(int ltype, int rtype, const TeXEnvironment& env) {
  int i = getGlueIndex(ltype, rtype, env);
  auto glueType = _glueTypes[i];
  return glueType->_space * glueType->getFactor(env);
}

float Glue::getSpace(int skipType, const TeXEnvironment& env) {
  auto glue = getGlue(skipType);
  if (glue == nullptr) return 0;
  return glue->_space * glue->getFactor(env);
}
