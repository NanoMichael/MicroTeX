#include "render.h"

#include "atom/atom.h"
#include "core/core.h"
#include "core/formula.h"

using namespace std;
using namespace tex;

const color TeXRender::_defaultcolor = black;
float TeXRender::_defaultSize = -1;
float TeXRender::_magFactor = 0;

TeXRender::TeXRender(const sptr<Box>& box, float textSize, bool trueValues) {
  _box = box;
  if (_defaultSize != -1) _textSize = _defaultSize;
  if (_magFactor != 0) {
    _textSize = textSize * std::abs(_magFactor);
  } else {
    _textSize = textSize;
  }
  if (!trueValues) _insets += (int) (0.18f * textSize);
  if (Box::DEBUG) {
    const auto group = wrap(box);
    _box = group;
    BoxFilter filter = [](auto b) {
      return dynamic_cast<CharBox*>(b.get()) != nullptr;
    };
    buildDebug(nullptr, group, std::move(filter));
  }
}

sptr<BoxGroup> TeXRender::wrap(const sptr<Box>& box) {
  sptr<BoxGroup> parent;
  if (auto group = dynamic_pointer_cast<BoxGroup>(box); group != nullptr) {
    parent = group;
  } else {
    parent = sptrOf<HBox>(box);
  }
  return parent;
}

void TeXRender::buildDebug(
  const sptr<BoxGroup>& parent,
  const sptr<Box>& box,
  BoxFilter&& filter
) {
  if (parent != nullptr) {
    if (box->isSpace()) {
      parent->addOnly(box);
    } else if (filter(box)) {
      parent->addOnly(sptrOf<DebugBox>(box));
    } else {
      // placeholder to consume the space of the current box
      parent->addOnly(sptrOf<StrutBox>(box));
    }
  }
  if (auto group = dynamic_pointer_cast<BoxGroup>(box); group != nullptr) {
    const auto kern = sptrOf<StrutBox>(-group->_width, -group->_height, -group->_depth, -group->_shift);
    // snapshot of current children
    const auto children = group->descendants();
    group->addOnly(kern);
    for (const auto& child: children) {
      buildDebug(group, child, std::forward<BoxFilter>(filter));
    }
  } else if (auto decor = dynamic_pointer_cast<DecorBox>(box); decor != nullptr) {
    const auto g = wrap(decor->_base);
    decor->_base = g;
    buildDebug(nullptr, g, std::forward<BoxFilter>(filter));
  }
}

inline float TeXRender::getTextSize() const {
  return _textSize;
}

int TeXRender::getHeight() const {
  return (int) (
    _box->_height * _textSize +
    _box->_depth * _textSize +
    _insets.top + _insets.bottom
  );
}

int TeXRender::getDepth() const {
  return (int) (_box->_depth * _textSize + _insets.bottom);
}

int TeXRender::getWidth() const {
  return (int) (_box->_width * _textSize + _insets.left + _insets.right);
}

float TeXRender::getBaseline() const {
  return (
    (_box->_height * _textSize + _insets.top) /
    ((_box->_height + _box->_depth) * _textSize + _insets.top + _insets.bottom)
  );
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
  if (!trueval) _insets += (int) (0.18f * _textSize);
}

void TeXRender::setWidth(int width, Alignment align) {
  float diff = width - getWidth();
  // FIXME
  // only care if new width larger than old
  if (diff > 0) {
    _box = sptrOf<HBox>(_box, (float) width, align);
  }
}

void TeXRender::setHeight(int height, Alignment align) {
  float diff = height - getHeight();
  // FIXME
  // only care if new height larger than old
  if (diff > 0) {
    _box = sptrOf<VBox>(_box, diff, align);
  }
}

void TeXRender::draw(Graphics2D& g2, int x, int y) {
  color old = g2.getColor();
  g2.scale(_textSize, _textSize);
  if (!isTransparent(_fg)) {
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

TeXRender* TeXRenderBuilder::build(Formula& f) {
  return build(f._root);
}

TeXRender* TeXRenderBuilder::build(const sptr<Atom>& fc) {
  sptr<Atom> f = fc;
  if (f == nullptr) f = sptrOf<EmptyAtom>();
  if (_textSize == -1) {
    throw ex_invalid_state("A size is required, call function setSize before build.");
  }
  return nullptr;
}
