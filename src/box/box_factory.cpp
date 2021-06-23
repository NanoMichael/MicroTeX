#include "box/box_factory.h"
#include "atom/atom_basic.h"
#include "utils/utils.h"
#include "env/env.h"

using namespace std;
using namespace tex;

sptr<Box> tex::createDelim(SymbolAtom& sym, Env& env, int size) {
  return StrutBox::empty();
}

sptr<Box> tex::createHorDelim(const std::string& sym, Env& env, float height) {
  return StrutBox::empty();
}

sptr<Box> tex::createVerDelim(const std::string& sym, Env& env, float width) {
  return StrutBox::empty();
}
