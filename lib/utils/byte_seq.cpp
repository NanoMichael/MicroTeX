#include "utils/byte_seq.h"
#include "utils/log.h"

#include <cstdlib>

namespace microtex {

ByteSeq::ByteSeq() {
  data = malloc(CHUNK_SIZE);
  // the first 4 bytes is the bytes size used
  index = 4;
  capacity = CHUNK_SIZE;
}

void ByteSeq::ensureCapacity(len_t required) {
  if (index + required < capacity) {
    return;
  }
#ifdef HAVE_LOG
  logv("grow buffer, capacity: %lu, to be add: %lu\n", capacity, required);
#endif
  data = realloc(data, capacity + CHUNK_SIZE);
  capacity += CHUNK_SIZE;
}

void* ByteSeq::finish() {
#ifdef HAVE_LOG
  logv("finish drawing commands, bytes: %lu\n", index);
#endif
  auto ptr = (unsigned int*) data;
  *ptr = index;
  return data;
}

}
