#ifndef MICROTEX_UNI_SYMBOL_H
#define MICROTEX_UNI_SYMBOL_H

#include "utils/types.h"
#include "utils/utils.h"

namespace microtex {

struct Symbol {
  const c32 unicode;
  const u32 flag;
  const char* name;

  AtomType type() const;

  LimitsType limitsType() const;

  bool isVariable() const;

  /**
   * Get the symbol from the given name.
   *
   * @param name the name of the symbol
   * @return the symbol of the given name or null if not found
   */
  static const Symbol* get(const char* name);
};

}  // namespace microtex

#endif  // MICROTEX_UNI_SYMBOL_H
