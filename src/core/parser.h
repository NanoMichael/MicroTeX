#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "atom/atom.h"
#include "common.h"

#include <string>
#include <set>

using namespace std;

namespace tex {

class TeXFormula;

/**
 * This class implements a parser for latex formulas
 */
class TeXParser {

private:
	wstring _parseString;
	int _pos, _spos, _len;
	int _line, _col;
	int _group;
	int _atIsLetter;
	bool _insertion;
	bool _arrayMode;
	bool _ignoreWhiteSpace;
	bool _isPartial;
	bool _hideUnknownChar;

	/********************* statics *****************/
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
	static const wchar_t BACKPRIME;
	static const wchar_t DEGRE;
	/**
	 * script to formula map
	 */
	static const map<wchar_t, char> SUP_SCRIPT_MAP;
	static const map<wchar_t, char> SUB_SCRIPT_MAP;

	static const set<wstring> _unparsedContents;

	void firstpass() throw(ex_parse);

	shared_ptr<Atom> getScripts(wchar_t f) throw(ex_parse);

	wstring getCommand();

	shared_ptr<Atom> processEscape() throw(ex_parse);

	void insert(int beg, int end, const wstring& formula);

	/**
	 * return a string with command and options and args
	 *
	 * @param command
	 * 		name of command
	 * @return
	 */
	wstring getCommandWithArgs(const wstring& command);

	/**
	 * process the given TeX command (by parsing following command
	 * arguments in the parse string).
	 */
	shared_ptr<Atom> processCommands(const wstring& cmd) throw(ex_parse);

	bool replaceScript();

	void skipWhiteSpace();

	static wchar_t convert2RomanNumber(wchar_t c);

	void init(bool ispartial, const wstring& parsestring, _out_ TeXFormula* formula, bool firstpass) throw(ex_parse);
public:
	static bool _isLoading;

	TeXFormula* _formula;

	TeXParser() {
		init(true, L"", nullptr, false);
	}

	/**
	 * create a new TeXParser
	 * @param latex
	 *      the string to be parsed
	 * @param formula
	 *      the formula where to put the atoms
	 * @throw ex_parse
	 *      if the string could not be parsed correctly
	 */
	TeXParser(const wstring& latex, _out_ TeXFormula* formula) throw(ex_parse) {
		init(true, latex, formula, true);
	}

	/**
	 * create a new TeXParser
	 * @param isPartial
	 *      if true certain exceptions are not thrown
	 * @param latex
	 *      the string to be parsed
	 * @param formula
	 *      the formula where to put the atoms
	 * @throw ex_parse
	 *      if the string could not be parsed correctly
	 */
	TeXParser(bool isPartial, const wstring& latex, _out_ TeXFormula* formula) throw(ex_parse) {
		init(isPartial, latex, formula, true);
	}

	/**
	 * create a new TeXParser with or without a first pass
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
	TeXParser(bool isPartial, const wstring& latex, _out_ TeXFormula* formula, bool firstPass) throw(ex_parse) {
		init(isPartial, latex, formula, firstPass);
	}

	/**
	 * create a new TeXParser with or without a first pass
	 * @param latex
	 *      the string to be parsed
	 * @param formula
	 *      the formula where to put the atoms
	 * @param firstPass
	 *      a boolean to indicate if the parser must replace the user-defined macros by their content
	 * @throw ex_parse
	 *      if the string could not be parsed correctly
	 */
	TeXParser(const wstring& latex, _out_ TeXFormula* formula, bool firstPass) throw(ex_parse) {
		init(true, latex, formula, firstPass);
	}

	/**
	 * Create a new TeXParser which ignores or not the white spaces, it's useful
	 * for mbox command
	 *
	 * @param isPartial
	 * 		if true certains exceptions are not thrown
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
	TeXParser(bool isPartial, const wstring& latex, _out_ TeXFormula* formula, bool firstpass, bool space) throw(ex_parse) {
		init(isPartial, latex, formula, firstpass);
		_ignoreWhiteSpace = space;
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
	TeXParser(const wstring& latex, _out_ TeXFormula* formula, bool firstpass, bool space) throw(ex_parse) {
		init(true, latex, formula, firstpass);
		_ignoreWhiteSpace = space;
	}

	/**
	 * reset the parser with a new latex expression
	 */
	void reset(const wstring& latex);

	/**
	 * return true if we get a partial formula
	 */
	inline bool getIsPartial() const {
		return _isPartial;
	}

	/**
	 * get the number of the current line
	 */
	inline int getLine() const {
		return _line;
	}

	/**
	 * get the number of the current column
	 */
	inline int getCol() const {
		return _pos - _col - 1;
	}

	/**
	 * get and remove the last atom of the current formula
	 */
	shared_ptr<Atom> getLastAtom();

	/**
	 * get and remove the atom represented by the current formula
	 */
	shared_ptr<Atom> getFormulaAtom();

	/**
	 * Put an atom in the current formula
	 */
	void addAtom(const shared_ptr<Atom>& atom);

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
	 * Return a boolean indicating if the character @ is considered as a letter
	 * or not
	 */
	inline bool isAtLetter() {
		return (_atIsLetter != 0);
	}

	/**
	 * Return a boolean indicating if the parser is used to parse an array or
	 * not
	 */
	inline bool isArrayMode() const {
		return _arrayMode;
	}

	inline void setArrayMode(bool arrayMode) {
		_arrayMode = arrayMode;
	}

	/**
	 * Return a boolean indicating if the parser must ignore white spaces
	 */
	inline bool isIgnoreWhiteSpace() const {
		return _ignoreWhiteSpace;
	}

	/**
	 * Return a boolean indicating if the parser is in math mode
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
	 * 		the number of character to be rewinded
	 * @return the new position in the parsed string
	 */
	inline int rewind(int n) {
		_pos -= n;
		return _pos;
	}

	inline wstring getStringFromCurrentPos() {
		return _parseString.substr(_pos);
	}

	inline void finish() {
		_pos = _parseString.size();
	}

	/**
	 * Add a new row when the parser is in array mode
	 *
	 * @throw ex_parse
	 * 		if the parser is not in array mode
	 */
	void addRow() throw(ex_parse);

	/**
	 * parse the input string
	 * @throw ex_parse
	 *      if an error is encountered during parse
	 */
	void parse() throw(ex_parse);

	/**
	 * Get the contents between two delimiters
	 *
	 * @param openClose
	 * 		the opening and closing character (such $)
	 * @return the enclosed contents
	 * @throw ex_parse
	 *      if the contents are badly enclosed
	 */
	wstring getDollarGroup(wchar_t openClose) throw(ex_parse);

	/**
	 * Get the contents between two delimiters
	 *
	 * @param open
	 * 		the opening character
	 * @param close
	 *      the closing character
	 * @return the enclosed contents
	 * @throw ex_parse
	 *      if the contents are badly enclosed
	 */
	wstring getGroup(wchar_t open, wchar_t close) throw(ex_parse);

	/**
	 * Get the contents between two strings as in \\begin{foo}... \\end{foo}
	 *
	 * @param open
	 * 		the opening string
	 * @param close
	 *      the closing string
	 * @return the enclosed contents
	 * @throw ex_parse
	 *      if the contents are badly enclosed
	 */
	wstring getGroup(const wstring& open, const wstring& close) throw(ex_parse);

	/**
	 * Get the argument of a command in his atomic format
	 *
	 * @return the corresponding atom
	 * @throw ex_parse
	 * 		if the argument is incorrect
	 */
	shared_ptr<Atom> getArgument() throw(ex_parse);

	wstring getOverArgument() throw(ex_parse);

	pair<int, float> getLength() throw(ex_parse);

	/**
	 * Convert a character in the corresponding atom in using the file
	 * TeXFormulaSettings.xml for non-alphanumeric characters
	 *
	 * @param c
	 * 		the character to be converted
	 * @return the corresponding atom
	 * @throw ex_parse
	 *      if the character is unknown
	 */
	shared_ptr<Atom> convertCharacter(wchar_t c, bool oneChar) throw(ex_parse);

	/**
	 * Get the arguments and the options of a command
	 *
	 * @param nbArgs
	 * 		the number of arguments of the command
	 * @param opts
	 *      must be 1 if the options are found before the first argument
	 *      and must be 2 if they must be found before the second argument
	 * @param args
	 *      a vector to put with argument strings
	 */
	void getOptsArgs(int nbArgs, int opts, _out_ vector<wstring>& args);

	/**
	 * Test the validity of the name of a command. It must contains only alpha
	 * characters and eventually a @ if makeAtletter activated
	 *
	 * @param cmd
	 * 		the command's name
	 * @return the validity of the name
	 */
	bool isValidName(const wstring& cmd);

	/**
	 * Test the validity of a character in a command. It must contains only
	 * alpha characters and eventually a @ if makeAtletter activated
	 */
	inline bool isValidCharacterInCommand(wchar_t ch) const {
		return isalpha(ch) || (_atIsLetter != 0 && ch == '@');
	}

};

}

#endif // PARSER_H_INCLUDED
