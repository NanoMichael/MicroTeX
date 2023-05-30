#ifdef HAVE_CWRAPPER

#include "wrapper/byte_seq.h"

#include <cstdlib>

#include "utils/log.h"

namespace microtex {

ByteSeq::ByteSeq() {
  _data = malloc(CHUNK_SIZE);
  // the first 4 bytes is the bytes size used
  _index = 4;
  _capacity = CHUNK_SIZE;
}

void ByteSeq::_put(const char* str) {
  auto l = sizeOf(str);
  strcpy((char*)_data + _index, str);
  _index += l;
}

void ByteSeq::ensureCapacity(len_t required) {
  if (_index + required < _capacity) {
    return;
  }
#ifdef HAVE_LOG
  logv("grow buffer, _capacity: %u, to be add: %u\n", _capacity, required);
#endif
  _data = realloc(_data, _capacity + CHUNK_SIZE);
  _capacity += CHUNK_SIZE;
}

void* ByteSeq::finish() {
#ifdef HAVE_LOG
  logv("finish drawing commands, bytes: %u\n", _index);
#endif
  auto ptr = (unsigned int*)_data;
  *ptr = _index;
  return _data;
}

}  // namespace microtex

#endif  // HAVE_CWRAPPER
