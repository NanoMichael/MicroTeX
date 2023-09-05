#include "atom/atom_zstack.h"

#include "atom/atom_basic.h"
#include "box/box_group.h"
#include "box/box_single.h"
#include "env/env.h"
#include "env/units.h"

using namespace microtex;

ZStackAtom::ZStackAtom(
    const ZStackArgs& hargs,
    const ZStackArgs& vargs,
    const sptr<Atom>& atom,
    const sptr<Atom>& anchor
  ) : _hargs(hargs), _vargs(vargs) {
      _atom = atom == nullptr ? sptrOf<EmptyAtom>() : atom;
      _anchor = anchor == nullptr ? sptrOf<EmptyAtom>() : anchor;
}

sptr<Box> ZStackAtom::createBox(Env& env) {
  const auto anchor = _anchor->createBox(env);
  const auto box = _atom->createBox(env);

  // calculate horizontal position
  auto offset = Units::fsize(_hargs.offset, env);
  auto l = 0.f;
  if (_hargs.align == Alignment::left) {
    l = offset;
  } else if (_hargs.align == Alignment::center) {
    l = (anchor->_width - box->_width) / 2 + offset;
  } else if (_hargs.align == Alignment::right) {
    l = anchor->_width - box->_width + offset;
  }

  const auto hbox = sptrOf<HBox>();
  if (l < 0) hbox->add(StrutBox::create(-l));
  hbox->add(anchor);
  const auto minWidth = hbox->_width;

  // add a kern box to shift the left position to the left position of the box
  // to be stacked relative to the anchor box
  const auto kern = l < 0 ? -hbox->_width : l - anchor->_width;
  if (kern != 0.f) hbox->add(StrutBox::create(kern));

  // calculate vertical shift (default align at baseline)
  auto shift = Units::fsize(_vargs.offset, env);
  if (_vargs.align == Alignment::top) {
    shift += box->_height - anchor->_height;
  } else if (_vargs.align == Alignment::bottom) {
    shift = anchor->_depth - box->_depth - shift;
  } else if (_vargs.align == Alignment::center) {
    auto x = anchor->_depth - box->_depth;
    auto y = (anchor->vlen() - box->vlen()) / 2;
    shift = x - y - shift;
  }
  box->_shift = shift;

  hbox->add(box);

  if (hbox->_width < minWidth) {
    hbox->add(StrutBox::create(minWidth - hbox->_width));
  }

  return hbox;
}
