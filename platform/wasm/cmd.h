#ifndef LATEX_CMD_H
#define LATEX_CMD_H

#include <cstdlib>
#include <memory>

namespace tex {

class Cmds {
private:
  static constexpr size_t CHUNK_SIZE = 4 * 1024; // 4KB

  static inline size_t sizeOf() { return 0; }

  template<typename T, typename... Ts>
  static size_t sizeOf(const T& t, const Ts& ... ts) {
    return sizeof(T) + sizeOf(ts...);
  }

  size_t index;
  size_t capacity;
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

  void ensureCapacity(size_t required);

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

#endif //LATEX_CMD_H
