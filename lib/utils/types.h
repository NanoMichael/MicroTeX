#ifndef TINYTEX_TYPES_H
#define TINYTEX_TYPES_H

#include <memory>
#include <cinttypes>

namespace tinytex {

using i8 = std::int8_t;
using u8 = std::uint8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using c32 = char32_t;

/** Type alias shared_ptr<T> to sptr<T> */
template<typename T>
using sptr = std::shared_ptr<T>;

/** Type alias shared_ptr<T> to uptr<T> */
template<typename T>
using uptr = std::unique_ptr<T>;

}

#endif //TINYTEX_TYPES_H
