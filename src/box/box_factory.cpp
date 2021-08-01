#include "box/box_factory.h"
#include "atom/atom_basic.h"
#include "utils/utils.h"
#include "env/env.h"
#include "atom/atom_char.h"

#include <deque>

using namespace std;

namespace tex {

sptr<Box> createHDelim(const sptr<SymbolAtom>& sym, Env& env, int size) {
  const auto& chr = sym->getChar(env);
  return sptrOf<CharBox>(chr.hLarger(size));
}

sptr<Box> createVDelim(const sptr<SymbolAtom>& sym, Env& env, int size) {
  const auto& chr = sym->getChar(env);
  return sptrOf<CharBox>(chr.vLarger(size));
}

static sptr<Box> createDelim(const std::string& sym, Env& env, const float len, bool isVertical) {
  const auto& atom = SymbolAtom::get(sym);
  const auto& chr = atom->getChar(env);
  // 1. try from variants
  const auto variantCount = isVertical ? chr.vLargerCount() : chr.hLargerCount();
  // tail recursion, compiler will optimize this
  std::function<sptr<Box>(int)> v = [&](int i) -> sptr<Box> {
    if (i >= variantCount) return nullptr;
    const auto& n = isVertical ? chr.vLarger(i) : chr.hLarger(i);
    const auto l = isVertical ? n.height() + n.depth() : n.width();
    if (l >= len) return sptrOf<CharBox>(n);
    return v(i + 1);
  };
  auto vb = v(0);
  if (vb != nullptr) return vb;

  // 2. try from assembly
  const auto& assembly = isVertical ? chr.vAssembly() : chr.hAssembly();
  if (assembly.isEmpty()) return sptrOf<CharBox>(chr);

  deque<pair<int, sptr<Box>>> f;
  vector<sptr<Box>> r;
  auto fl = 0.f, rl = 0.f;

  auto run = [&](int i) {
    const auto& part = assembly[i];
    const auto b = sptrOf<CharBox>(chr.assemblyPart(part.glyph()));
    const auto l = isVertical ? b->vlen() : b->_width;
    if (!part.isExtender()) {
      f.emplace_back(r.size(), b);
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
  const auto cnt = (int) std::ceil((len - fl + (f.size() - 1) * m) / (rl - r.size() * m));
  const auto p = r.size() * cnt + f.size() - 1;
  const auto e = (cnt * rl + fl - p * m - len) / p;
  const auto space = (
    isVertical
    ? sptrOf<StrutBox>(0.f, -(m + e), 0.f, 0.f)
    : StrutBox::create(-(m + e))
  );

  sptr<BoxGroup> box;
  if (isVertical) box = sptrOf<VBox>();
  else box = sptrOf<HBox>();

  for (size_t i = 0; i < r.size(); i++) {
    while (!f.empty()) {
      const auto&[k, n] = f.front();
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
    const auto&[_, b] = f.front();
    box->add(b);
    f.pop_front();
    if (f.empty()) break;
    box->add(space);
  }

  return box;
}

sptr<Box> createHDelim(const std::string& sym, Env& env, float width) {
  return createDelim(sym, env, width, false);
}

sptr<Box> createVDelim(const std::string& sym, Env& env, float height) {
  return createDelim(sym, env, height, true);
}

}
