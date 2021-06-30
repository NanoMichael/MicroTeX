#ifndef LATEX_GLUE_H
#define LATEX_GLUE_H

#include "utils/utils.h"
#include "utils/enums.h"

namespace tex {

class Env;

class GlueBox;

/** Represents glue by its 3 components. Contains the "glue rules" */
class Glue {
private:
  constexpr static int TYPE_COUNT = 8;
  constexpr static int STYLE_COUNT = 5;

  // contains the different glue types
  static const Glue _glueTypes[4];
  // the glue table represents the "glue rules"
  static const char _table[TYPE_COUNT][TYPE_COUNT][STYLE_COUNT];

  // the glue components, in "mu" unit
  u16 _space, _stretch, _shrink;

  sptr<GlueBox> createBox(const Env& env) const;

  static float getFactor(const Env& env);

  static const Glue& getGlue(SpaceType skipType);

  static int indexOf(AtomType ltype, AtomType rtype, const Env& env);

  Glue(u16 space, u16 stretch, u16 shrink) noexcept
    : _space(space), _stretch(stretch), _shrink(shrink) {}

public:
  no_copy_assign(Glue);

  /**
   * Creates a box representing the glue type according to the "glue rules" based
   * on the atom types between which the glue must be inserted.
   *
   * @param ltype left atom type
   * @param rtype right atom type
   * @param env the Env
   * @return a box containing representing the glue
   */
  static sptr<GlueBox> get(AtomType ltype, AtomType rtype, const Env& env);

  /**
   * Creates a box representing the glue type according to the "glue rules" based
   * on the skip-type
   */
  static sptr<GlueBox> get(SpaceType skipType, const Env& env);

  /**
   * Get the space amount from the given left-type and right-type of atoms
   * according to the "glue rules".
   */
  static float getSpace(AtomType ltype, AtomType rtype, const Env& env);

  /** Get the space amount from the given skip-type according to the "glue rules" */
  static float getSpace(SpaceType skipType, const Env& env);
};

}

#endif //LATEX_GLUE_H
