#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED

#include <exception>
#include <string>

namespace tex {

/**
 * Superclass of all the possible TeX exceptions that can be thrown
 */
class ex_tex : public std::exception {
private:
  const std::string _msg;

public:
  explicit ex_tex(const std::string& msg) : _msg(msg) {}

  explicit ex_tex(const std::string& msg, const exception& cause)
      : _msg(msg + "\n caused by: " + cause.what()) {}

  const char* what() const noexcept override {
    return _msg.c_str();
  }
};

/**
 * Signals that an error occurred while loading the necessary resources into
 * memory.
 */
class ex_res_parse : public ex_tex {
public:
  explicit ex_res_parse(const std::string& msg)
      : ex_tex(msg) {}

  explicit ex_res_parse(const std::string& msg, const exception& cause)
      : ex_tex(msg, cause) {}
};

/**
 * xml-parse exception
 */
class ex_xml_parse : public ex_res_parse {
public:
  /**
   * attribute problem
   */
  explicit ex_xml_parse(
      const std::string& resName,
      const std::string& elName,
      const std::string& attrName,
      const std::string& msg)
      : ex_res_parse(
            resName + ": invalid <" + elName + ">-element found: attribute '" +
            attrName + "' " + msg) {}

  /**
   * attribute problem
   */
  explicit ex_xml_parse(
      const std::string& resName,
      const std::string& elName,
      const std::string& attrName,
      const std::string& msg,
      const exception& cause)
      : ex_res_parse(
            resName + ": invalid <" + elName + ">-element found: attribute '" +
                attrName + "' " + msg,
            cause) {}
  /**
   * other exceptions
   */
  explicit ex_xml_parse(const std::string& resName, const exception& e) : ex_res_parse(resName, e) {}

  /**
   * required element missing
   */
  explicit ex_xml_parse(const std::string& resName, const std::string& elName)
      : ex_res_parse(
            resName + ": the required <" + elName + ">-elment not found!") {}

  explicit ex_xml_parse(const std::string& msg) : ex_res_parse(msg) {}
};

/**
 * Exception while registering an alphabet
 */
class ex_alphabet_registration : public ex_tex {
public:
  explicit ex_alphabet_registration(const std::string& msg) : ex_tex(msg) {}
};

/**
 * Exception while no character-to-delimiter mapping
 */
class ex_delimiter_mapping_not_found : public ex_tex {
public:
  explicit ex_delimiter_mapping_not_found(const char& delimiter)
      : ex_tex("No mapping found for the character '" + std::string({delimiter}) + "'!") {}
};

class ex_empty_formula : public ex_tex {
public:
  explicit ex_empty_formula() : ex_tex("Illegal operation with an empty formula!") {}
};

class ex_font_loaded : public ex_tex {
public:
  explicit ex_font_loaded(const std::string& msg) : ex_tex(msg) {}
};

/**
 * Unknown predefined TeXFormula name was used
 */
class ex_formula_not_found : public ex_tex {
public:
  explicit ex_formula_not_found(const std::string& name)
      : ex_tex("There's no predefined formula with the name '" + name) {}
};

/**
 * Unknown atom type
 */
class ex_invalid_atom_type : public ex_tex {
public:
  explicit ex_invalid_atom_type(const std::string& msg) : ex_tex(msg) {}
};

/**
 * Unknown delimiter
 */
class ex_invalid_delimiter : public ex_tex {
public:
  explicit ex_invalid_delimiter(const std::string& symbolName)
      : ex_tex("The symbol with the name '" + symbolName +
               "' is not defined as a delimiter") {}

  explicit ex_invalid_delimiter(const char& ch, const std::string& symbolName)
      : ex_tex(
            "The character '" + std::string({ch}) + "' is not mapped to a symbol with the name '" +
            symbolName + "', but that symbol is not defined as a delimiter.") {}
};

/**
 * Unknown delimiter type
 */
class ex_invalid_delimiter_type : public ex_tex {
public:
  explicit ex_invalid_delimiter_type() : ex_tex("The delimiter type was not valid!") {}
};

/**
 * Matrix dimension not valid
 */
class ex_invalid_matrix : public ex_tex {
public:
  explicit ex_invalid_matrix(const std::string& msg) : ex_tex(msg) {}
};

/**
 * Invalid symbol
 */
class ex_invalid_symbol_type : public ex_tex {
public:
  explicit ex_invalid_symbol_type(const std::string& msg) : ex_tex(msg) {}
};

/**
 * Invalid TeXFormula
 */
class ex_invalid_formula : public ex_tex {
public:
  explicit ex_invalid_formula(const std::string& msg) : ex_tex(msg) {}
};

/**
 * Unknown unit constant was used
 */
class ex_invalid_unit : public ex_tex {
public:
  explicit ex_invalid_unit()
      : ex_tex("The unit was not valid! use the unit defined in 'TeXConstants'.") {}
};

/**
 * Error occurred while parsing a string to a formula
 */
class ex_parse : public ex_tex {
public:
  explicit ex_parse(const std::string& msg, const exception& cause) : ex_tex(msg, cause) {}

  explicit ex_parse(const std::string& msg) : ex_tex(msg) {}
};

/**
 * Symbol mapping not found
 */
class ex_symbol_mapping_not_found : public ex_tex {
public:
  explicit ex_symbol_mapping_not_found(const std::string& symbolName)
      : ex_tex("No mapping found for the symbol '" + symbolName + "'!") {}
};

/**
 * Unknown symbol
 */
class ex_symbol_not_found : public ex_tex {
public:
  explicit ex_symbol_not_found(const std::string& name)
      : ex_tex("There's no symbol with the name '" + name + "' defined.") {}
};

/**
 * Text style mapping not found
 */
class ex_text_style_mapping_not_found : public ex_tex {
public:
  explicit ex_text_style_mapping_not_found(const std::string& name)
      : ex_tex("No mapping found for the text style '" + name + "'!") {}
};

class ex_invalid_state : public ex_tex {
public:
  explicit ex_invalid_state(const std::string& e) : ex_tex(e) {}
};

class ex_invalid_param : public ex_tex {
public:
  explicit ex_invalid_param(const std::string& e) : ex_tex(e) {}
};

}  // namespace tex

#endif
