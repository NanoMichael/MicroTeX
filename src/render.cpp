#include "render.h"

#include "atom/atom.h"
#include "core/core.h"
#include "core/formula.h"
#include "unimath/uni_font.h"
#include "env/env.h"
#include "env/units.h"

using namespace std;
using namespace tex;

TeXRender::TeXRender(const sptr<Box>& box, float textSize, bool hasPadding) {
  _box = box;
  _textSize = textSize;
  _fixedScale = _textSize / Env::fixedTextSize();
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

float TeXRender::getTextSize() const {
  return _textSize;
}

int TeXRender::getHeight() const {
  return (int) ((_box->_height + _box->_depth) * _fixedScale);
}

int TeXRender::getDepth() const {
  return (int) (_box->_depth * _fixedScale);
}

int TeXRender::getWidth() const {
  return (int) (_box->_width * _fixedScale);
}

float TeXRender::getBaseline() const {
  return _box->_height / (_box->_height + _box->_depth);
}

void TeXRender::setTextSize(float textSize) {
  _textSize = textSize;
  _fixedScale = _textSize / Env::fixedTextSize();
}

void TeXRender::setForeground(color fg) {
  _fg = fg;
}

void TeXRender::setWidth(int width, Alignment align) {
  float diff = width - getWidth();
  // only care if new width larger than old
  if (diff > 0) {
    _box = sptrOf<HBox>(_box, (float) width, align);
  }
}

void TeXRender::setHeight(int height, Alignment align) {
  float diff = height - getHeight();
  // only care if new height larger than old
  if (diff > 0) {
    _box = sptrOf<VBox>(_box, diff, align);
  }
}

void TeXRender::draw(Graphics2D& g2, int x, int y) {
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

TeXRender* TeXRenderBuilder::build(Formula& f) {
  return build(f._root);
}

TeXRender* TeXRenderBuilder::build(const sptr<Atom>& fc) {
  sptr<Atom> f = fc;
  if (f == nullptr) f = sptrOf<EmptyAtom>();
  if (_textSize == -1) {
    throw ex_invalid_state("A size is required, call function setSize before build.");
  }
  if (_mathFontName.empty()) {
    throw ex_invalid_state("A math font is required, call function setMathFontName before build.");
  }

  auto fctx = sptrOf<FontContext>();
  fctx->selectMathFont(_mathFontName);
  fctx->selectMainFont(_mainFontName);

  Env env(_style, fctx, _textSize);
  const auto isLimitedWidth = _widthUnit != UnitType::none && _textWidth != 0;
  if (isLimitedWidth) {
    env.setTextWidth(_widthUnit, _textWidth);
  }
  if (_lineSpaceUnit != UnitType::none) {
    env.setLineSpace(_lineSpaceUnit, _lineSpace);
  }

  TeXRender* render;
  auto box = f->createBox(env);
  if (isLimitedWidth) {
    HBox* hb;
    if (_lineSpaceUnit != UnitType::none && _lineSpace != 0) {
      auto space = Units::fsize(_lineSpaceUnit, _lineSpace, env);
      auto split = BoxSplitter::split(box, env.textWidth(), space);
      hb = new HBox(split, _isMaxWidth ? split->_width : env.textWidth(), _align);
    } else {
      hb = new HBox(box, _isMaxWidth ? box->_width : env.textWidth(), _align);
    }
    render = new TeXRender(sptr<Box>(hb), _textSize);
  } else {
    render = new TeXRender(box, _textSize);
  }

  if (!isTransparent(_fg)) render->setForeground(_fg);
  return render;
}
