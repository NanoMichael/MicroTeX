#include "openmath/clm.h"

#include <cstring>

#include "utils/exceptions.h"

namespace tex {

class BinaryFileReader final {
private:
  constexpr static const uint32 CHUNK_SIZE = 10 * 1024;
  FILE* _file;
  uint8 _buff[CHUNK_SIZE];
  uint32 _currentSize = 0;
  uint32 _index = 0;
  bool _eof = false;

  void readChunk(uint32 remain = 0) {
    if (_eof) return;
    const auto size = CHUNK_SIZE - remain;
    memcpy(_buff, _buff + size, remain);
    auto read = fread(_buff + remain, 1, size, _file);
    if (read < size) _eof = true;
    _currentSize = read + remain;
    _index = 0;
  }

public:
  BinaryFileReader(const char* filePath) {
    _file = fopen(filePath, "rb");
    if (_file == nullptr) {
      throw ex_file_not_found(std::string(filePath) + " cannot be opened.");
    }
  }

  template <typename T>
  T read() {
    const auto bytes = sizeof(T);
    const auto remain = _currentSize - _index;
    if (remain < bytes) readChunk(remain);
    if (_index >= _currentSize) throw ex_eof("");
    const uint8* p = _buff + _index;
    _index += bytes;
    auto shift = bytes - 1;
    T t = 0;
    for (int i = 0; i < bytes; i++) {
      t |= (T)(*(p + i)) << ((shift - i) * 8);
    }
    return t;
  }

  ~BinaryFileReader() {
    if (_file != nullptr) fclose(_file);
  }
};

OTFFont* CLMReader::read() const {
  OTFFont* ptrFont = new OTFFont();

  return ptrFont;
}

}  // namespace tex
