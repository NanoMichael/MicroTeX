#ifndef UTF_H_INCLUDED
#define UTF_H_INCLUDED

#include <string>
#include "utils/utils.h"

namespace tex {

/** Append a unicode code-point to a utf-8 string */
void appendToUtf8(std::string& out, c32 code);

/**
 * Get the next Unicode code-point from the given utf-8 string,
 * the index of the following code-point in the byte sequence
 * was given by #i after current code-point was decoded.
 */
c32 nextUnicode(const std::string& src, int& i);

}  // namespace tex

#endif
