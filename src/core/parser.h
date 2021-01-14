#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "atom/atom.h"
#include "common.h"

#include <set>
#include <string>

namespace tex {

/**
 * Convert a character to roman-number if it is a digit localized
 * @param c character to be converted
 */
wchar_t convertToRomanNumber(wchar_t c);

class TeXFormula;

/**
 * This class implements a parser for latex formulas
 */
class TeXParser {
private:
  std::wstring _parseString;
  int _pos, _spos, _len;
  int _line, _col;
  int _group;
  int _atIsLetter;
  bool _insertion;
  bool _arrayMode;
  bool _ignoreWhiteSpace;
  bool _isPartial;
  bool _hideUnknownChar;

  /**
   * escape character
   */
  static const wchar_t ESCAPE;
  /**
   * grouping characters (for parsing)
   */
  static const wchar_t L_GROUP;
  static const wchar_t R_GROUP;
  static const wchar_t L_BRACK;
  static const wchar_t R_BRACK;
  static const wchar_t DOLLAR;
  static const wchar_t DQUOTE;
  /**
   * percent char for comments
   */
  static const wchar_t PERCENT;
  /**
   * script character (for parsing)
   */
  static const wchar_t SUB_SCRIPT;
  static const wchar_t SUPER_SCRIPT;
  static const wchar_t PRIME;
  static const wchar_t PRIME_UTF;
  static const wchar_t BACKPRIME;
  static const wchar_t DEGRE;
  /**
   * scripts to formula map
   */
  static const std::map<wchar_t, char> SUP_SCRIPT_MAP;
  static const std::map<wchar_t, char> SUB_SCRIPT_MAP;

  static const std::set<std::wstring> _unparsedContents;

  /**
   * Preprocess parse string
   */
  void firstpass();

  sptr<Atom> getScripts(wchar_t f);

  std::wstring getCommand();

  sptr<Atom> processEscape();

  void insert(int beg, int end, const std::wstring& formula);

  /**
   * Return a string with command, options and arguments.
   *
   * @param command
   *      name of command
   * @return command with arguments string with format: \cmd[opt][...]{arg}{...}
   */
  std::wstring getCommandWithArgs(const std::wstring& command);

  /**
   * Process the given TeX command (by parsing following command
   * arguments in the parse string).
   */
  sptr<Atom> processCommands(const std::wstring& cmd);

  void skipWhiteSpace();

  /**
   * Replace the script-characters with command.
   */
  bool replaceScript();

  void preprocess(std::wstring& cmd, std::vector<std::wstring>& args, int& pos);

  void preprocessNewCmd(std::wstring& cmd, std::vector<std::wstring>& args, int& pos);

  void inflateNewCmd(std::wstring& cmd, std::vector<std::wstring>& args, int& pos);

  void inflateEnv(std::wstring& cmd, std::vector<std::wstring>& args, int& pos);

  void init(
      bool ispartial,
      const std::wstring& parsestring,
      _out_ TeXFormula* formula,
      bool firstpass);

public:
  static bool _isLoading;

  TeXFormula* _formula;

  TeXParser() {
    init(true, L"", nullptr, false);
  }

  /**
   * Create a new TeXParser
   * @param latex
   *      the string to be parsed
   * @param formula
   *      the formula where to put the atoms
   * @throw ex_parse
   *      if the string could not be parsed correctly
   */
  TeXParser(const std::wstring& latex, _out_ TeXFormula* formula) {
    init(true, latex, formula, true);
  }

  /**
   * Create a new TeXParser
   * @param isPartial
   *      if true certain exceptions are not thrown
   * @param latex
   *      the string to be parsed
   * @param formula
   *      the formula where to put the atoms
   * @throw ex_parse
   *      if the string could not be parsed correctly
   */
  TeXParser(bool isPartial, const std::wstring& latex, _out_ TeXFormula* formula) {
    init(isPartial, latex, formula, true);
  }

  /**
   * Create a new TeXParser with or without a first pass
   * @param isPartial
   *      if true certain exceptions are not thrown
   * @param latex
   *      the string to be parsed
   * @param formula
   *      the formula where to put the atoms
   * @param firstPass
   *      a boolean to indicate if the parser must replace the user-defined macros by their content
   * @throw ex_parse
   *      if the string could not be parsed correctly
   */
  TeXParser(
      bool isPartial,
      const std::wstring& latex,
      _out_ TeXFormula* formula,
      bool firstPass) {
    init(isPartial, latex, formula, firstPass);
  }

  /**
   * Create a new TeXParser with or without a first pass
   * @param latex
   *      the string to be parsed
   * @param formula
   *      the formula where to put the atoms
   * @param firstPass
   *      a boolean to indicate if the parser must replace the user-defined macros by their content
   * @throw ex_parse
   *      if the string could not be parsed correctly
   */
  TeXParser(const std::wstring& latex, _out_ TeXFormula* formula, bool firstPass) {
    init(true, latex, formula, firstPass);
  }

  /**
   * Create a new TeXParser which ignores or not the white spaces, it's useful
   * for mbox command
   *
   * @param isPartial
   *      if true certains exceptions are not thrown
   * @param latex
   *      the string to be parsed
   * @param formula
   *      the formula to hold the atoms
   * @param firstpass
   *      a boolean to indicate if the parser must replace the
   *      user-defined macros by their content
   * @param space
   *      a boolean to indicate if the parser must ignore or not the
   *      white space
   * @throw ex_parse
   *      if the string could not be parsed correctly
   */
  TeXParser(
      bool isPartial,
      const std::wstring& latex,
      _out_ TeXFormula* formula,
      bool firstpass,
      bool ignoreWhiteSpace) {
    init(isPartial, latex, formula, firstpass);
    _ignoreWhiteSpace = ignoreWhiteSpace;
  }

  /**
   * Create a new TeXParser which ignores or not the white spaces, it's useful
   * for mbox command
   *
   * @param latex
   *      the string to be parsed
   * @param formula
   *      the formula to hold the atoms
   * @param firstpass
   *      a boolean to indicate if the parser must replace the
   *      user-defined macros by their content
   * @param space
   *      a boolean to indicate if the parser must ignore or not the
   *      white space
   * @throw ex_parse
   *      if the string could not be parsed correctly
   */
  TeXParser(
      const std::wstring& latex,
      _out_ TeXFormula* formula,
      bool firstpass,
      bool ignoreWhiteSpace) {
    init(true, latex, formula, firstpass);
    _ignoreWhiteSpace = ignoreWhiteSpace;
  }

  /**
   * Reset the parser with a new latex expression
   */
  void reset(const std::wstring& latex);

  /**
   * Return true if we get a partial formula
   */
  inline bool getIsPartial() const {
    return _isPartial;
  }

  /**
   * Get the number of the current line
   */
  inline int getLine() const {
    return _line;
  }

  /**
   * Get the number of the current column
   */
  inline int getCol() const {
    return _pos - _col - 1;
  }

  /**
   * Get and remove the last atom of the current formula
   */
  sptr<Atom> popLastAtom();

  /**
   * Get and remove the atom represented by the current formula
   */
  sptr<Atom> getFormulaAtom();

  /**
   * Put an atom in the current formula
   */
  void addAtom(const sptr<Atom>& atom);

  /**
   * Indicate if the character @ can be used in the command's name
   */
  inline void makeAtLetter() {
    _atIsLetter++;
  }

  /**
   * Indicate if the character @ can be used in the command's name
   */
  inline void makeAtOther() {
    _atIsLetter--;
  }

  /**
   * Test if the character @ is considered as a letter or not
   */
  inline bool isAtLetter() {
    return (_atIsLetter != 0);
  }

  /**
   * Test if the parser is used to parse an array or not
   */
  inline bool isArrayMode() const {
    return _arrayMode;
  }

  inline void setArrayMode(bool arrayMode) {
    _arrayMode = arrayMode;
  }

  /**
   * Test if the parser must ignore white spaces
   */
  inline bool isIgnoreWhiteSpace() const {
    return _ignoreWhiteSpace;
  }

  /**
   * Test if the parser is in math mode
   */
  inline bool isMathMode() const {
    return _ignoreWhiteSpace;
  }

  /**
   * Return the current position in the parsed string
   */
  inline int getPos() const {
    return _pos;
  }

  /**
   * Rewind the current parsed string
   *
   * @param n
   *      the number of character to be rewinded
   * @return the new position in the parsed string
   */
  inline int rewind(int n) {
    _pos -= n;
    return _pos;
  }

  inline void finish() {
    _pos = _parseString.size();
  }

  std::wstring forwardFromCurrentPos();

  /**
   * Add a new row when the parser is in array mode
   *
   * @throw ex_parse
   *      if the parser is not in array mode
   */
  void addRow();

  /**
   * Parse the input string
   * 
   * @throw ex_parse
   *      if an error is encountered during parse
   */
  void parse();

  /**
   * Get the contents between two delimiters
   *
   * @param openClose
   *      the opening and closing character (such as $)
   * @return the enclosed contents
   * @throw ex_parse
   *      if the contents are badly enclosed
   */
  std::wstring getDollarGroup(wchar_t openClose);

  /**
   * Get the contents between two delimiters
   *
   * @param open
   *      the opening character
   * @param close
   *      the closing character
   * @return the enclosed contents
   * @throw ex_parse
   *      if the contents are badly enclosed
   */
  std::wstring getGroup(wchar_t open, wchar_t close);

  /**
   * Get the contents between two strings as in \\begin{foo}... \\end{foo}
   *
   * @param open
   *      the opening string
   * @param close
   *      the closing string
   * @return the enclosed contents
   * @throw ex_parse
   *      if the contents are badly enclosed
   */
  std::wstring getGroup(const std::wstring& open, const std::wstring& close);

  /**
   * Get the argument of a command in his atomic format
   *
   * @return the corresponding atom
   * @throw ex_parse
   *      if the argument is incorrect
   */
  sptr<Atom> getArgument();

  /**
   * Get the supscript argument
   */
  std::wstring getOverArgument();

  /**
   * Get the unit and length from given string. The string must be in the format: a digital
   * following with the unit (e.g. 10px, 1cm, 8.2em, ...) or a pair (UNIT_PIXEL, 0) will be
   * returned.
   */
  std::pair<int, float> getLength();

  /**
   * Convert a character in the corresponding atom in using the file
   * TeXFormulaSettings.xml for non-alphanumeric characters
   *
   * @param c
   *      the character to be converted
   * @param onChar
   *      if convert only one char in the parse string
   * @return the corresponding atom
   * @throw ex_parse
   *      if the character is unknown
   */
  sptr<Atom> convertCharacter(wchar_t c, bool oneChar);

  /**
   * Get the arguments and the options of a command
   *
   * @param nbArgs
   *      the number of arguments of the command
   * @param opts
   *      must be 1 if the options are found before the first argument,
   *      and must be 2 if they must be found before the second argument,
   *      otherwise there're no options for the command.
   * @param args
   *      a vector to put with argument strings
   */
  void getOptsArgs(int nbArgs, int opts, _out_ std::vector<std::wstring>& args);

  /**
   * Test the validity of the name of a command. It must contains only alpha
   * characters and eventually a @ if makeAtletter activated
   *
   * @param cmd
   *      the command's name
   * @return the validity of the name
   */
  bool isValidName(const std::wstring& cmd);

  /**
   * Test the validity of a character in a command. It must contains only
   * alpha characters and eventually a @ if makeAtletter activated
   */
  inline bool isValidCharacterInCommand(wchar_t ch) const {
    return isalpha(ch) || (_atIsLetter != 0 && ch == '@');
  }
};

}  // namespace tex

#endif  // PARSER_H_INCLUDED
