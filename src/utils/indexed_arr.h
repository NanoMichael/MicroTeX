#ifndef INDEXED_ARR_H_INCLUDED
#define INDEXED_ARR_H_INCLUDED

namespace tex {

/**
 * Template to represents 2 dimensions array with N element(s) for each item and
 * sorted by the first M element(s).
 */
template <typename T, size_t N, size_t M>
class IndexedArray {
private:
  const T* _raw;
  size_t   _rows;
  bool     _auto_delete;

  char compare(const T a[M], const T b[M]) const {
    for (size_t i = 0; i < M; i++) {
      if (a[i] < b[i]) return -1;
      if (a[i] > b[i]) return 1;
    }
    return 0;
  }

public:
  IndexedArray(const IndexedArray& arr) = delete;

  IndexedArray(IndexedArray&& arr) = delete;

  IndexedArray() : _raw(nullptr), _rows(0), _auto_delete(false) {}

  IndexedArray(const T* arr, int len, bool auto_delete = false)
      : _raw(arr), _rows(len / N), _auto_delete(auto_delete) {}

  void operator=(IndexedArray&& o) {
    _raw         = o._raw;
    _rows        = o._rows;
    _auto_delete = o._auto_delete;
    // reset o
    o._raw         = nullptr;
    o._rows        = 0;
    o._auto_delete = false;
  }

  /** Find the item by the given keys, return nullptr if not found */
  template <typename... Ks>
  const T* operator()(const Ks&... keys) const {
    if (_raw == nullptr) return nullptr;
    const T k[] = {keys...};
    int     l = 0, h = _rows;
    while (l <= h) {
      const int  m   = l + ((h - l) >> 1);
      const T*   r   = _raw + (m * N);
      const char cmp = compare(k, r);
      if (cmp == 0) return r;
      cmp < 0 ? h = m - 1 : l = m + 1;
    }
    return nullptr;
  }

  /** Get the item by the given index, return nullptr if index out of range */
  const T* operator[](const size_t i) const {
    if (_raw == nullptr || i < 0 || i >= _rows) return nullptr;
    return _raw + (i * N);
  }

  inline size_t columns() const {
    return N;
  }

  inline size_t rows() const {
    return _rows;
  }

  inline bool isEmpty() const {
    return _rows == 0 || _raw == nullptr;
  }

  ~IndexedArray() {
    if (_auto_delete && _raw != nullptr) delete[] _raw;
    _raw = nullptr;
  }
};

}  // namespace tex

#endif
