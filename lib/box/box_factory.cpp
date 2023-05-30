#include "box/box_factory.h"

#include <deque>

#include "atom/atom_basic.h"
#include "atom/atom_char.h"
#include "box/box_group.h"
#include "core/formula.h"
#include "env/env.h"
#include "utils/exceptions.h"
#include "utils/utils.h"

using namespace std;

namespace microtex {

static constexpr auto ROUND_TOL = 10;  // in font design unit

sptr<Box> createHDelim(const sptr<SymbolAtom>& sym, Env& env, int size) {
  const auto& chr = sym->getChar(env);
  return sptrOf<CharBox>(chr.hLarger(size));
}

sptr<Box> createVDelim(const sptr<SymbolAtom>& sym, Env& env, int size) {
  const auto& chr = sym->getChar(env);
  return sptrOf<CharBox>(chr.vLarger(size));
}

static sptr<Box> createDelim(
  const std::string& sym,
  Env& env,
  const float len,
  bool isVertical,
  bool round = false
) {
  sptr<SymbolAtom> atom;
  if (sym.length() == 1) {
    const auto it = Formula::_charToSymbol.find(sym[0]);
    if (it != Formula::_charToSymbol.end()) {
      atom = SymbolAtom::get(it->second);
    } else {
      atom = SymbolAtom::get(sym);
    }
  } else {
    atom = SymbolAtom::get(sym);
  }
  if (atom == nullptr) {
    throw ex_parse(sym + " is not a delimiter!");
  }

  const auto& chr = atom->getChar(env);
  // 1. try from variants
  const auto variantCount = isVertical ? chr.vLargerCount() : chr.hLargerCount();
  const auto tolerance = ROUND_TOL * env.scale();
  auto wider = false;
  // tail recursion, compiler will optimize this
  std::function<sptr<Box>(int)> v = [&](int i) -> sptr<Box> {
    if (i >= variantCount) return nullptr;
    const auto& n = isVertical ? chr.vLarger(i) : chr.hLarger(i);
    const auto l = isVertical ? n.height() + n.depth() : n.width();
    wider = l >= len || (round && len - l <= tolerance);
    if (i == variantCount - 1 || wider) return sptrOf<CharBox>(n);
    return v(i + 1);
  };
  auto vb = v(0);
  if (vb != nullptr && wider) return vb;

  // 2. try from assembly
  const auto& assembly = isVertical ? chr.vAssembly() : chr.hAssembly();
  if (assembly.isEmpty()) {
    if (vb != nullptr) return vb;
    return sptrOf<CharBox>(chr);
  }

  deque<pair<int, sptr<Box>>> f;
  vector<sptr<Box>> r;
  auto fl = 0.f, rl = 0.f, max = microtex::F_MAX;

  auto run = [&](int i) {
    const auto& part = assembly[i];
    const auto b = sptrOf<CharBox>(chr.assemblyPart(part.glyph()));
    const auto l = isVertical ? b->vlen() : b->_width;
    if (part.startConnectorLength() != 0) {
      max = std::min(max, (float)part.startConnectorLength());
    }
    if (part.endConnectorLength() != 0) {
      max = std::min(max, (float)part.endConnectorLength());
    }
    if (!part.isExtender()) {
      f.emplace_back(static_cast<int>(r.size()), b);
      fl += l;
    } else {
      r.push_back(b);
      rl += l;
    }
  };

  const auto pcnt = assembly.partCount();
  if (isVertical) {
    for (auto i = pcnt - 1; i >= 0; --i) run(i);
  } else {
    for (auto i = 0; i < pcnt; i++) run(i);
  }

  const auto m = env.mathConsts().minConnectorOverlap() * env.scale();
  // must satisfy:
  //   len < repeat_cnt * repeat_len + fixed_len
  //           - (repeat_part_size * repeat_cnt + fixed_part_size - 1) * min_connector_overlap
  // thus:
  //   repeat_cnt > (len - fixed_len + (fixed_part_size - 1) * min_connector_overlap)
  //                / (repeat_len - repeat_part_size * min_connector_overlap)
  const auto cnt = (int)std::ceil((len - fl + (f.size() - 1) * m) / (rl - r.size() * m));
  const auto p = r.size() * cnt + f.size() - 1;
  const auto e = (cnt * rl + fl - p * m - len) / p;
  const auto s = std::min(m + e, max);
  const auto space = (isVertical ? sptrOf<StrutBox>(0.f, -s, 0.f, 0.f) : StrutBox::create(-s));

  sptr<BoxGroup> box;
  if (isVertical)
    box = sptrOf<VBox>();
  else
    box = sptrOf<HBox>();

  for (size_t i = 0; i < r.size(); i++) {
    while (!f.empty()) {
      const auto& [k, n] = f.front();
      if (k == i) {
        box->add(n);
        box->add(space);
        f.pop_front();
      } else {
        break;
      }
    }
    const auto b = r[i];
    for (int j = 0; j < cnt; j++) {
      box->add(b);
      if (i == r.size() - 1 && j == cnt - 1 && f.empty()) break;
      box->add(space);
    }
  }

  while (!f.empty()) {
    const auto& [_, b] = f.front();
    box->add(b);
    f.pop_front();
    if (f.empty()) break;
    box->add(space);
  }

  return box;
}

sptr<Box> createHDelim(const std::string& sym, Env& env, float width, bool round) {
  return createDelim(sym, env, width, false, round);
}

sptr<Box> createVDelim(const std::string& sym, Env& env, float height, bool round) {
  return createDelim(sym, env, height, true, round);
}

}  // namespace microtex
