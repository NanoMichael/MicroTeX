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

using namespace std;

#define ANSI_BOLD "\x1b[1m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_UNDERLINE "\x1b[4m"
#define ANSI_RESET "\x1b[0m"

#define __dbg(format, ...)                                    \
    {                                                         \
        fprintf(                                              \
            stdout,                                           \
            "FILE: " ANSI_COLOR_UNDERLINE                     \
            "%s" ANSI_RESET                                   \
            ", LINE: " ANSI_COLOR_RED                         \
            "%d" ANSI_RESET                                   \
            ", FUNCTION: " ANSI_COLOR_CYAN                    \
            "%s" ANSI_RESET                                   \
            ", MSG: " format,                                 \
            __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    }

#ifdef HAVE_LOG
#include <iostream>
#define __log std::cout
#endif  // HAVE_LOG

#define __print printf

// OUT parameter, the parameter may be affected by this function
#define _out_
// IN parameter, the parameter will never be changed by this function
#define _in_

namespace tex {

/**
 * The root directory of the "TeX resources" (defined in latex.cpp)
 */
extern string RES_BASE;

static const float POS_INF = numeric_limits<float>::infinity();
static const float NEG_INF = -POS_INF;
static const float F_MAX = numeric_limits<float>::max();
static const float F_MIN = -F_MAX;
static const double PI = atan(1.0) * 4;

template <typename T>
using sptr = shared_ptr<T>;

/**
 * For compare with 0.0f, if a value < PREC,  we trade it as 0.0f
 */
static const float PREC = 0.0000001f;

/**
 * Demangling name
 */
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

/**
 * Convert a value to string
 */
template <class T>
inline string tostring(T val) {
    ostringstream os;
    os << val;
    return os.str();
}

/**
 * Convert a value to wide string
 */
template <class T>
inline wstring towstring(T val) {
    wostringstream os;
    os << val;
    return os.str();
}

template <class T>
inline void valueof(const string& s, T& val) {
    stringstream ss;
    ss << s;
    ss >> val;
}

template <class T>
inline void valueof(const wstring& s, T& val) {
    wstringstream ss;
    ss << s;
    ss >> val;
}

inline bool str2int(const string& str, int& res, int radix) {
    char* endptr = nullptr;
    errno = 0;

    const long val = strtol(str.c_str(), &endptr, radix);

    if ((val == LONG_MAX || val == LONG_MIN) && errno == ERANGE)
        return false;

    res = static_cast<int>(val);
    return endptr == str.c_str() + str.size();
}

/**
 * Transform a string to lowercase
 */
inline string& tolower(_out_ string& src) {
    transform(src.begin(), src.end(), src.begin(), ::tolower);
    return src;
}

inline wstring& tolower(_out_ wstring& src) {
    transform(src.begin(), src.end(), src.begin(), ::tolower);
    return src;
}

/**
 * Ignore left side whitespace in a string
 */
inline string& ltrim(string& s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}

/**
 * Ignore right side whitespace in a string
 */
inline string& rtrim(string& s) {
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

/**
 * Ignore left and right side whitespace in a string
 */
inline string& trim(string& s) {
    return ltrim(rtrim(s));
}

/**
 * Split string with specified delimeter
 */
inline void split(const string& str, char del, _out_ vector<string>& res) {
    stringstream ss(str);
    string tok;
    while (getline(ss, tok, del)) res.push_back(tok);
}

inline bool startswith(const string& str, const string& cmp) {
    return str.find(cmp) == 0;
}

inline bool endswith(const string& str, const string& cmp) {
    return str.rfind(cmp) == (str.length() - cmp.length());
}

inline bool startswith(const wstring& str, const wstring& cmp) {
    return str.find(cmp) == 0;
}

inline bool endswith(const wstring& str, const wstring& cmp) {
    return str.rfind(cmp) == (str.length() - cmp.length());
}

/**
 * Split string with delimiter
 */
class strtokenizer {
private:
    string _str;
    string _del;
    bool _ret;
    int _pos;

public:
    strtokenizer(const string& str) {
        _str = str;
        _del = " \t\n\r\f";
        _ret = false;
        _pos = 0;
    }

    strtokenizer(const string& str, const string& del, bool ret = false) {
        _str = str;
        _del = del;
        _ret = ret;
        _pos = 0;
    }

    int count_tokens() {
        int c = 0;
        bool in = false;
        for (int i = _pos, len = _str.length(); i < len; i++) {
            if (_del.find(_str[i]) != string::npos) {
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

    string next_token() {
        int i = _pos;
        int len = _str.length();

        if (i < len) {
            if (_ret) {
                if (_del.find(_str[_pos]) != string::npos)
                    return string({_str[_pos++]});
                for (_pos++; _pos < len; _pos++)
                    if (_del.find(_str[_pos]) != string::npos)
                        return _str.substr(i, _pos - i);
                return _str.substr(i);
            }

            while (i < len && _del.find(_str[i]) != string::npos) i++;

            _pos = i;
            if (i < len) {
                for (_pos++; _pos < len; _pos++)
                    if (_del.find(_str[_pos]) != string::npos)
                        return _str.substr(i, _pos - i);
                return _str.substr(i);
            }
        }
        return "";
    }
};

/**
 * Convert unicode wide string to UTF-8 encoded string
 */
inline string& wide2utf8(const wchar_t* in, _out_ string& out) {
    unsigned int codepoint = 0;
    for (; *in != 0; ++in) {
        if (*in >= 0xd800 && *in <= 0xdbff) {
            codepoint = ((*in - 0xd800) << 10) + 0x10000;
        } else {
            if (*in >= 0xdc00 && *in <= 0xdfff) {
                codepoint |= *in - 0xdc00;
            } else {
                codepoint = *in;
            }

            if (codepoint <= 0x7f) {
                out.append(1, static_cast<char>(codepoint));
            } else if (codepoint <= 0x7ff) {
                out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            } else if (codepoint <= 0xffff) {
                out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            } else {
                out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
                out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
            }
            codepoint = 0;
        }
    }
    return out;
}

inline string wide2utf8(const wchar_t* in) {
    string out;
    return wide2utf8(in, out);
}

/**
 * Convert an UTF-8 encoded char sequence to wide unicode string,
 * the encoding of input char sequence must be known as UTF-8
 */
inline wstring& utf82wide(const char* in, _out_ wstring& out) {
    if (in == nullptr) return out;
    unsigned int codepoint;
    while (*in != 0) {
        unsigned char ch = static_cast<unsigned char>(*in);
        if (ch <= 0x7f)
            codepoint = ch;
        else if (ch <= 0xbf)
            codepoint = (codepoint << 6) | (ch & 0x3f);
        else if (ch <= 0xdf)
            codepoint = ch & 0x1f;
        else if (ch <= 0xef)
            codepoint = ch & 0x0f;
        else
            codepoint = ch & 0x07;
        ++in;
        if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff)) {
            if (codepoint > 0xffff) {
                out.append(1, static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
                out.append(1, static_cast<wchar_t>(0xdc00 + (codepoint & 0x03ff)));
            } else if (codepoint < 0xd800 || codepoint >= 0xe000)
                out.append(1, static_cast<wchar_t>(codepoint));
        }
    }
    return out;
}

inline wstring utf82wide(const char* in) {
    wstring out;
    return utf82wide(in, out);
}

/**
 * Returns a replacement string for the given one that has all backslashes
 * and dollar signs escaped
 */
inline string& quotereplace(const string& src, _out_ string& out) {
    for (size_t i = 0; i < src.length(); i++) {
        char c = src[i];
        if (c == '\\' || c == '$') out.append(1, '\\');
        out.append(1, c);
    }
    return out;
}

inline wstring& quotereplace(const wstring& src, _out_ wstring& out) {
    for (size_t i = 0; i < src.length(); i++) {
        wchar_t c = src[i];
        if (c == L'\\' || c == L'$') out.append(1, L'\\');
        out.append(1, c);
    }
    return out;
}

/**
 * Replace string with specified string in the first
 */
inline string& replacefirst(_out_ string& src, const string& from, const string& to) {
    size_t start = src.find(from);
    if (start == string::npos) return src;
    src.replace(start, from.length(), to);
    return src;
}

inline string& replaceall(_out_ string& src, const string& from, const string& to) {
    if (from.empty()) return src;
    size_t start = 0;
    while ((start = src.find(from, start)) != string::npos) {
        src.replace(start, from.length(), to);
        start += to.length();
    }
    return src;
}

inline wstring& replacefirst(_out_ wstring& src, const wstring& from, const wstring& to) {
    size_t start = src.find(from);
    if (start == wstring::npos) return src;
    src.replace(start, from.length(), to);
    return src;
}

inline wstring& replaceall(_out_ wstring& src, const wstring& from, const wstring& to) {
    if (from.empty()) return src;
    size_t start = 0;
    while ((start = src.find(from, start)) != wstring::npos) {
        src.replace(start, from.length(), to);
        start += to.length();
    }
    return src;
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
