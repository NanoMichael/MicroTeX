#include "box/box.h"

#include "unimath/uni_font.h"

using namespace microtex;
using namespace std;

void Box::copyMetrics(const sptr<Box>& box) {
  _width = box->_width;
  _height = box->_height;
  _depth = box->_depth;
  _shift = box->_shift;
}

int Box::lastFontId() {
  return FontContext::NO_FONT;
}

void Box::replaceFirst(const sptr<Box>& from, const sptr<Box>& to) {
  // nothing to do
}

std::string Box::name() const {
  return "Box";
}

std::string Box::toString() const {
  return "";
}

void BoxGroup::add(const sptr<Box>& box) {
  _children.push_back(box);
}

void BoxGroup::add(int pos, const sptr<Box>& box) {
  _children.insert(_children.begin() + pos, box);
}

int BoxGroup::lastFontId() {
  int id = FontContext::NO_FONT;
  for (int i = _children.size() - 1; i >= 0 && id == FontContext::NO_FONT; i--) {
    id = _children[i]->lastFontId();
  }
  return id;
}

int DecorBox::lastFontId() {
  return _base->lastFontId();
}
