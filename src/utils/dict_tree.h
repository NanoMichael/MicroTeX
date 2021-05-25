#ifndef DICT_TREE_INCLUDED
#define DICT_TREE_INCLUDED

#include "utils/utils.h"

namespace tex {

template<typename K, typename V>
struct SortedDictTree {
private:
  K _key;
  V _value;
  u16 _childCount;
  /** Children sorted by _key, MUST NOT BE NULL */
  SortedDictTree<K, V>** _children;

public:
  no_copy_assign(SortedDictTree);

  SortedDictTree() = delete;

  SortedDictTree(const K& key, const V& value, u16 childCount)
    : _key(key),
      _value(value),
      _childCount(childCount),
      _children(childCount == 0 ? nullptr : new SortedDictTree<K, V>* [childCount]) {}

  inline K key() const { return _key; }

  inline V value() const { return _value; }

  inline u16 childCount() const { return _childCount; }

  /** Get child at the given index. */
  inline SortedDictTree<K, V>*& child(u16 i) const {
    return _children[i];
  }

  /**
   * Get the child match the given key or null if not found.
   * 
   * @param key the key to match
   */
  const SortedDictTree<K, V>* operator[](const K& key) const {
    const int index = binIndexOf(_childCount, [&](int i) { return key - _children[i]->_key; });
    return index < 0 ? nullptr : _children[index];
  }

  bool operator==(const SortedDictTree<K, V>& b) const {
    if (&b == this) return true;
    bool same = _key == b._key && _value == b._value && _childCount == b._childCount;
    if (!same) return false;
    for (u16 i = 0; i < _childCount; i++) {
      same &= *child(i) == *(b.child(i));
      if (!same) return false;
    }
    return true;
  }

  bool operator!=(const SortedDictTree<K, V>& b) const {
    return !(*this == b);
  }

  ~SortedDictTree<K, V>() {
    for (u16 i = 0; i < _childCount; i++) delete _children[i];
    delete[] _children;
  }
};

}  // namespace tex

#endif
