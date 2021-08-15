#ifndef UTF_H_INCLUDED
#define UTF_H_INCLUDED

#include <string>
#include "utils/utils.h"

namespace tex {

/** Append a unicode code-point to a utf-8 string */
void appendToUtf8(std::string& out, c32 code);

/** Convert unicode wide string to UTF-8 string. */
std::string wide2utf8(const std::wstring& src);

/**
 * Convert an UTF-8 encoded char sequence to wide unicode string,
 * the encoding of input char sequence must be known as UTF-8
 */
std::wstring utf82wide(const std::string& src);

}  // namespace tex

#endif
