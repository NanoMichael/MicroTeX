#include "box/box.h"
#include "fonts/fonts.h"

using namespace tex;
using namespace std;

bool Box::DEBUG = false;

void Box::copyMetrics(const sptr<Box>& box) {
  _width = box->_width;
  _height = box->_height;
  _depth = box->_depth;
  _shift = box->_shift;
}

int Box::lastFontId() {
  return TeXFont::NO_FONT;
}

int BoxGroup::lastFontId() {
  int id = TeXFont::NO_FONT;
  for (int i = _children.size() - 1; i >= 0 && id == TeXFont::NO_FONT; i--) {
    id = _children[i]->lastFontId();
  }
  return id;
}

int DecorBox::lastFontId() {
  return _base->lastFontId();
}
