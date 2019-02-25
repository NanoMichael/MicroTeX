#ifndef FONT_DEF_RES_H_INCLUDED
#define FONT_DEF_RES_H_INCLUDED

#include "fonts/font_info.h"
#include "fonts/font_reg.h"

#define __len(x) ((int)(sizeof(x) / sizeof((x)[0])))

#define DEF_FONT(name, path, unicode)   \
    void __font_reg(name)() {           \
        int id = FontInfo::__id(#name); \
        auto info = FontInfo::__create(id, RES_BASE + "/" + #path, unicode);

#define space(x)   info->__set_space(x);
#define xHeight(x) info->__set_xheight(x);
#define quad(x)    info->__set_quad(x);
#define skew(x)    info->setSkewChar(x);

#define __ver(m, v) info->m(FontInfo::__id(#v));
#define roman(x)    __ver(setRomanId, x)
#define bold(x)     __ver(setBoldId, x)
#define it(x)       __ver(setItId, x)
#define ss(x)       __ver(setSsId, x)
#define tt(x)       __ver(setTtId, x)

#define __start_def(type) \
    {                     \
        const type x[] = {
#define __end_def(method)      \
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
#define METRICS_END   __end_def(__push_metrics)

/**
 * Define the extensions of this font.
 * 
 * There 5 components for each item:
 * 
 *      [code, top, middle, repeat, bottom]
 */
#define EXTENSIONS_START __start_def(int)
#define EXTENSIONS_END   __end_def(__push_extensions)

/**
 * Define the larger-version of a specific character.
 * 
 * There 3 components for each item:
 * 
 *      [code, larger-code, larger-font-id]
 */
#define LARGERS_START __start_def(int)
#define LARGERS_END   __end_def(__push_largers)

/**
 * Define the ligtures for 2 characters.
 * 
 * There 3 components for each item:
 * 
 *      [left-code, right-code, lig-code]
 */
#define LIGTURES_START __start_def(wchar_t)
#define LIGTURES_END   __end_def(__push_ligtures)

/**
 * Define the kerning for 2 characters.
 * 
 * There 3 components for each item:
 * 
 *      [left-code, right-code, kerning]
 */
#define KERNS_START __start_def(float)
#define KERNS_END   __end_def(__push_kerns)

#define END }

#endif
