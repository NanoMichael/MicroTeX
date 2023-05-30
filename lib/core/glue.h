#ifndef MICROTEX_GLUE_H
#define MICROTEX_GLUE_H

#include "utils/utils.h"

namespace microtex {

class Env;

class GlueBox;

/** Represents glue by its 3 components. Contains the "glue rules" */
class Glue {
private:
  // the glue components, in "mu" unit
  u16 _space, _stretch, _shrink;

  sptr<GlueBox> createBox(const Env& env) const;

public:
  no_copy_assign(Glue);

  Glue(u16 space, u16 stretch, u16 shrink) noexcept
      : _space(space), _stretch(stretch), _shrink(shrink) {}

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

}  // namespace microtex

#endif  // MICROTEX_GLUE_H
