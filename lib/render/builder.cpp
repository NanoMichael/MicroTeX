#include "builder.h"
#include "core/formula.h"
#include "core/split.h"
#include "utils/exceptions.h"
#include "atom/atom_basic.h"
#include "box/box_single.h"
#include "box/box_group.h"

using namespace tinytex;

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
  fctx->selectMainFont(_mainFontName);

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
