#ifndef UTF_H_INCLUDED
#define UTF_H_INCLUDED

#include <string>

namespace tex {

/** Convert unicode wide string to UTF-8 encoded string. */
std::string wide2utf8(const std::wstring& src);

/**
 * Convert an UTF-8 encoded char sequence to wide unicode string,
 * the encoding of input char sequence must be known as UTF-8
 */
std::wstring utf82wide(const std::string& src);

}  // namespace tex

#endif
