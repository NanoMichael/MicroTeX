#include "wrapper/cwrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char** argv) {
  // dummy function call to avoid link optimization
  microtex_isLittleEndian();
  return 0;
}

#ifdef __cplusplus
}
#endif
