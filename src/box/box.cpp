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

void Box::draw(Graphics2D& g2, float x, float y) {
  onDraw(g2, x, y);
  // if (DEBUG) drawDebug(g2, x, y);
}

void Box::drawDebug(Graphics2D& g2, float x, float y) {
  const color prevColor = g2.getColor();
  const Stroke& prevStroke = g2.getStroke();
  g2.setColor(red);
  g2.setStrokeWidth(std::abs(1.f / g2.sx()));
  g2.drawRect(x, y - _height, _width, _height + _depth);
  g2.setColor(prevColor);
  g2.setStroke(prevStroke);
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
