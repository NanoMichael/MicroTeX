#ifndef UTF_H_INCLUDED
#define UTF_H_INCLUDED

#include <string>

#include "utils/utils.h"

namespace microtex {

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

/**
 * Scan Unicodes that cannot be split from a sequence and collect
 * it into an arbitrary container.
 *
 * @param next function to get the next Unicode code-point, 0 terminated
 * @param collect function to collect the given Unicode-point
 */
void scanContinuedUnicodes(
  const std::function<c32()>& next,
  const std::function<void(c32)>& collect
);

}  // namespace microtex

#endif
