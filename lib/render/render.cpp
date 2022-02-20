#include "render.h"

#include "atom/atom.h"
#include "core/split.h"
#include "core/debug_config.h"
#include "env/env.h"
#include "box/box_single.h"

using namespace std;
using namespace tinytex;

Render::Render(const sptr<Box>& box, float textSize) {
  _box = box;
  _textSize = textSize;
  _fixedScale = _textSize / Env::fixedTextSize();
  const auto& debugConfig = DebugConfig::INSTANCE;
  if (debugConfig.enable) {
    const auto group = wrap(box);
    _box = group;
    BoxFilter filter = [&](const sptr<Box>& b) {
      return (
        debugConfig.showOnlyChar
        ? dynamic_cast<CharBox*>(b.get()) != nullptr
        : !b->isSpace()
      );
    };
    buildDebug(nullptr, group, filter);
  }
}

sptr<BoxGroup> Render::wrap(const sptr<Box>& box) {
  sptr<BoxGroup> parent;
  if (auto group = dynamic_pointer_cast<BoxGroup>(box); group != nullptr) {
    parent = group;
  } else {
    parent = sptrOf<HBox>(box);
  }
  return parent;
}

void Render::buildDebug(
  const sptr<BoxGroup>& parent,
  const sptr<Box>& box,
  const BoxFilter& filter
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
      buildDebug(group, child, filter);
    }
  } else if (auto decor = dynamic_pointer_cast<DecorBox>(box); decor != nullptr) {
    const auto g = wrap(decor->_base);
    decor->_base = g;
    buildDebug(nullptr, g, filter);
  }
}

float Render::getTextSize() const {
  return _textSize;
}

int Render::getHeight() const {
  return (int) ((_box->_height + _box->_depth) * _fixedScale);
}

int Render::getDepth() const {
  return (int) (_box->_depth * _fixedScale);
}

int Render::getWidth() const {
  return (int) (_box->_width * _fixedScale);
}

float Render::getBaseline() const {
  return _box->_height / (_box->_height + _box->_depth);
}

void Render::setTextSize(float textSize) {
  _textSize = textSize;
  _fixedScale = _textSize / Env::fixedTextSize();
}

void Render::setForeground(color fg) {
  _fg = fg;
}

void Render::draw(Graphics2D& g2, int x, int y) {
  color old = g2.getColor();
  g2.setColor(isTransparent(_fg) ? DFT_COLOR : _fg);
  g2.translate(x, y);
  g2.scale(_fixedScale, _fixedScale);

  // draw formula box
  _box->draw(g2, 0, _box->_height);

  // restore
  g2.scale(1.f / _fixedScale, 1.f / _fixedScale);
  g2.translate(-x, -y);
  g2.setColor(old);
}
