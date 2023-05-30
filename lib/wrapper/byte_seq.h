#ifndef MICROTEX_BYTE_SEQ_H
#define MICROTEX_BYTE_SEQ_H

#ifdef HAVE_CWRAPPER

#include <cstring>

#include "utils/utils.h"

namespace microtex {

using len_t = unsigned int;

class ByteSeq {
private:
  static constexpr len_t CHUNK_SIZE = 4 * 1024;  // 4KB

  static inline len_t sizeOf() { return 0; }

  static inline len_t sizeOf(const char* str) { return strlen(str) + 1; }

  template <typename T, typename... Ts>
  static len_t sizeOf(const T& t, const Ts&... ts) {
    return sizeof(T) + sizeOf(ts...);
  }

  len_t _index;
  len_t _capacity;
  void* _data;

  inline void _put() {}

  void _put(const char* str);

  template <typename T>
  void _put(T t) {
    auto l = sizeof(T);
    T* ptr = (T*)((char*)_data + _index);
    *ptr = t;
    _index += l;
  }

  template <typename T, typename... Ts>
  void _put(T t, Ts... ts) {
    _put(t);
    _put(ts...);
  }

  void ensureCapacity(len_t required);

public:
  no_copy_assign(ByteSeq);

  ByteSeq();

  template <typename T, typename... Ts>
  void put(T t, Ts... ts) {
    auto l = sizeOf(t, ts...);
    ensureCapacity(l);
    _put(t, ts...);
  }

  void* finish();
};

}  // namespace microtex

#endif  // HAVE_CWRAPPER

#endif  // MICROTEX_BYTE_SEQ_H
