#include "atom/atom_sideset.h"

#include "atom/atom_basic.h"
#include "atom/atom_scripts.h"
#include "atom/atom_stack.h"
#include "atom/atom_zstack.h"

using namespace microtex;

SideSetsAtom::SideSetsAtom(const sptr<Atom>& base, const sptr<Atom>& left, const sptr<Atom>& right) {
  _base = base == nullptr ? sptrOf<EmptyAtom>() : base;
  _left = left == nullptr ? sptrOf<EmptyAtom>() : left;
  _right = right == nullptr ? sptrOf<EmptyAtom>() : right;
  _type = AtomType::bigOperator;
  _limitsType = LimitsType::noLimits;
}

sptr<Box> SideSetsAtom::createBox(Env& env) {
  _base->_limitsType = LimitsType::noLimits;
  sptr<Box> l, m, r;
  sptr<PlaceholderAtom> pa;
  auto italic = 0.f;

  ScriptResult right;
  if (auto sr = dynamic_cast<ScriptsAtom*>(_right.get()); sr != nullptr && sr->_base == nullptr) {
    right = ScriptsAtom(_base, sr->_sub, sr->_sup).createScripts(env);
    m = right.base;
    pa = _base->isChar() ? nullptr
                         : sptrOf<PlaceholderAtom>(m->_width, m->_height, m->_depth, right.italic);
    italic = right.italic;
  } else {
    _right = _right == nullptr ? sptrOf<EmptyAtom>() : _right;
    r = _right->createBox(env);
  }

  ScriptResult left;
  if (auto sl = dynamic_cast<ScriptsAtom*>(_left.get()); sl != nullptr && sl->_base == nullptr) {
    const auto base = pa == nullptr ? _base : pa;
    left = ScriptsAtom(base, sl->_sub, sl->_sup, false).createScripts(env);
    if (m == nullptr) m = left.base;
    italic = left.italic;
  } else {
    l = _left->createBox(env);
  }

  if (m == nullptr) {
    m = _base->createBox(env);
  }

  const auto hbox = sptrOf<HBox>();
  if (l != nullptr) {
    hbox->add(l);
  } else {
    const auto& [_1, scripts, space, kern, widthReduce, _2] = left;
    hbox->add(StrutBox::create(space));
    hbox->add(scripts);
    const auto d = kern + widthReduce;
    if (std::abs(d) > PREC) hbox->add(StrutBox::create(d));
  }

  const auto ml = hbox->_width;
  hbox->add(m);

  if (r != nullptr) {
    hbox->add(r);
  } else {
    const auto& [_1, scripts, space, kern, widthReduce, _2] = right;
    const auto d = kern + widthReduce;
    if (std::abs(d) > PREC) hbox->add(StrutBox::create(d));
    hbox->add(scripts);
    hbox->add(StrutBox::create(space));
  }

  sptr<Box> limits;
  float shift = 0.f;
  if (_under != nullptr || _over != nullptr) {
    const auto k = sptrOf<PlaceholderAtom>(m->_width, hbox->_height, hbox->_depth, italic);
    const auto& result =
      StackAtom(k, StackArgs::autoSpace(_over), StackArgs::autoSpace(_under)).createStack(env);
    limits = result.box;
    shift = result.kernelShift;
  }

  if (limits == nullptr) return hbox;

  const ZStackArgs hargs{Alignment::left, ml - shift, UnitType::none};
  const ZStackArgs vargs{Alignment::none};
  const auto anchor = sptrOf<PlaceholderAtom>(hbox);
  const auto atom = sptrOf<PlaceholderAtom>(limits);

  return ZStackAtom(hargs, vargs, atom, anchor).createBox(env);
}
