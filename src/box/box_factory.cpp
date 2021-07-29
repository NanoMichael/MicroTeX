#include "box/box_factory.h"
#include "atom/atom_basic.h"
#include "utils/utils.h"
#include "env/env.h"

using namespace std;
using namespace tex;

sptr<Box> tex::createHDelim(const sptr<SymbolAtom>& sym, Env& env, int size) {
  const auto& chr = sym->getChar(env);
  return sptrOf<CharBox>(chr.hLarger(size));
}

sptr<Box> tex::createVDelim(const sptr<SymbolAtom>& sym, Env& env, int size) {
  const auto& chr = sym->getChar(env);
  return sptrOf<CharBox>(chr.vLarger(size));
}

sptr<Box> tex::createHDelim(const std::string& sym, Env& env, float width) {
  return StrutBox::empty();
}

sptr<Box> tex::createVDelim(const std::string& sym, Env& env, float height) {
  return StrutBox::empty();
}
