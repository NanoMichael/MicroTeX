#include "config.h"

#ifdef CLATEX_USE_SAMPLES

#ifndef SAMPLES_H_INCLUDED
#define SAMPLES_H_INCLUDED

#include <algorithm>
#include <fstream>
#include "latex.h"
#include "utils/utf.h"

namespace tex {

using namespace std;

class Samples {
private:
  int _index;
  vector<std::string> _samples;

  void readSamples(const string& path) {
    string line;
    string sample;
    std::ifstream f(path);
    if (f.is_open()) {
      while (getline(f, line)) {
        if (!line.empty() &&
            !isSpace(line) &&
            std::all_of(line.begin(), line.end(), [](char c) { return c == '%'; })
          ) {
          add(sample);
          sample = "";
        } else {
          if (!line.empty() && !isSpace(line)) sample += line + "\n";
        }
      }
      f.close();
    }
    add(sample);
  }

  void add(const string& str) {
    if (str.empty()) return;
    if (isSpace(str)) return;
    _samples.push_back(str);
  }

  bool isSpace(const string& str) {
    return std::all_of(str.begin(), str.end(), [](char c) { return isspace(c); });
  }

public:
  Samples(const string& file) : _index(0) { readSamples(file); }

  const std::string& next() {
    if (_index >= _samples.size()) _index = 0;
    const std::string& x = _samples[_index];
    _index++;
    return x;
  }

  int count() const {
    return _samples.size();
  }
};

}  // namespace tex

#endif  // SAMPLES_H_INCLUDED
#endif  // CLATEX_USE_SAMPLES
