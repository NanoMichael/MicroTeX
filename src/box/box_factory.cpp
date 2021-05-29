#include "common.h"
#include "box/box_factory.h"
#include "atom/atom_basic.h"
#include "core/core.h"
#include "utils/utils.h"

using namespace std;
using namespace tex;

sptr<Box> DelimiterFactory::create(SymbolAtom& symbol, Environment& env, int size) {
  if (size > 4) return symbol.createBox(env);

  TeXFont& tf = *(env.getTeXFont());
  const TexStyle style = env.getStyle();
  Char c = tf.getChar(symbol.getName(), style);
  int i = 0;

  for (int i = 1; i <= size && tf.hasNextLarger(c); i++) c = tf.getNextLarger(c, style);

  if (i <= size && !tf.hasNextLarger(c)) {
    CharBox A(tf.getChar(L'A', "mathnormal", style));
    auto b = create(symbol.getName(), env, size * (A._height + A._depth));
    return b;
  }

  return sptrOf<CharBox>(c);
}

sptr<Box> DelimiterFactory::create(const string& symbol, Environment& env, float minHeight) {
  TeXFont& tf = *(env.getTeXFont());
  const TexStyle style = env.getStyle();
  Char c = tf.getChar(symbol, style);

  // start with smallest character
  float total = c.getHeight() + c.getDepth();

  // try larger versions of the same char until min-height has been reached
  while (total < minHeight && tf.hasNextLarger(c)) {
    c = tf.getNextLarger(c, style);
    total = c.getHeight() + c.getDepth();
  }
  // tall enough char found
  if (total >= minHeight) {
    /**if (total > minHeight) {
            auto cb = sptrOf<CharBox>(c);
            float scale = minHeight / total;
            return sptrOf<ScaleBox>(cb, scale);
        }*/
    return sptrOf<CharBox>(c);
  } else if (tf.isExtensionChar(c)) {
    // construct vertical box
    auto* vBox = new VBox();
    Extension* ext = tf.getExtension(c, style);

    // insert top part
    if (ext->hasTop()) {
      c = ext->getTop();
      vBox->add(sptrOf<CharBox>(c));
    }

    if (ext->hasMiddle()) {
      c = ext->getMiddle();
      vBox->add(sptrOf<CharBox>(c));
    }

    if (ext->hasBottom()) {
      c = ext->getBottom();
      vBox->add(sptrOf<CharBox>(c));
    }

    // insert repeatable part until tall enough
    c = ext->getRepeat();
    auto rep = sptrOf<CharBox>(c);
    while (vBox->_height + vBox->_depth <= minHeight) {
      if (ext->hasTop() && ext->hasBottom()) {
        vBox->add(1, rep);
        if (ext->hasMiddle()) {
          vBox->add(vBox->size() - 1, rep);
        }
      } else if (ext->hasBottom()) {
        vBox->add(0, rep);
      } else {
        vBox->add(rep);
      }
    }
    delete ext;
    return sptr<Box>(vBox);
  }
  // no extensions, so return the tallest possible character
  return sptrOf<CharBox>(c);
}

sptr<Atom> XLeftRightArrowFactory::MINUS;
sptr<Atom> XLeftRightArrowFactory::LEFT;
sptr<Atom> XLeftRightArrowFactory::RIGHT;

sptr<Box> XLeftRightArrowFactory::create(Environment& env, float width) {
  // initialize
  if (MINUS == nullptr) {
    MINUS = SymbolAtom::get("minus");
    LEFT = SymbolAtom::get("leftarrow");
    RIGHT = SymbolAtom::get("rightarrow");
  }
  sptr<Box> left = LEFT->createBox(env);
  sptr<Box> right = RIGHT->createBox(env);
  float swidth = left->_width + right->_width;

  if (width < swidth) {
    auto* hb = new HBox(left);
    hb->add(sptrOf<StrutBox>(-min(swidth - width, left->_width), 0.f, 0.f, 0.f));
    hb->add(right);
    return sptr<Box>(hb);
  }

  sptr<Box> minu = SmashedAtom(MINUS, "").createBox(env);
  sptr<Box> kern = SpaceAtom(UnitType::mu, -3.4f, 0.f, 0.f).createBox(env);

  float mwidth = minu->_width + kern->_width;
  swidth += 2 * kern->_width;

  auto* hb = new HBox();
  float w = 0.f;
  for (w = 0; w < width - swidth - mwidth; w += mwidth) {
    hb->add(minu);
    hb->add(kern);
  }

  hb->add(sptrOf<ScaleBox>(minu, (width - swidth - w) / minu->_width, 1.f));

  hb->add(0, kern);
  hb->add(0, left);
  hb->add(kern);
  hb->add(right);

  return sptr<Box>(hb);
}

sptr<Box> XLeftRightArrowFactory::create(bool left, Environment& env, float width) {
  // initialize
  if (MINUS == nullptr) {
    MINUS = SymbolAtom::get("minus");
    LEFT = SymbolAtom::get("leftarrow");
    RIGHT = SymbolAtom::get("rightarrow");
  }
  auto arr = left ? LEFT->createBox(env) : RIGHT->createBox(env);
  float h = arr->_height;
  float d = arr->_depth;

  float swidth = arr->_width;
  if (width <= swidth) {
    arr->_depth = d / 2;
    return arr;
  }

  sptr<Box> minu = SmashedAtom(MINUS, "").createBox(env);
  sptr<Box> kern = SpaceAtom(UnitType::mu, -4.f, 0, 0).createBox(env);
  float mwidth = minu->_width + kern->_width;
  swidth += kern->_width;
  auto* hb = new HBox();
  float w = 0.f;
  for (w = 0; w < width - swidth - mwidth; w += mwidth) {
    hb->add(minu);
    hb->add(kern);
  }

  float sf = (width - swidth - w) / minu->_width;

  hb->add(SpaceAtom(UnitType::mu, -2.f * sf, 0, 0).createBox(env));
  hb->add(ScaleAtom(MINUS, sf, 1).createBox(env));

  if (left) {
    hb->add(0, SpaceAtom(UnitType::mu, -3.5f, 0, 0).createBox(env));
    hb->add(0, arr);
  } else {
    hb->add(SpaceAtom(UnitType::mu, -2.f * sf - 2.f, 0, 0).createBox(env));
    hb->add(arr);
  }

  hb->_depth = d / 2;
  hb->_height = h;

  return sptr<Box>(hb);
}
