#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED

#include <exception>
#include <string>
#include <utility>

namespace microtex {

/** Superclass of all the possible exceptions that can be thrown */
class ex_tex : public std::exception {
private:
  const std::string _msg;

public:
  explicit ex_tex(std::string msg) : _msg(std::move(msg)) {}

  explicit ex_tex(const std::string& msg, const exception& cause)
      : _msg(msg + "\n caused by: " + cause.what()) {}

  const char* what() const noexcept override { return _msg.c_str(); }
};

/** Error occurred while parsing a string to a formula */
class ex_parse : public ex_tex {
public:
  explicit ex_parse(const std::string& msg, const exception& cause) : ex_tex(msg, cause) {}

  explicit ex_parse(const std::string& msg) : ex_tex(msg) {}
};

/** Error occurred while conditions not satisfied */
class ex_invalid_state : public ex_tex {
public:
  explicit ex_invalid_state(const std::string& e) : ex_tex(e) {}
};

/** Error occurred while an invalid param was given */
class ex_invalid_param : public ex_tex {
public:
  explicit ex_invalid_param(const std::string& e) : ex_tex(e) {}
};

class ex_file_not_found : public ex_tex {
public:
  explicit ex_file_not_found(const std::string& e) : ex_tex(e) {}
};

class ex_eof : public ex_tex {
public:
  explicit ex_eof(const std::string& msg) : ex_tex(msg) {}
};

class ex_unprivileged : public ex_tex {
public:
  explicit ex_unprivileged(const std::string& msg) : ex_tex(msg) {}
};

}  // namespace microtex

#endif
