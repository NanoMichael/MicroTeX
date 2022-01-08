#include "utils/string_utils.h"

#include <utility>

using namespace std;
using namespace tex;

StrTokenizer::StrTokenizer(string str) : _str(std::move(str)) {
  _del = " \t\n\r\f";
  _ret = false;
  _pos = 0;
}

StrTokenizer::StrTokenizer(string str, string del, bool ret)
  : _str(std::move(str)), _del(std::move(del)), _ret(ret) {
  _pos = 0;
}

int StrTokenizer::count() {
  int c = 0;
  bool in = false;
  for (int i = _pos, len = _str.length(); i < len; i++) {
    if (_del.find(_str[i]) != std::string::npos) {
      if (_ret) c++;
      if (in) {
        c++;
        in = false;
      }
    } else {
      in = true;
    }
  }
  if (in) c++;
  return c;
}

std::string StrTokenizer::next() {
  int i = _pos;
  int len = _str.length();

  if (i < len) {
    if (_ret) {
      if (_del.find(_str[_pos]) != std::string::npos)
        return std::string({_str[_pos++]});
      for (_pos++; _pos < len; _pos++)
        if (_del.find(_str[_pos]) != std::string::npos)
          return _str.substr(i, _pos - i);
      return _str.substr(i);
    }

    while (i < len && _del.find(_str[i]) != std::string::npos) i++;

    _pos = i;
    if (i < len) {
      for (_pos++; _pos < len; _pos++)
        if (_del.find(_str[_pos]) != std::string::npos)
          return _str.substr(i, _pos - i);
      return _str.substr(i);
    }
  }
  return "";
}

map<string, string> tex::parseOption(const string& options) {
  if (options.empty()) return {};

  map<string, string> res;
  StrTokenizer tokens(options, ",");
  const int c = tokens.count();
  for (int i = 0; i < c; i++) {
    std::string tok = tokens.next();
    trim(tok);
    std::vector<std::string> optarg;
    split(tok, '=', optarg);
    if (!optarg.empty()) {
      if (optarg.size() == 2) {
        res[trim(optarg[0])] = trim(optarg[1]);
      } else if (optarg.size() == 1) {
        res[trim(optarg[0])] = "";
      }
    }
  }
  return res;
}
