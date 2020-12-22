#ifndef CLM_INCLUDED
#define CLM_INCLUDED

#include <cstdio>

#include "openmath/otf.h"

namespace tex {

class BinaryFileReader final {
private:
  constexpr static const uint32 CHUNK_SIZE = 10 * 1024;
  const char* _filePath;
  FILE* _file;
  uint8 _buff[CHUNK_SIZE];
  uint32 _currentSize;
  uint32 _index;
  bool _eof;

  void readChunk(uint32 remain = 0);

  const uint8* read(uint32 count);

public:
  BinaryFileReader(const char* filePath);

  uint8 readUInt8();

  uint16 readUInt16();

  int16 readInt16();

  uint32 readUInt32();

  int32 readInt32();

  ~BinaryFileReader();
};

/** Read a OTFFont from `.clm` file. */
class CLMReader final {
private:
  BinaryFileReader _reader;

public:
  CLMReader(const char* filePath);

  OTFFont* read() const;
};

}  // namespace tex

#endif
