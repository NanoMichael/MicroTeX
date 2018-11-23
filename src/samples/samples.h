#include "config.h"

#ifdef __USE_SAMPLES

#ifndef SAMPLES_H_INCLUDED
#define SAMPLES_H_INCLUDED

#include <algorithm>
#include <fstream>
#include <iostream>
#include "latex.h"

namespace tex {

class Samples {
private:
    int _index;
    vector<wstring> _samples;

    void readSamples() {
        const string path = LaTeX::getResRootPath() + "/SAMPLES.tex";
        string line = "";
        string sample = "";
        std::ifstream f(path);
        if (f.is_open()) {
            while (getline(f, line)) {
                if (!line.empty() &&
                    !isSpace(line) &&
                    std::all_of(line.begin(), line.end(), [](char c) { return c == '%'; })) {
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
        std::cout << str << std::endl;
        _samples.push_back(utf82wide(str.c_str()));
    }

    bool isSpace(const string& str) {
        return std::all_of(str.begin(), str.end(), [](char c) { return isspace(c); });
    }

public:
    Samples() : _index(0) { readSamples(); }

    const wstring& next() {
        if (_index >= _samples.size()) _index = 0;
        const wstring& x = _samples[_index];
        _index++;
        return x;
    }
};

}  // namespace tex

#endif  // SAMPLES_H_INCLUDED
#endif  // __USE_SAMPLES
