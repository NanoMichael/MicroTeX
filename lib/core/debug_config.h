#ifndef MICROTEX_DEBUG_CONFIG_H
#define MICROTEX_DEBUG_CONFIG_H

#include "graphic/graphic_basic.h"

namespace microtex {

/** The graphic debug configs */
struct DebugConfig {
  /** If enable debug mode */
  bool enable = false;
  /** If show only the char boxes */
  bool showOnlyChar = true;
  /** The box's bound rectangle color */
  color boundColor = RED;
  /** The box's baseline color */
  color baselineColor = BLUE;

  static DebugConfig INSTANCE;
};

}  // namespace microtex

#endif
