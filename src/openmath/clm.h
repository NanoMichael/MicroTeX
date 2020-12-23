#ifndef CLM_INCLUDED
#define CLM_INCLUDED

#include <cstdio>

#include "openmath/otf.h"

namespace tex {

class BinaryFileReader;

/** Read a OTFFont from `.clm` file. */
class CLMReader final {
private:
  BinaryFileReader* _reader;

public:
  CLMReader(const char* filePath);

  OTFFont* read() const;
};

}  // namespace tex

#endif
