#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "atom/atom.h"
#include "env/units.h"

namespace microtex {

using Args = std::vector<std::string>;

/**
 * Convert a character to roman-number if it is a digit localized
 * @param c character to be converted
 */
c32 convertToRomanNumber(c32 c);

class Formula;

class MacroInfo;

/** This class implements a parser for latex formulas */
class Parser {
private:
  std::string _latex;
  int _pos, _spos, _len;
  int _group;
  int _atIsLetter;
  bool _insertion;
  bool _arrayMode;
  bool _isMathMode;
  bool _isPartial;

  /** Preprocess parse string */
  void preprocess();

  sptr<Atom> getScripts(char first);

  std::string getCmd();

  sptr<Atom> processEscape();

  void insert(int beg, int end, const std::string& formula);

  /**
   * Return a string with command, options and arguments.
   *
   * @param cmd name of command
   * @return command with arguments string with format: \cmd[opt][...]{arg}{...}
   */
  std::string getCmdWithArgs(const std::string& cmd);

  /**
   * Process the given TeX command (by parsing following command
   * arguments in the parse string).
   */
  sptr<Atom> processCmd(const std::string& cmd, MacroInfo* mac);

  void preprocess(std::string& cmd, Args& args, int& pos);

  void preprocessNewCmd(std::string& cmd, Args& args, int& pos);

  void inflateNewCmd(std::string& cmd, Args& args, int& pos);

  void inflateEnv(std::string& cmd, Args& args, int& pos);

  void init(bool isPartial, const std::string& latex, Formula* formula, bool firstPass);

  sptr<Atom> getCharAtom();

public:
  Formula* _formula;

  /**
   * Create a new TeXParser
   * @param latex the string to be parsed
   * @param formula the formula where to put the atoms
   * @throw ex_parse if the string could not be parsed correctly
   */
  Parser(const std::string& latex, Formula* formula) { init(true, latex, formula, true); }

  /**
   * Create a new TeXParser
   * @param isPartial if true certain exceptions are not thrown
   * @param latex the string to be parsed
   * @param formula the formula where to put the atoms
   * @throw ex_parse if the string could not be parsed correctly
   */
  Parser(bool isPartial, const std::string& latex, Formula* formula) {
    init(isPartial, latex, formula, true);
  }

  /**
   * Create a new TeXParser with or without a first pass
   * @param isPartial if true certain exceptions are not thrown
   * @param latex the string to be parsed
   * @param formula the formula where to put the atoms
   * @param preprocess indicate if the parser must replace the user-defined macros by their content
   *
   * @throw ex_parse if the string could not be parsed correctly
   */
  Parser(bool isPartial, const std::string& latex, Formula* formula, bool preprocess) {
    init(isPartial, latex, formula, preprocess);
  }

  /**
   * Create a new TeXParser with or without a first pass
   * @param latex the string to be parsed
   * @param formula the formula where to put the atoms
   * @param preprocess indicate if the parser must replace the user-defined macros by their content
   *
   * @throw ex_parse if the string could not be parsed correctly
   */
  Parser(const std::string& latex, Formula* formula, bool preprocess) {
    init(true, latex, formula, preprocess);
  }

  /**
   * Create a new TeXParser which ignores or not the white spaces, it's useful
   * for mbox command
   *
   * @param isPartial if true certain exceptions are not thrown
   * @param latex the string to be parsed
   * @param formula the formula to hold the atoms
   * @param preprocess indicate if the parser must replace the user-defined macros by their content
   * @param isMathMode a boolean to indicate if the parser must ignore or not the white space
   *
   * @throw ex_parse if the string could not be parsed correctly
   */
  Parser(
    bool isPartial,
    const std::string& latex,
    Formula* formula,
    bool preprocess,
    bool isMathMode
  ) {
    init(isPartial, latex, formula, preprocess);
    _isMathMode = isMathMode;
  }

  void skipWhiteSpace(int count = -1);

  /** Reset the parser with a new latex expression */
  void reset(const std::string& latex);

  /** Return true if we get a partial formula */
  inline bool isPartial() const { return _isPartial; }

  /** Get and remove the last atom of the current formula */
  sptr<Atom> popBack() const;

  /** Get and remove the atom represented by the current formula */
  sptr<Atom> popFormulaAtom() const;

  /** Put an atom in the current formula */
  void addAtom(const sptr<Atom>& atom) const;

  /** Make the character @ as letter that can be used in the command's name */
  inline void makeAtLetter() { _atIsLetter++; }

  /** Make the character @ that can not be used in the command's name */
  inline void makeAtOther() { _atIsLetter--; }

  /** Test if the character @ is considered as a letter or not */
  inline bool isAtLetter() const { return (_atIsLetter != 0); }

  /** Test if the parser is used to parse an array or not */
  inline bool isArrayMode() const { return _arrayMode; }

  /** Test if the parser is in math mode  */
  inline bool isMathMode() const { return _isMathMode; }

  /** Return the current position in the parsed string */
  inline int getPos() const { return _pos; }

  /**
   * Rewind the current parsed string
   *
   * @param n the number of character to be rewind
   * @return the new position in the parsed string
   */
  inline int rewind(int n) {
    _pos -= n;
    return _pos;
  }

  /** Finish the parse process */
  inline void finish() { _pos = _latex.size(); }

  /**
   * Forward from current position to get a balanced group.
   * <li> Forward to the end of the parse string if no group was in process
   * <li> Otherwise get the balanced group embraced by '{' and '}' and forward
   */
  std::string forwardBalancedGroup();

  /** Forward parse while the given function #f returns true. */
  std::string forward(std::function<bool(char)>&& f);

  /**
   * Add a new row when the parser is in array mode
   *
   * @throw ex_parse if the parser is not in array mode
   */
  void addRow() const;

  /**
   * Parse the input string
   *
   * @throw ex_parse if an error is encountered during parse
   */
  void parse();

  /**
   * Get the contents between two delimiters
   *
   * @param openClose the opening and closing character (such as $)
   * @return the enclosed contents
   *
   * @throw ex_parse if the contents are badly enclosed
   */
  std::string getGroup(char openClose);

  /**
   * Get the contents between two delimiters
   *
   * @param open the opening character
   * @param close the closing character
   * @return the enclosed contents
   *
   * @throw ex_parse if the contents are badly enclosed
   */
  std::string getGroup(char open, char close);

  /**
   * Get the contents between two strings as in \\begin{foo}... \\end{foo}
   *
   * @param open the opening string
   * @param close the closing string
   * @return the enclosed contents
   *
   * @throw ex_parse if the contents are badly enclosed
   */
  std::string getGroup(const std::string& open, const std::string& close);

  /**
   * Get the argument of a command in his atomic format
   *
   * @return the corresponding atom
   *
   * @throw ex_parse if the argument is incorrect
   */
  sptr<Atom> getArgument();

  /** Get the supscript argument */
  std::string getOverArgument();

  /**
   * Get the unit and length from given string. The string must be in the format: a digital
   * following with the unit (e.g. 10px, 1cm, 8.2em, ...) or a pair (UnitType::none, 0) will be
   * returned.
   */
  Dimen getDimen();

  /**
   * Convert a character in the corresponding atom
   *
   * @param chr the character to be converted
   * @return the corresponding atom
   */
  sptr<Atom> getCharAtom(c32 chr);

  /** Get the simple scripts from given character */
  sptr<Atom> getSimpleScripts(bool isPrime);

  /**
   * Get the arguments and the options of a command
   *
   * @param argc the number of arguments of the command
   * @param opts
   *   must be 1 if the options are found before the first argument,
   *   and must be 2 if they must be found before the second argument,
   *   otherwise there're no options for the command.
   *
   * @param args a vector to put with argument strings
   */
  void getOptsArgs(int argc, int opts, Args& args);

  /**
   * Test the validity of the name of a command. It must contains only alpha
   * characters and eventually a @ if makeAtletter activated
   *
   * @param cmd the command's name
   * @return the validity of the name
   */
  bool isValidCmd(const std::string& cmd) const;

  /**
   * Test the validity of a character in a command. It must contains only
   * alpha characters and eventually a @ if makeAtletter activated
   */
  bool isValidCharInCmd(char ch) const;
};

}  // namespace microtex

#endif  // PARSER_H_INCLUDED
