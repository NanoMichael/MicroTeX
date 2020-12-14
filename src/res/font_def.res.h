#ifndef FONT_DEF_RES_H_INCLUDED
#define FONT_DEF_RES_H_INCLUDED

#include "fonts/font_info.h"
#include "fonts/font_reg.h"

#define __len(x) ((int)(sizeof(x) / sizeof((x)[0])))

#define DEF_FONT(name, path, unicode)  \
  void __font_reg(name)() {            \
    int  id   = tex::FontInfo::__id(#name); \
    auto info = tex::FontInfo::__create(id, tex::RES_BASE + "/" + #path);

#define space(x)   info->__space(x);
#define xHeight(x) info->__xheight(x);
#define quad(x)    info->__quad(x);
#define skew(x)    info->__skewChar(x);

#define __ver(m, v) info->m(tex::FontInfo::__id(#v));
#define roman(x)    __ver(__romanId, x)
#define bold(x)     __ver(__boldId, x)
#define it(x)       __ver(__itId, x)
#define ss(x)       __ver(__ssId, x)
#define tt(x)       __ver(__ttId, x)

#define __start_def(type) \
  {                       \
    static const type x[] = {
#define __end_def(method)    \
  }                          \
  ;                          \
  info->method(x, __len(x)); \
  }                          \
  ;

/**
 * Define the metrics of this font.
 * 
 * There 5 components for each item:
 * 
 *      [code, width, height, depth, italic-correction]
 */
#define METRICS_START __start_def(float)
#define METRICS_END   __end_def(__metrics)

/**
 * Define the extensions of this font.
 * 
 * There 5 components for each item:
 * 
 *      [code, top, middle, repeat, bottom]
 */
#define EXTENSIONS_START __start_def(int)
#define EXTENSIONS_END   __end_def(__extensions)

/**
 * Define the larger-version of a specific character.
 * 
 * There 3 components for each item:
 * 
 *      [code, larger-code, larger-font-id]
 */
#define LARGERS_START __start_def(int)
#define LARGERS_END   __end_def(__largers)

/**
 * Define the ligtures for 2 characters.
 * 
 * There 3 components for each item:
 * 
 *      [left-code, right-code, lig-code]
 */
#define LIGTURES_START __start_def(wchar_t)
#define LIGTURES_END   __end_def(__ligtures)

/**
 * Define the kerning for 2 characters.
 * 
 * There 3 components for each item:
 * 
 *      [left-code, right-code, kerning]
 */
#define KERNS_START __start_def(float)
#define KERNS_END   __end_def(__kerns)

#define END }

#endif
