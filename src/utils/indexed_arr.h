#ifndef INDEXED_ARR_H_INCLUDED
#define INDEXED_ARR_H_INCLUDED

#include <cstdio>

namespace tex {

/**
 * Template to represents 2 dimensions array with N elements for each item and
 * sorted by the first element.
 */
template <class T, size_t N>
class IndexedArray {
private:
  const T* const _arr;
  const size_t _len;

  IndexedArray() {}

  IndexedArray(const IndexedArray& arr) {}

public:
  IndexedArray(const T* a, const size_t len) : _arr(a), _len(len) {}

  /** Columns of this array, equal to N */
  inline size_t columns() const {
    return N;
  }

  /** Rows of this array */
  inline size_t rows() const {
    return _len / N;
  }

  /** Find the array by the given key, return nullptr if not found */
  const T* operator[](const T& key) const {
    size_t l = 0, h = rows() - 1;
    while (l <= h) {
      const size_t m = (l + h) >> 1;
      const T* const p = _arr + (m * N);

      if (key == *p) return p;

      if (key < *p)
        h = m - 1;
      else
        l = m + 1;
    }
    return nullptr;
  }
};

}  // namespace tex

#endif
