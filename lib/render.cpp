#include "render.h"

#include "atom/atom.h"
#include "core/split.h"
#include "core/formula.h"
#include "core/debug_config.h"
#include "unimath/uni_font.h"
#include "env/env.h"
#include "env/units.h"
#include "utils/exceptions.h"
#include "box/box_single.h"
#include "atom/atom_basic.h"

using namespace std;
using namespace tex;

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
    buildDebug(nullptr, group, std::move(filter));
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

void Render::setWidth(int width, Alignment align) {
  float diff = width - getWidth();
  // only care if new width larger than old
  if (diff > 0) {
    _box = sptrOf<HBox>(_box, (float) width, align);
  }
}

void Render::setHeight(int height, Alignment align) {
  float diff = height - getHeight();
  // only care if new height larger than old
  if (diff > 0) {
    _box = sptrOf<VBox>(_box, diff, align);
  }
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

TexStyle RenderBuilder::_overrideStyle = TexStyle::text;
bool RenderBuilder::_enableOverrideStyle = false;

RenderBuilder& RenderBuilder::setLineSpace(UnitType unit, float space) {
  if (_widthUnit == UnitType::none) {
    throw ex_invalid_state("Cannot set line space without having specified a width!");
  }
  _lineSpace = space;
  _lineSpaceUnit = unit;
  return *this;
}

RenderBuilder& RenderBuilder::setIsMaxWidth(bool i) {
  if (_widthUnit == UnitType::none) {
    throw ex_invalid_state("Cannot set 'isMaxWidth' without having specified a width!");
  }
  if (i) {
    // Currently isMaxWidth==true does not work with
    // Alignment::center or Alignment::right (see HBox constructor)
    //
    // The case (1) we don't support by setting align := Alignment::left
    // here is this:
    //      \text{hello world\\hello} with align=Alignment::center (but forced
    //      to Alignment::left) and isMaxWidth==true results in:
    //      [hello world]
    //      [hello ]
    // and NOT:
    //      [hello world]
    //      [ hello ]
    //
    // However, this case (2) is currently not supported anyway
    // (Alignment::center with isMaxWidth==false):
    //      [ hello world ]
    //      [ hello ]
    // and NOT:
    //      [ hello world ]
    //      [ hello ]
    //
    // => until (2) is solved, we stick with the hack to set align
    // := Alignment::left!
    _align = Alignment::left;
  }
  _isMaxWidth = i;
  return *this;
}

Render* RenderBuilder::build(Formula& f) {
  return build(f._root);
}

Render* RenderBuilder::build(const sptr<Atom>& fc) {
  sptr<Atom> f = fc;
  if (f == nullptr) f = sptrOf<EmptyAtom>();
  if (_textSize == -1) {
    throw ex_invalid_state("A text size is required, call function setTextSize before build.");
  }
  if (_mathFontName.empty()) {
    throw ex_invalid_state("A math font is required, call function setMathFontName before build.");
  }

  auto fctx = sptrOf<FontContext>();
  fctx->selectMathFont(_mathFontName);
  if (!_mainFontName.empty()) {
    fctx->selectMainFont(_mainFontName);
  }

  const auto style = _enableOverrideStyle ? _overrideStyle : _style;
  Env env(style, fctx, _textSize);
  const auto isLimitedWidth = _widthUnit != UnitType::none && _textWidth != 0;
  if (isLimitedWidth) {
    env.setTextWidth(_widthUnit, _textWidth);
  }
  if (_lineSpaceUnit != UnitType::none) {
    env.setLineSpace(_lineSpaceUnit, _lineSpace);
  }

  Render* render;
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
    render = new Render(sptr<Box>(hb), _textSize);
  } else {
    render = new Render(box, _textSize);
  }

  if (!isTransparent(_fg)) render->setForeground(_fg);
  return render;
}
