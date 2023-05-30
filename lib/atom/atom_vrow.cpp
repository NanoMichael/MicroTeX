#include "atom/atom_vrow.h"

#include "box/box_group.h"
#include "box/box_single.h"
#include "env/env.h"

using namespace microtex;
using namespace std;

VRowAtom::VRowAtom() {
  _addInterline = false;
  _valign = Alignment::center;
  _halign = Alignment::none;
  _raise = sptrOf<SpaceAtom>(UnitType::ex, 0.f, 0.f, 0.f);
}

VRowAtom::VRowAtom(const sptr<Atom>& base) {
  _addInterline = false;
  _valign = Alignment::center;
  _halign = Alignment::none;
  _raise = sptrOf<SpaceAtom>(UnitType::ex, 0.f, 0.f, 0.f);
  if (base != nullptr) {
    auto* a = dynamic_cast<VRowAtom*>(base.get());
    if (a != nullptr) {
      _elements.insert(_elements.end(), a->_elements.begin(), a->_elements.end());
    } else {
      _elements.push_back(base);
    }
  }
}

void VRowAtom::setRaise(UnitType unit, float r) {
  _raise = sptrOf<SpaceAtom>(unit, r, 0.f, 0.f);
}

sptr<Atom> VRowAtom::popLastAtom() {
  auto x = _elements.back();
  _elements.pop_back();
  return x;
}

void VRowAtom::prepend(const sptr<Atom>& el) {
  if (el != nullptr) _elements.insert(_elements.begin(), el);
}

void VRowAtom::append(const sptr<Atom>& el) {
  if (el != nullptr) _elements.push_back(el);
}

sptr<Box> VRowAtom::createBox(Env& env) {
  auto vb = new VBox();
  auto lineSpace = sptrOf<StrutBox>(0.f, env.lineSpace(), 0.f, 0.f);

  if (_halign != Alignment::none) {
    float maxWidth = F_MIN;
    vector<sptr<Box>> boxes;
    const size_t size = _elements.size();
    // find the width of the widest box
    for (auto& atom : _elements) {
      auto box = atom->createBox(env);
      boxes.push_back(box);
      maxWidth = std::max(maxWidth, box->_width);
    }
    // align the boxes and add it to the vertical box
    for (int i = 0; i < size; i++) {
      auto box = boxes[i];
      auto hb = sptrOf<HBox>(box, maxWidth, _halign);
      vb->add(hb);
      if (_addInterline && i < size - 1) vb->add(lineSpace);
    }
  } else {
    // convert atoms to boxes and add to the vertical box
    const size_t size = _elements.size();
    for (int i = 0; i < size; i++) {
      vb->add(_elements[i]->createBox(env));
      if (_addInterline && i < size - 1) vb->add(lineSpace);
    }
  }

  vb->_shift = -_raise->createBox(env)->_width;
  if (_valign == Alignment::top) {
    float t = vb->size() == 0 ? 0 : vb->_children.front()->_height;
    vb->_height = t;
    vb->_depth = vb->_depth + vb->_height - t;
  } else if (_valign == Alignment::center) {
    const float axis = env.axisHeight();
    const float h = vb->_height + vb->_depth;
    vb->_height = h / 2 + axis;
    vb->_depth = h / 2 - axis;
  } else {
    float t = vb->size() == 0 ? 0 : vb->_children.back()->_depth;
    vb->_height = vb->_depth + vb->_height - t;
    vb->_depth = t;
  }
  return sptr<Box>(vb);
}
