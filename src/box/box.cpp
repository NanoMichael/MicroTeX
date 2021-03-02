#include "box/box.h"
#include "fonts/fonts.h"

using namespace tex;
using namespace std;

bool Box::DEBUG = false;

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
