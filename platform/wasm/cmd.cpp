#include "cmd.h"
#include "utils/log.h"

#include <cstdlib>

namespace tex {

Cmds::Cmds() {
  data = malloc(CHUNK_SIZE);
  // the first 4 bytes are the bytes size used
  index = 4;
  capacity = CHUNK_SIZE;
}

void Cmds::ensureCapacity(len_t required) {
  if (index + required < capacity) {
    return;
  }
#ifdef HAVE_LOG
  logv("grow buffer, capacity: %lu, to be add: %lu\n", capacity, required);
#endif
  data = realloc(data, capacity + CHUNK_SIZE);
  capacity += CHUNK_SIZE;
}

void* Cmds::finish() {
#ifdef HAVE_LOG
  logv("finish drawing commands, bytes: %lu\n", index);
#endif
  auto ptr = (unsigned int*) data;
  *ptr = index;
  return data;
}

}
