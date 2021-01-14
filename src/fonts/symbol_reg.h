#ifndef SYMBOL_REG_H_INCLUDED
#define SYMBOL_REG_H_INCLUDED

#include <vector>

namespace tex {

/** Symbols registration function */
typedef void (*__reg_symbols_func)(void);

/** Represents a set of symbols registration */
class SymbolsSet {
public:
  virtual std::vector<__reg_symbols_func> regs() const = 0;
};

}  // namespace tex

#define __symbols_reg(name) \
  __reg_symbols_##name

#define DECL_SYMBOLS_REG(name) \
  extern void __symbols_reg(name)()

#define DECL_SYMBOLS_SET(name)                                          \
  class SymbolsSet##name : public tex::SymbolsSet {                     \
  public:                                                               \
    virtual std::vector<tex::__reg_symbols_func> regs() const override; \
  };

#define REG_SYMBOLS(name) \
  __symbols_reg(name),

#define DEF_SYMBOLS_SET(name)                                           \
  std::vector<tex::__reg_symbols_func> SymbolsSet##name::regs() const { \
    return {
#define END_DEF_SYMBOLS_SET \
  }                         \
  ;                         \
  }

#endif
