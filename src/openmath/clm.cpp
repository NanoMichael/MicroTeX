#include "openmath/clm.h"

#include <cstring>

#include "utils/exceptions.h"

namespace tex {

BinaryFileReader::BinaryFileReader(const char* filePath) : _filePath(filePath) {
  _file = fopen(filePath, "rb");
  if (_file == nullptr) {
    throw ex_file_not_found(std::string(filePath) + " cannot be opened.");
  }
}

void BinaryFileReader::readChunk(uint32 remain) {
  if (_eof) return;
  memcpy(_buff, _buff + CHUNK_SIZE - remain, remain);
  const auto size = CHUNK_SIZE - remain;
  auto read = fread(_buff + remain, 1, size, _file);
  if (read < size) _eof = true;
  _currentSize = read;
  _index = 0;
}

const uint8* BinaryFileReader::read(uint32 count) {
  const auto remain = _currentSize - _index;
  if (remain < count) readChunk(remain);
  if (_index >= _currentSize) throw ex_eof(_filePath);
  const uint8* p = _buff + _index;
  _index += count;
  return p;
}

uint8 BinaryFileReader::readUInt8() {
  return *read(1);
}

uint16 BinaryFileReader::readUInt16() {
  const uint8* p = read(2);
  return *p << 8 | *(p + 1);
}

OTFFont* CLMReader::read() const {
  OTFFont* ptrFont = new OTFFont();

  return ptrFont;
}

}  // namespace tex
