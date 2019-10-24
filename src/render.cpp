#include "render.h"
#include "atom/atom.h"
#include "core/core.h"
#include "core/formula.h"

using namespace tex;

const color TeXRender::_defaultcolor = black;
float TeXRender::_defaultSize = -1;
float TeXRender::_magFactor = 0;

TeXRender::TeXRender(const sptr<Box> box, float textSize, bool trueValues) {
  _box = box;
  if (_defaultSize != -1) _textSize = _defaultSize;
  if (_magFactor != 0) {
    _textSize = textSize * abs(_magFactor);
  } else {
    _textSize = textSize;
  }
  if (!trueValues) _insets += (int)(0.18f * textSize);
}

float TeXRender::getTextSize() const {
  return _textSize;
}

int TeXRender::getHeight() const {
  return (int)(_box->_height * _textSize + 0.99f +
               _box->_depth * _textSize + 0.99f +
               _insets.top + _insets.bottom);
}

int TeXRender::getDepth() const {
  return (int)(_box->_depth * _textSize + 0.99f + _insets.bottom);
}

int TeXRender::getWidth() const {
  return (int)(_box->_width * _textSize + 0.99f + _insets.left + _insets.right);
}

float TeXRender::getBaseline() const {
  return ((_box->_height * _textSize + 0.99f + _insets.top) /
          ((_box->_height + _box->_depth) * _textSize + 0.99f + _insets.top + _insets.bottom));
}

void TeXRender::setTextSize(float textSize) {
  _textSize = textSize;
}

void TeXRender::setForeground(color fg) {
  _fg = fg;
}

Insets TeXRender::getInsets() {
  return _insets;
}

void TeXRender::setInsets(const Insets& insets, bool trueval) {
  _insets = insets;
  if (!trueval) _insets += (int)(0.18f * _textSize);
}

void TeXRender::setWidth(int width, int align) {
  float diff = width - getWidth();
  // FIXME
  // only care if new width larger than old
  if (diff > 0) {
    _box = sptr<Box>(new HorizontalBox(_box, (float)width, align));
  }
}

void TeXRender::setHeight(int height, int align) {
  float diff = height - getHeight();
  // FIXME
  // only care if new height larger than old
  if (diff > 0) {
    _box = sptr<Box>(new VerticalBox(_box, diff, align));
  }
}

void TeXRender::draw(_out_ Graphics2D& g2, int x, int y) {
  color old = g2.getColor();
  g2.scale(_textSize, _textSize);
  if (!istrans(_fg)) {
    g2.setColor(_fg);
  } else {
    g2.setColor(_defaultcolor);
  }

  // draw formula box
  _box->draw(g2, (x + _insets.left) / _textSize, (y + _insets.top) / _textSize + _box->_height);

  // restore
  g2.reset();
  g2.setColor(old);
}

DefaultTeXFont* TeXRenderBuilder::createFont(float size, int type) {
  DefaultTeXFont* tf = new DefaultTeXFont(size);
  if (type == 0) tf->setSs(false);
  if ((type & ROMAN) != 0) tf->setRoman(true);
  if ((type & TYPEWRITER) != 0) tf->setTt(true);
  if ((type & SANSSERIF) != 0) tf->setSs(true);
  if ((type & ITALIC) != 0) tf->setIt(true);
  if ((type & BOLD) != 0) tf->setBold(true);
  return tf;
}

TeXRender* TeXRenderBuilder::build(TeXFormula& f) {
  return build(f._root);
}

TeXRender* TeXRenderBuilder::build(const sptr<Atom>& fc) {
  sptr<Atom> f = fc;
  if (f == nullptr) f = sptr<Atom>(new EmptyAtom());
  if (_style == -1) {
    throw ex_invalid_state("A style is required, call function setStyle before build.");
  }
  if (_textSize == -1) {
    throw ex_invalid_state("A size is required, call function setSize before build.");
  }

  DefaultTeXFont* font = (_type == -1) ? new DefaultTeXFont(_textSize)
                                       : createFont(_textSize, _type);
  sptr<TeXFont> tf(font);
  TeXEnvironment* te = nullptr;
  if (_widthUnit != -1 && _textWidth != 0) {
    te = new TeXEnvironment(_style, tf, _widthUnit, _textWidth);
  } else {
    te = new TeXEnvironment(_style, tf);
  }

  if (_lineSpaceUnit != -1) te->setInterline(_lineSpaceUnit, _lineSpace);

  auto box = f->createBox(*te);
  TeXRender* ti = nullptr;
  if (_widthUnit != -1 && _textWidth != 0) {
    HorizontalBox* hb = nullptr;
    if (_lineSpaceUnit != -1 && _lineSpace != 0) {
      float il = _lineSpace * SpaceAtom::getFactor(_lineSpaceUnit, *te);
      auto b = BoxSplitter::split(box, te->getTextWidth(), il);
      hb = new HorizontalBox(b, _isMaxWidth ? b->_width : te->getTextWidth(), _align);
    } else {
      hb = new HorizontalBox(box, _isMaxWidth ? box->_width : te->getTextWidth(), _align);
    }
    ti = new TeXRender(sptr<Box>(hb), _textSize, _trueValues);
  } else {
    ti = new TeXRender(box, _textSize, _trueValues);
  }

  if (!istrans(_fg)) ti->setForeground(_fg);
  ti->_iscolored = te->_isColored;

  delete te;
  return ti;
}
