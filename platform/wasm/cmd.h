#ifndef MICROTEX_CMD_H
#define MICROTEX_CMD_H

namespace microtex {

using len_t = unsigned int;

class Cmds {
private:
  static constexpr len_t CHUNK_SIZE = 4 * 1024; // 4KB

  static inline len_t sizeOf() { return 0; }

  template<typename T, typename... Ts>
  static len_t sizeOf(const T& t, const Ts& ... ts) {
    return sizeof(T) + sizeOf(ts...);
  }

  len_t index;
  len_t capacity;
  void* data;

  inline void _put() {}

  template<typename T>
  void _put(T t) {
    auto l = sizeof(T);
    T* ptr = (T*) ((char*) data + index);
    *ptr = t;
    index += l;
  }

  template<typename T, typename... Ts>
  void _put(T t, Ts... ts) {
    _put(t);
    _put(ts...);
  }

  void ensureCapacity(len_t required);

public:

  Cmds();

  template<typename T, typename... Ts>
  void put(T t, Ts... ts) {
    auto l = sizeOf(t, ts...);
    ensureCapacity(l);
    _put(t, ts...);
  }

  void* finish();
};

}

#endif //MICROTEX_CMD_H
