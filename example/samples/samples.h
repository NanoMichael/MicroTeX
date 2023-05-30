#ifndef MICROTEX_SAMPLES_H
#define MICROTEX_SAMPLES_H

#include <string>
#include <vector>

namespace microtex {

class Samples {
private:
  int _index;
  std::vector<std::string> _samples;

  void readSamples(const std::string& path);

  void add(const std::string& str);

  bool isSpace(const std::string& str);

public:
  explicit Samples(const std::string& file) : _index(0) { readSamples(file); }

  const std::string& next();

  inline bool isEmpty() const { return _samples.empty(); }

  inline int count() const { return _samples.size(); }
};

}  // namespace microtex

#endif  // MICROTEX_SAMPLES_H
