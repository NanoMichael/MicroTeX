#ifndef UTF_H_INCLUDED
#define UTF_H_INCLUDED

#include <string>
#include "utils/utils.h"

namespace tex {

/** Append an Unicode code-point to an utf-8 string */
void appendToUtf8(std::string& out, c32 code);

/**
 * Get the next Unicode code-point from the given utf-8 string
 * start from #i, the bytes count consumed in the byte sequence
 * was given by #cnt after the code-point was decoded.
 */
c32 nextUnicode(const std::string& src, int i, int& cnt);

/**
 * Test if a given Unicode code-point is a variation-selector.
 * See [https://en.m.wikipedia.org/wiki/Variant_form_(Unicode)]
 * for details.
 */
bool isVariationSelector(c32 code);

/**
 * Test if a given Unicode code-point is a zero-width joiner.
 * See [https://en.wikipedia.org/wiki/Zero-width_joiner]
 * for details.
 */
bool isZWJ(c32 code);

/**
 * Test if a given Unicode code-point is a zero-width non-joiner.
 * See [https://en.wikipedia.org/wiki/Zero-width_non-joiner]
 * for details
 */
bool isZWNJ(c32 code);

/** Test if a given Unicode code-point is a joiner. */
bool isJoiner(c32 code);

}  // namespace tex

#endif
