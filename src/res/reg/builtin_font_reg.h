#ifndef BUILTIN_FONT_REG_H_INCLUDED
#define BUILTIN_FONT_REG_H_INCLUDED

#include "fonts/font_info.h"
#include "fonts/font_reg.h"

/**
 * DEFAULT FONT TABLES
 * Page 437 in [The TeXBook]
 */
DECL_FONT_REG(msbm10);
DECL_FONT_REG(cmex10);
DECL_FONT_REG(cmmi10);
DECL_FONT_REG(cmmib10);
DECL_FONT_REG(moustache);
DECL_FONT_REG(cmmi10_unchanged);
DECL_FONT_REG(cmmib10_unchanged);
DECL_FONT_REG(stmary10);
DECL_FONT_REG(cmsy10);
DECL_FONT_REG(msam10);
DECL_FONT_REG(cmbsy10);
DECL_FONT_REG(dsrom10);
DECL_FONT_REG(rsfs10);
DECL_FONT_REG(eufm10);
DECL_FONT_REG(eufb10);
DECL_FONT_REG(cmti10);
DECL_FONT_REG(cmti10_unchanged);
DECL_FONT_REG(cmbxti10);
DECL_FONT_REG(cmr10);
DECL_FONT_REG(cmss10);
DECL_FONT_REG(cmssi10);
DECL_FONT_REG(cmtt10);
DECL_FONT_REG(cmbx10);
DECL_FONT_REG(cmssbx10);
DECL_FONT_REG(special);
DECL_FONT_REG(r10);
DECL_FONT_REG(r10_unchanged);
DECL_FONT_REG(ss10);
DECL_FONT_REG(si10);
DECL_FONT_REG(i10);
DECL_FONT_REG(bx10);
DECL_FONT_REG(bi10);
DECL_FONT_REG(sbi10);
DECL_FONT_REG(sb10);
DECL_FONT_REG(tt10);

namespace tex {

DECL_FONT_SET(Builtin);

}  // namespace tex

#endif
