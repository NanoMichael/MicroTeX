#include "render.h"

#include "atom/atom.h"
#include "core/split.h"
#include "core/debug_config.h"
#include "env/env.h"
#include "box/box_single.h"

using namespace std;
using namespace tinytex;

namespace tinytex {

using BoxFilter = std::function<bool(const sptr<Box>&)>;

struct RenderConfig {
  sptr<Box> root;
  float textSize;
  float fixedScale;
  color fg;
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

static void buildDebug(
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

} // namespace tinytex

Render::Render(const sptr<Box>& box, float textSize) {
  _config = new RenderConfig{box, textSize, textSize / Env::fixedTextSize(), black};
  const auto& debugConfig = DebugConfig::INSTANCE;
  if (debugConfig.enable) {
    const auto group = tinytex::wrap(box);
    _config->root = group;
    BoxFilter filter = [&](const sptr<Box>& b) {
      return (
        debugConfig.showOnlyChar
        ? dynamic_cast<CharBox*>(b.get()) != nullptr
        : !b->isSpace()
      );
    };
    tinytex::buildDebug(nullptr, group, filter);
  }
}

Render::~Render() {
  delete _config;
}

float Render::getTextSize() const {
  return _config->textSize;
}

int Render::getHeight() const {
  auto box = _config->root;
  return (int) (box->vlen() * _config->fixedScale);
}

int Render::getDepth() const {
  return (int) (_config->root->_depth * _config->fixedScale);
}

int Render::getWidth() const {
  return (int) (_config->root->_width * _config->fixedScale);
}

float Render::getBaseline() const {
  auto box = _config->root;
  return box->_height / box->vlen();
}

void Render::setTextSize(float textSize) {
  _config->textSize = textSize;
  _config->fixedScale = textSize / Env::fixedTextSize();
}

void Render::setForeground(color fg) {
  _config->fg = fg;
}

void Render::draw(Graphics2D& g2, int x, int y) {
  color old = g2.getColor();
  auto fixedScale = _config->fixedScale;
  auto box = _config->root;

  g2.setColor(isTransparent(_config->fg) ? black : _config->fg);
  g2.translate(x, y);
  g2.scale(fixedScale, fixedScale);

  // draw formula box
  box->draw(g2, 0, box->_height);

  // restore
  g2.scale(1.f / fixedScale, 1.f / fixedScale);
  g2.translate(-x, -y);
  g2.setColor(old);
}
