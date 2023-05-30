#include "samples.h"

#include <algorithm>
#include <fstream>

using namespace std;
using namespace microtex;

void Samples::readSamples(const string& path) {
  string line;
  string sample;
  std::ifstream f(path);
  if (f.is_open()) {
    while (getline(f, line)) {
      // clang-format off
      if (!line.empty() &&
          !isSpace(line) &&
          std::all_of(line.begin(), line.end(), [](char c) { return c == '%'; })
        ) {
        // clang-format on
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

void Samples::add(const string& str) {
  if (str.empty()) return;
  if (isSpace(str)) return;
  _samples.push_back(str);
}

bool Samples::isSpace(const string& str) {
  return std::all_of(str.begin(), str.end(), [](char c) {
    // FIXME
    // avoid the disgusting Windows assertion `c > -1 && c < 255`
    // return isspace(c);
    return c == ' ' || c == '\r' || c == '\n';
  });
}

const std::string& Samples::next() {
  if (_index >= _samples.size()) _index = 0;
  const std::string& x = _samples[_index];
  _index++;
  return x;
}