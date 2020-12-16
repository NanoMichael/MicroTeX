#ifndef FONT_REG_H_INCLUDED
#define FONT_REG_H_INCLUDED

#include <string>
#include <vector>

namespace tex {

class FontInfo;

/** Font registration function */
typedef void (*__reg_font_func)(void);

/** Represents a font description registration */
typedef struct {
  std::string name;
  __reg_font_func reg;
} FontReg;

/** Represents a set of font descriptions */
class FontSet {
public:
  virtual std::vector<FontReg> regs() const = 0;
};

}  // namespace tex

#define __font_reg(name) \
  __reg_font_##name

#define DECL_FONT_REG(name) \
  extern void __font_reg(name)()

#define DECL_FONT_SET(name)                                  \
  class FontSet##name : public tex::FontSet {                \
  public:                                                    \
    virtual std::vector<tex::FontReg> regs() const override; \
  };

#define REG_FONT(name) \
  {#name, __font_reg(name)},

#define DEF_FONT_SET(name)                                \
  std::vector<tex::FontReg> FontSet##name::regs() const { \
    return {
#define END_DEF_FONT_SET \
  }                      \
  ;                      \
  }

#endif
