#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "config.h"

#if defined(HAVE_LOG) && defined(__GNUC__)
#include <cxxabi.h>
#endif

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "utils/log.h"
#include "utils/nums.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

using namespace std;

/** OUT parameter, declaring that the function may has side effect on this parameter. */
#define _out_
/** IN parameter, declaring that the function will never has side effect on this parameter. */
#define _in_

namespace tex {

/** The root directory of the "TeX resources" (defined in latex.cpp) */
extern string RES_BASE;

/** Type alias shared_ptr<T> to sptr<T> */
template <typename T>
using sptr = shared_ptr<T>;

/** Return the real name of the function, class or struct name. */
#ifdef HAVE_LOG
#ifdef __GNUC__
inline string demangle_name(const char* name) {
    int status = -4;
    char* res = abi::__cxa_demangle(name, 0, 0, &status);
    const char* const real_name = status == 0 ? res : name;
    string res_str(real_name);
    if (res != nullptr) {
        free(res);
    }
    return res_str;
}
#else
inline string demangle_name(const char* name) {
    return name;
}
#endif  // __GNUC__
#endif  // HAVE_LOG

/**
 * Find the position of a value in the vector, return -1 if not found
 */
template <class T>
inline int indexOf(const vector<T>& v, const T& x) {
    auto it = find(v.begin(), v.end(), x);
    int i = std::distance(v.begin(), it);
    return (i >= v.size() ? -1 : i);
}

/***************************************************************************************************
 *                                        exceptions                                               *
 ***************************************************************************************************/

/**
 * Superclass of all the possible TeX exceptions that can be thrown
 */
class ex_tex : public exception {
private:
    const string _msg;

public:
    explicit ex_tex(const string& msg) : _msg(msg) {}

    explicit ex_tex(const string& msg, const exception& cause)
        : _msg(msg + "\n caused by: " + cause.what()) {}

    const char* what() const throw() override {
        return _msg.c_str();
    }
};

/**
 * Signals that an error occurred while loading the necessary resources into
 * memory.
 */
class ex_res_parse : public ex_tex {
public:
    explicit ex_res_parse(const string& msg)
        : ex_tex(msg) {}

    explicit ex_res_parse(const string& msg, const exception& cause)
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
        const string& resName,
        const string& elName,
        const string& attrName,
        const string& msg)
        : ex_res_parse(
              resName + ": invalid <" + elName + ">-element found: attribute '" +
              attrName + "' " + msg) {}

    /**
     * attribute problem
     */
    explicit ex_xml_parse(
        const string& resName,
        const string& elName,
        const string& attrName,
        const string& msg,
        const exception& cause)
        : ex_res_parse(
              resName + ": invalid <" + elName + ">-element found: attribute '" +
                  attrName + "' " + msg,
              cause) {}
    /**
     * other exceptions
     */
    explicit ex_xml_parse(const string& resName, const exception& e) : ex_res_parse(resName, e) {}

    /**
     * required element missing
     */
    explicit ex_xml_parse(const string& resName, const string& elName)
        : ex_res_parse(
              resName + ": the required <" + elName + ">-elment not found!") {}

    explicit ex_xml_parse(const string& msg) : ex_res_parse(msg) {}
};

/**
 * Exception while registering an alphabet
 */
class ex_alphabet_registration : public ex_tex {
public:
    explicit ex_alphabet_registration(const string& msg) : ex_tex(msg) {}
};

/**
 * Exception while no character-to-delimiter mapping
 */
class ex_delimiter_mapping_not_found : public ex_tex {
public:
    explicit ex_delimiter_mapping_not_found(const char& delimiter)
        : ex_tex("No mapping found for the character '" + string({delimiter}) + "'!") {}
};

class ex_empty_formula : public ex_tex {
public:
    explicit ex_empty_formula() : ex_tex("Illegal operation with an empty formula!") {}
};

class ex_font_loaded : public ex_tex {
public:
    explicit ex_font_loaded(const string& msg) : ex_tex(msg) {}
};

/**
 * Unknown predefined TeXFormula name was used
 */
class ex_formula_not_found : public ex_tex {
public:
    explicit ex_formula_not_found(const string& name)
        : ex_tex("There's no predefined formula with the name '" + name) {}
};

/**
 * Unknown atom type
 */
class ex_invalid_atom_type : public ex_tex {
public:
    explicit ex_invalid_atom_type(const string& msg) : ex_tex(msg) {}
};

/**
 * Unknown delimiter
 */
class ex_invalid_delimiter : public ex_tex {
public:
    explicit ex_invalid_delimiter(const string& symbolName)
        : ex_tex("The symbol with the name '" + symbolName +
                 "' is not defined as a delimiter") {}

    explicit ex_invalid_delimiter(const char& ch, const string& symbolName)
        : ex_tex(
              "The character '" + string({ch}) + "' is not mapped to a symbol with the name '" +
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
    explicit ex_invalid_matrix(const string& msg) : ex_tex(msg) {}
};

/**
 * Invalid symbol
 */
class ex_invalid_symbol_type : public ex_tex {
public:
    explicit ex_invalid_symbol_type(const string& msg) : ex_tex(msg) {}
};

/**
 * Invalid TeXFormula
 */
class ex_invalid_formula : public ex_tex {
public:
    explicit ex_invalid_formula(const string& msg) : ex_tex(msg) {}
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
    explicit ex_parse(const string& msg, const exception& cause) : ex_tex(msg, cause) {}

    explicit ex_parse(const string& msg) : ex_tex(msg) {}
};

/**
 * Symbol mapping not found
 */
class ex_symbol_mapping_not_found : public ex_tex {
public:
    explicit ex_symbol_mapping_not_found(const string& symbolName)
        : ex_tex("No mapping found for the symbol '" + symbolName + "'!") {}
};

/**
 * Unknown symbol
 */
class ex_symbol_not_found : public ex_tex {
public:
    explicit ex_symbol_not_found(const string& name)
        : ex_tex("There's no symbol with the name '" + name + "' defined.") {}
};

/**
 * Text style mapping not found
 */
class ex_text_style_mapping_not_found : public ex_tex {
public:
    explicit ex_text_style_mapping_not_found(const string& name)
        : ex_tex("No mapping found for the text style '" + name + "'!") {}
};

class ex_invalid_state : public ex_tex {
public:
    explicit ex_invalid_state(const string& e) : ex_tex(e) {}
};

class ex_invalid_param : public ex_tex {
public:
    explicit ex_invalid_param(const string& e) : ex_tex(e) {}
};

/***************************************************************************************************
 *                                      TeXConstants                                               *
 ***************************************************************************************************/

enum TeXConstants {

    /********************************* alignment constants ****************************************/
    /**
     * Extra space will be added to the right of the formula
     */
    ALIGN_LEFT = 0,
    /**
     * Extra space will be added to the left of the formula
     */
    ALIGN_RIGHT,  // =1
    /**
     * The formula will be centered in the middle. this constant
     * can be used for both horizontal and vertical alignment
     */
    ALIGN_CENTER,  // =2
    /**
     * Extra space will be added under the formula
     */
    ALIGN_TOP,  // =3
    /**
     * Extra space will be added above the formula
     */
    ALIGN_BOTTOM,  // =4
    /**
     * None
     */
    ALIGN_NONE,  // =5

    /********************************** space size constants **************************************/
    THINMUSKIP = 1,
    MEDMUSKIP = 2,
    THICKMUSKIP = 3,
    NEGTHINMUSKIP = -1,
    NEGMEDMUSKIP = -2,
    NEGTHICKMUSKP = -3,

    QUAD = 3,

    /****************************** script display type constants *********************************/
    SCRIPT_NORMAL = 0,
    SCRIPT_NOLIMITS,
    SCRIPT_LIMITS,

    /*********************************** atom type constants **************************************/
    /**
     * Ordinary symbol, e.g. "slash"
     */
    TYPE_ORDINARY = 0,
    /**
     * Big operator, e.g. "sum"
     */
    TYPE_BIG_OPERATOR,  // =1
    /**
     * Binary operator, e.g. "plus"
     */
    TYPE_BINARY_OPERATOR,  // =2
    /**
     * Relation, e.g. "equals"
     */
    TYPE_RELATION,  // =3
    /**
     * Opening symbol, e.g. "lbrace"
     */
    TYPE_OPENING,  // =4
    /**
     * Closing symbol, e.g. "rbrace"
     */
    TYPE_CLOSING,  // =5
    /**
     * Punctuation symbol, e.g. "comma"
     */
    TYPE_PUNCTUATION,  // =6
    /**
     * Atom type: inner atom (NOT FOR SYMBOLS)
     */
    TYPE_INNER,  // =7
    /**
     * Accent, e.g. "hat"
     */
    TYPE_ACCENT = 10,
    /**
     * Inter-text in matrix environment
     */
    TYPE_INTERTEXT,  // =11
    /**
     * Multi-column in matrix envrionment
     */
    TYPE_MULTICOLUMN,  // =12
    /**
     * Horizontal line in matrix envrionment
     */
    TYPE_HLINE,  // =13
    /**
     * Multi-row in matrix envrionment
     */
    TYPE_MULTIROW,  // =14

    /****************************** over and under delimiter type constants ***********************/
    DELIM_BRACE = 0,
    DELIM_SQUARE_BRACKET,
    DELIM_BRACKET,
    DELIM_LEFT_ARROW,
    DELIM_RIGHT_ARROW,
    DELIM_LEFT_RIGHT_ARROW,
    DELIM_DOUBLE_LEFT_ARROW,
    DELIM_DOUBLE_RIGHT_ARROW,
    DELIM_DOUBLE_LEFT_RIGHT_ARROW,
    DELIM_SIGNLE_LINE,
    DELIM_DOUBLE_LINE,

    /************************************* TeX style constants ************************************/
    /**
     * Display style
     * @par
     * The larger versions of big operators are used and limits are placed under
     * and over these operators (default). Symbols are rendered in the largest
     * size.
     */
    STYLE_DISPLAY = 0,
    /**
     * Text style
     * @par
     * The small versions of big operator are used and limits are attached to
     * these operators as scripts (default). The same size as in the display
     * style is used to render symbols.
     */
    STYLE_TEXT = 2,
    /**
     * Script style
     * @par
     * The same as the the text style, but symbols are rendered in a smaller size.
     */
    STYLE_SCRIPT = 4,
    /**
     * Script_script style
     * @par
     * The same as the script style, but symbols are rendered in a smaller size.
     */
    STYLE_SCRIPT_SCRIPT = 6,

    /************************************ TeX unit constants **************************************/
    /**
     * 1 em = the width of the capital 'M' in the current font
     */
    UNIT_EM = 0,
    /**
     * 1 ex = the height of the character 'x' in the current font
     */
    UNIT_EX,
    UNIT_PIXEL,
    /**
     * postscript point
     */
    UNIT_POINT,
    /**
     * 1 pica = 12 point
     */
    UNIT_PICA,
    /**
     * 1 mu = 1/18 em (em taken from the "mufont")
     */
    UNIT_MU,
    /**
     * 1 cm = 28.346456693 point
     */
    UNIT_CM,
    /**
     * 1 mm = 2.8346456693 point
     */
    UNIT_MM,
    /**
     * 1 in = 72 point
     */
    UNIT_IN,
    /**
     * 1 sp = 65536 point
     */
    UNIT_SP,
    UNIT_PT,
    UNIT_DD,
    UNIT_CC,
    /**
     * 1 x8 = 1 default rule thickness
     */
    UNIT_X8
};

}  // namespace tex

#endif  // COMMON_H_INCLUDED
