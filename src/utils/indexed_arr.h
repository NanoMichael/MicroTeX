#ifndef INDEXED_ARR_H_INCLUDED
#define INDEXED_ARR_H_INCLUDED

namespace tex {

/**
 * Template to represents 2 dimensions array with N elements for each item and
 * sorted by the first M element.
 */
template <class T, size_t N, size_t M>
class IndexedArray {
private:
  const T* const _raw;
  const size_t _rows;

  IndexedArray() {}

  IndexedArray(const IndexedArray& arr) {}

  int get(int& l, int& h, const int offset, const T& t) {
    while (l <= h) {
      const int m = l + ((h - l) >> 1);
      const int v = *(_raw + (m * N) + offset);
      if (v == t) return m;
      t < v ? h = m - 1 : l = m + 1;
    }
    return -1;
  }

  int last(int l, int h, const int offset, const T& t) {
    int r = -1;
    while (l <= h) {
      int m = l + ((h - l + 1) >> 1);
      int v = *(_raw + (m * N) + offset);

      if (t < v) {
        h = m - 1;
      } else if (t > v) {
        l = m + 1;
      } else {
        r = m;
        l = m + 1;
      }
    }
    return r;
  }

  int first(int l, int h, const int offset, const T& t) {
    int r = -1;
    while (l <= h) {
      int m = l + ((h - l + 1) >> 1);
      int v = *(_raw + (m * N) + offset);

      if (t < v) {
        h = m - 1;
      } else if (t > v) {
        l = m + 1;
      } else {
        r = m;
        h = m - 1;
      }
    }
    return r;
  }

public:
  IndexedArray(const T* arr, int len) : _raw(arr), _rows(len / N) {}

  /** Columns of this array, equal to N */
  inline size_t columns() const {
    return N;
  }

  /** Rows of this array */
  inline size_t rows() const {
    return _len / N;
  }

  /** Find the row by the given keys */
  const T* operator()(const T args[M]) {
    int l = 0, h = _rows;
    for (int i = 0; i < M; i++) {
      const int r = get(l, h, i, args[i]);
      if (r < 0) return nullptr;
      if (i == M - 1) return _raw + (r * N);
      int v, t;
      for (int j = i + 1; j < M; j++) {
        v = *(_raw + (r * N) + j);
        t = args[j];
        if (v != t) break;
      }
      if (t < v) {
        h = r - 1;
        l = first(l, h, i, args[i]);
      } else if (t > v) {
        l = r + 1;
        h = last(l, h, i, args[i]);
      } else {
        return _raw + (r * N);
      }
    }
    return nullptr;
  }
};

}  // namespace tex

#endif
