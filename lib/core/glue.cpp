#include "core/glue.h"

#include "box/box_single.h"
#include "env/env.h"
#include "env/units.h"

using namespace std;
using namespace microtex;

namespace {
constexpr int TYPE_COUNT = 8;
constexpr int STYLE_COUNT = 5;

// contains the different glue types
const Glue _glueTypes[4]{
  {0, 0, 0},
  {3, 0, 0},
  {4, 4, 2},
  {5, 0, 5},
};

/*
 GLUE TABLE
 Page 181 in [The TeXBook]
 -------------------------------------------------------
       ORD   OP    BIN   REL   OPEN  CLOSE  PUNCT  INNER
 ORD    0     1    (2)   (3)    0     0      0     (1)
 OP     1     1     *    (3)    0     0      0     (1)
 BIN   (2)   (2)    *     *    (2)    *      *     (2)
 REL   (3)   (3)    *     0    (3)    0      0     (3)
 OPEN  (0)    0     *     0     0     0      0      0
 CLOSE (0)    1    (2)   (3)    0     0      0     (1)
 PUNCT (1)   (1)    *    (1)   (1)   (1)    (1)    (1)
 INNER (1)    1    (2)   (3)   (1)    0     (1)    (1)

 0: no space
 1: thin space
 2: medium space
 3: thick space

 The table entry is parenthesized if the space is to be inserted only in
 display and text styles, not in script and scriptscript styles.

 Some of the entries in the table are ‘*’; such cases never arise, because
 Bin atoms must be preceded and followed by atoms compatible with the
 nature of binary operations.
*/
const char _table[TYPE_COUNT][TYPE_COUNT][STYLE_COUNT]{
  {"0000", "1111", "2200", "3300", "0000", "0000", "0000", "1100"},
  {"1111", "1111", "0000", "3300", "0000", "0000", "0000", "1100"},
  {"2200", "2200", "0000", "0000", "2200", "0000", "0000", "2200"},
  {"3300", "3300", "0000", "0000", "3300", "0000", "0000", "3300"},
  {"0000", "0000", "0000", "0000", "0000", "0000", "0000", "0000"},
  {"0000", "1111", "2200", "3300", "0000", "0000", "0000", "1100"},
  {"1100", "1100", "0000", "1100", "1100", "1100", "1100", "1100"},
  {"1100", "1111", "2200", "3300", "1100", "0000", "1100", "1100"},
};

float getFactor(const Env& env) {
  return Units::fsize(UnitType::mu, 1.f, env);
}

const Glue& getGlue(SpaceType skipType) {
  const i8 i = static_cast<i8>(skipType);
  return _glueTypes[i < 0 ? -i : i];
}

int indexOf(AtomType ltype, AtomType rtype, const Env& env) {
  // types > INNER are considered of type ORD for glue calculations
  AtomType l = (ltype > AtomType::inner ? AtomType::ordinary : ltype);
  AtomType r = (rtype > AtomType::inner ? AtomType::ordinary : rtype);
  const i8 k = static_cast<i8>(env.style()) / 2;
  return _table[static_cast<u8>(l)][static_cast<u8>(r)][k] - '0';
}

}  // namespace

sptr<GlueBox> Glue::createBox(const Env& env) const {
  float factor = getFactor(env);
  return sptrOf<GlueBox>(_space * factor, _stretch * factor, _shrink * factor);
}

sptr<GlueBox> Glue::get(AtomType ltype, AtomType rtype, const Env& env) {
  if (ltype == AtomType::none || rtype == AtomType::none) {
    return sptrOf<GlueBox>(0.f, 0.f, 0.f);
  }
  int i = indexOf(ltype, rtype, env);
  return _glueTypes[i].createBox(env);
}

sptr<GlueBox> Glue::get(SpaceType skipType, const Env& env) {
  const Glue& glue = getGlue(skipType);
  auto b = glue.createBox(env);
  if (static_cast<i8>(skipType) < 0) b->negWidth();
  return b;
}

float Glue::getSpace(AtomType ltype, AtomType rtype, const Env& env) {
  int i = indexOf(ltype, rtype, env);
  const Glue& glueType = _glueTypes[i];
  return glueType._space * getFactor(env);
}

float Glue::getSpace(SpaceType skipType, const Env& env) {
  const Glue& glue = getGlue(skipType);
  const auto v = glue._space * getFactor(env);
  return static_cast<i8>(skipType) < 0 ? -v : v;
}
