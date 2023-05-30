#include "render.h"

#include <functional>

#include "atom/atom.h"
#include "box/box_single.h"
#include "core/debug_config.h"
#include "core/split.h"
#include "env/env.h"

using namespace std;
using namespace microtex;

namespace microtex {

using BoxFilter = std::function<bool(const sptr<Box>&)>;

struct RenderData {
  sptr<Box> root;
  float textSize;
  float fixedScale;
  color fg;
  bool isSplit;
};

static sptr<BoxGroup> wrap(const sptr<Box>& box) {
  sptr<BoxGroup> parent;
  if (auto group = dynamic_pointer_cast<BoxGroup>(box); group != nullptr) {
    parent = group;
  } else {
    parent = sptrOf<HBox>(box);
  }
  return parent;
}

static void
buildDebug(const sptr<BoxGroup>& parent, const sptr<Box>& box, const BoxFilter& filter) {
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
    const auto kern =
      sptrOf<StrutBox>(-group->_width, -group->_height, -group->_depth, -group->_shift);
    // snapshot of current children
    const auto children = group->descendants();
    group->addOnly(kern);
    for (const auto& child : children) {
      buildDebug(group, child, filter);
    }
  } else if (auto decor = dynamic_pointer_cast<DecorBox>(box); decor != nullptr) {
    const auto g = wrap(decor->_base);
    decor->_base = g;
    buildDebug(nullptr, g, filter);
  }
}

}  // namespace microtex

Render::Render(const sptr<Box>& box, float textSize, bool isSplit) {
  _data = new RenderData{box, textSize, textSize / Env::fixedTextSize(), black, isSplit};
  const auto& debugConfig = DebugConfig::INSTANCE;
  if (debugConfig.enable) {
    const auto group = microtex::wrap(box);
    _data->root = group;
    BoxFilter filter = [&](const sptr<Box>& b) {
      return (
        debugConfig.showOnlyChar ? dynamic_cast<CharBox*>(b.get()) != nullptr : !b->isSpace()
      );
    };
    microtex::buildDebug(nullptr, group, filter);
  }
}

Render::~Render() {
  delete _data;
}

float Render::getTextSize() const {
  return _data->textSize;
}

int Render::getHeight() const {
  auto box = _data->root;
  return (int)(box->vlen() * _data->fixedScale);
}

int Render::getDepth() const {
  return (int)(_data->root->_depth * _data->fixedScale);
}

int Render::getWidth() const {
  return (int)(_data->root->_width * _data->fixedScale);
}

float Render::getBaseline() const {
  auto box = _data->root;
  return box->_height / box->vlen();
}

bool Render::isSplit() const {
  return _data->isSplit;
}

void Render::setTextSize(float textSize) {
  _data->textSize = textSize;
  _data->fixedScale = textSize / Env::fixedTextSize();
}

void Render::setForeground(color fg) {
  _data->fg = fg;
}

void Render::draw(Graphics2D& g2, int x, int y) {
  color old = g2.getColor();
  auto fixedScale = _data->fixedScale;
  auto box = _data->root;

  g2.setColor(isTransparent(_data->fg) ? black : _data->fg);
  g2.translate(x, y);
  g2.scale(fixedScale, fixedScale);

  // draw formula box
  box->draw(g2, 0, box->_height);

  // restore
  g2.scale(1.f / fixedScale, 1.f / fixedScale);
  g2.translate(-x, -y);
  g2.setColor(old);
}
