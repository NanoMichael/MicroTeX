#if defined (__clang__)
#include "atom.h"
#include "atom_basic.h"
#include "parser.h"
#include "formula.h"
#include "macro.h"
#include "port.h"
#include "alphabet.h"
#include "fonts.h"
#elif defined (__GNUC__)
#include "atom/atom.h"
#include "atom/atom_basic.h"
#include "core/parser.h"
#include "core/formula.h"
#include "core/macro.h"
#include "port/port.h"
#include "fonts/alphabet.h"
#include "fonts/fonts.h"
#endif // defined

#include "common.h"

#include "memcheck.h"

using namespace std;
using namespace tex;
using namespace tex::fonts;
using namespace tex::port;
using namespace tex::core;

const wchar_t TeXParser::ESCAPE = '\\';
const wchar_t TeXParser::L_GROUP = '{';
const wchar_t TeXParser::R_GROUP = '}';
const wchar_t TeXParser::L_BRACK = '[';
const wchar_t TeXParser::R_BRACK = ']';
const wchar_t TeXParser::DOLLAR = '$';
const wchar_t TeXParser::DQUOTE = '\"';
const wchar_t TeXParser::PERCENT = '%';
const wchar_t TeXParser::SUB_SCRIPT = '_';
const wchar_t TeXParser::SUPER_SCRIPT = '^';
const wchar_t TeXParser::PRIME = '\'';
const wchar_t TeXParser::BACKPRIME = 0x2035;
const wchar_t TeXParser::DEGRE = 0x00B0;

const map<wchar_t, char> TeXParser::SUP_SCRIPT_MAP = {
	{ 0x2070, '0'},
	{ 0x00B9, '1'},
	{ 0x00B2, '2'},
	{ 0x00B3, '3'},
	{ 0x2074, '4'},
	{ 0x2075, '5'},
	{ 0x2076, '6'},
	{ 0x2077, '7'},
	{ 0x2078, '8'},
	{ 0x2079, '9'},
	{ 0x207A, '+'},
	{ 0x207B, '-'},
	{ 0x207C, '='},
	{ 0x207D, '('},
	{ 0x207E, ')'},
	{ 0x207F, 'n'}
};
const map<wchar_t, char> TeXParser::SUB_SCRIPT_MAP = {
	{ 0x2080, '0'},
	{ 0x2081, '1'},
	{ 0x2082, '2'},
	{ 0x2083, '3'},
	{ 0x2084, '4'},
	{ 0x2085, '5'},
	{ 0x2086, '6'},
	{ 0x2087, '7'},
	{ 0x2088, '8'},
	{ 0x2089, '9'},
	{ 0x208A, '+'},
	{ 0x208B, '-'},
	{ 0x208C, '='},
	{ 0x208D, '('},
	{ 0x208E, ')'},
};

const set<wstring> TeXParser::_unparsedContents = {
	L"dynamic",
	L"Text",
	L"Textit",
	L"Textbf",
	L"Textitbf",
	L"externalFont"
};

bool TeXParser::_isLoading = false;

void TeXParser::init(bool ispartial, const wstring& parsestring, _out_ TeXFormula* formula, bool fp) throw(ex_parse) {
	_pos = _spos = _len = 0;
	_line = _col = 0;
	_group = 0;
	_atIsLetter = 0;
	_insertion = _arrayMode = _ignoreWhiteSpace = false;
	_isPartial = _hideUnknownChar = true;

	_formula = formula;
	_ignoreWhiteSpace = true;
	_isPartial = ispartial;
	if (!parsestring.empty()) {
		_parseString = parsestring;
		_len = parsestring.length();
		_pos = 0;
		if (fp) {
			firstpass();
		}
	} else {
		_parseString = L"";
		_pos = 0;
		_len = 0;
	}
	ArrayOfAtoms* arr = dynamic_cast<ArrayOfAtoms*>(formula);
	_arrayMode = (arr != nullptr);
}

void TeXParser::reset(const wstring& latex) {
	_parseString = latex;
	_len = latex.length();
	_formula->_root = shared_ptr<Atom>(nullptr);
	_pos = 0;
	_spos = 0;
	_line = 0;
	_col = 0;
	_group = 0;
	_insertion = false;
	_atIsLetter = 0;
	_arrayMode = false;
	_ignoreWhiteSpace = true;
	firstpass();
}

shared_ptr<Atom> TeXParser::getLastAtom() {
	auto a = _formula->_root;
	RowAtom* ra = dynamic_cast<RowAtom*>(a.get());
	if (ra != nullptr)
		return ra->getLastAtom();
	_formula->_root = shared_ptr<Atom>(nullptr);
	return a;
}

shared_ptr<Atom> TeXParser::getFormulaAtom() {
	auto a = _formula->_root;
	_formula->_root = shared_ptr<Atom>(nullptr);
	return a;
}

void TeXParser::addAtom(const shared_ptr<Atom>& atom) {
	_formula->add(atom);
}

void TeXParser::addRow() throw(ex_parse) {
	if (!_arrayMode)
		throw ex_parse("can not add row in none-array mode!");
	((ArrayOfAtoms*) _formula)->addRow();
}

wstring TeXParser::getDollarGroup(wchar_t openclose) throw(ex_parse) {
	int spos = _pos;
	wchar_t ch;

	do {
		ch = _parseString[_pos++];
		if (ch == ESCAPE)
			_pos++;
	} while(_pos < _len && ch != openclose);

	if (ch == openclose)
		return _parseString.substr(spos, _pos - spos - 1);
	return _parseString.substr(spos, _pos - spos);
}

wstring TeXParser::getGroup(wchar_t open, wchar_t close) throw(ex_parse) {
	if (_pos == _len)
		return L"";

	int group, spos;
	wchar_t ch = _parseString[_pos];

	if (_pos < _len && ch == open) {
		group = 1;
		spos = _pos;
		while (_pos < _len - 1 && group != 0) {
			_pos++;
			ch = _parseString[_pos];
			if (ch == open)
				group++;
			else if (ch == close)
				group--;
			else if (ch == ESCAPE && _pos != _len - 1)
				_pos++;
		}

		_pos++;

		if (group != 0)
			return _parseString.substr(spos + 1, _pos - spos - 1);
		return _parseString.substr(spos + 1, _pos - spos - 2);
	}
	throw ex_parse("missing '" + tostring((char)open) + "'!");
}

wstring TeXParser::getGroup(const wstring& open, const wstring& close) throw(ex_parse) {
	int group = 1;
	int ol = open.length();
	int cl = close.length();
	bool lastO = isValidCharacterInCommand(open[ol - 1]);
	bool lastC = isValidCharacterInCommand(close[cl - 1]);
	int oc = 0;
	int cc = 0;
	int startC = 0;
	wchar_t prev = L'\0';
	wstring buf;

	while (_pos < _len && group != 0) {
		wchar_t c = _parseString[_pos];
		wchar_t c1;

		if (prev != ESCAPE && c == ' ') {

			while (_pos < _len && _parseString[_pos++] == ' ')
				buf.append(1, L' ');
			c = _parseString[--_pos];
			if (isValidCharacterInCommand(prev) && isValidCharacterInCommand(c))
				oc = cc = 0;
		}

		if (c == open[oc])
			oc++;
		else
			oc = 0;

		if (c == close[cc]) {
			if (cc == 0)
				startC = _pos;
			cc++;
		} else
			cc = 0;

		if (_pos + 1 < _len) {
			c1 = _parseString[_pos + 1];

			if (oc == ol) {
				if (!lastO || !isValidCharacterInCommand(c1))
					group++;
				oc = 0;
			}

			if (cc == cl) {
				if (!lastC || !isValidCharacterInCommand(c1))
					group--;
				cc = 0;
			}
		} else {
			if (oc == ol) {
				group++;
				oc = 0;
			}
			if (cc == cl) {
				group--;
				cc = 0;
			}
		}

		prev = c;
		buf.append(1, c);
		_pos++;
	}

	if (group != 0) {
		if (_isPartial)
			return buf;
		throw ex_parse("parse string not closed correctly!");
	}

	return buf.substr(0, buf.length() - _pos + startC);
}

wstring TeXParser::getOverArgument() throw(ex_parse) {
	if (_pos == _len)
		return L"";

	int ogroup = 1, spos;
	wchar_t ch = L'\0';

	spos = _pos;
	while (_pos < _len && ogroup != 0) {
		ch = _parseString[_pos];
		switch (ch) {
		case L_GROUP:
			ogroup++;
			break;
		case '&':
			// if a & is encountered at the same level as \over we must break the argument
			if (ogroup == 1)
				ogroup--;
			break;
		case R_GROUP:
			ogroup--;
			break;
		case ESCAPE:
			_pos++;
			// if a \\ or \cr is encountered at the same level as \over
			// we must break the argument
			if (_pos < _len && _parseString[_pos] == '\\' && ogroup == 1) {
				ogroup--;
				_pos--;
			} else if (_pos < _len - 1 && _parseString[_pos] == 'c' && _parseString[_pos + 1] == 'r' && ogroup == 1) {
				ogroup--;
				_pos--;
			}
			break;
		}
		_pos++;
	}

	if (ogroup >= 2) // end of string reached, bu not processed properly
		throw ex_parse("Illegal end, missing '}'!");

	wstring str;
	if (ogroup == 0) {
		str = _parseString.substr(spos, _pos - spos - 1);
	} else {
		str = _parseString.substr(spos, _pos - spos);
		ch = '\0';
	}

	if (ch == '&' || ch == '\\' || ch == R_GROUP)
		_pos--;

	return str;
}

wstring TeXParser::getCommand() {
	int spos = ++_pos;
	wchar_t ch = L'\0';

	while (_pos < _len) {
		ch = _parseString[_pos];
		if ((ch < 'a' || ch > 'z') && (ch < 'A' || ch > 'Z') && (_atIsLetter == 0 || ch != '@'))
			break;

		_pos++;
	}

	if (ch == L'\0')
		return L"";

	if (_pos == spos)
		_pos++;

	wstring com = _parseString.substr(spos, _pos - spos);
	if (com == L"cr" && _pos < _len && _parseString[_pos] == ' ')
		_pos++;

	return com;
}

void TeXParser::insert(int s, int e, const wstring& formula) {
	_parseString.replace(s, e - s, formula);
	_len = _parseString.length();
	_pos = s;
	_insertion = true;
}

wstring TeXParser::getCommandWithArgs(const wstring& command) {
	if (command == L"left")
		return getGroup(L"\\left", L"\\right");

	auto it = MacroInfo::_commands.find(command);
	if (it == MacroInfo::_commands.end())
		return L"\\" + command;

	MacroInfo* mac = it->second;
	int mac_opts = 0;
	if (mac->_hasOptions)
		mac_opts = mac->_posOpts;

	vector<wstring> mac_args;
	getOptsArgs(mac->_nbArgs, mac_opts, mac_args);
	wstring mac_arg(L"\\");
	mac_arg.append(command);
	for (int j = 0; j < mac->_posOpts; j++) {
		wstring arg_t = mac_args[mac->_nbArgs + j + 1];
		if (!arg_t.empty()) {
			mac_arg.append(L"[");
			mac_arg.append(arg_t);
			mac_arg.append(L"]");
		}
	}

	for (int j = 0; j < mac->_nbArgs; j++) {
		wstring arg_t = mac_args[j + 1];
		if (!arg_t.empty()) {
			mac_arg.append(L"{");
			mac_arg.append(arg_t);
			mac_arg.append(L"}");
		}
	}

	return mac_arg;
}

void TeXParser::skipWhiteSpace() {
	wchar_t c;
	while (_pos < _len) {
		c = _parseString[_pos];
		if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
			break;
		if (c == '\n') {
			_line++;
			_col = _pos;
		}
		_pos++;
	}
}

void TeXParser::getOptsArgs(int nbArgs, int opts, _out_ vector<wstring>& args) {
	/*
	 * A maximum of 10 options can be passed to a command,
	 * and the extra value will be added at the tail if needed
	 */
	args.resize(nbArgs + 10 + 1 + 1);
	if (nbArgs != 0) {

		// we get the options just after the command name
		if (opts == 1) {
			int j = nbArgs + 1;
			try {
				for (; j < nbArgs + 11; j++) {
					skipWhiteSpace();
					args[j] = getGroup(L_BRACK, R_BRACK);
				}
			} catch (ex_parse& e) {
				args[j] = L"";
			}
		}

		// we get the first argument
		skipWhiteSpace();
		try {
			args[1] = getGroup(L_GROUP, R_GROUP);
		} catch (ex_parse& e) {
			if (_parseString[_pos] != '\\') {
				args[1] = towstring(_parseString[_pos]);
				_pos++;
			} else {
				args[1] = getCommandWithArgs(getCommand());
			}
		}

		// we get the options after the first argument
		if (opts == 2) {
			int j = nbArgs + 1;
			try {
				for (; j < nbArgs + 11; j++) {
					skipWhiteSpace();
					args[j] = getGroup(L_BRACK, R_BRACK);
				}
			} catch (ex_parse& e) {
				args[j] = L"";
			}
		}

		// we get the next arguments
		for (int i = 2; i <= nbArgs; i++) {
			skipWhiteSpace();
			try {
				args[i] = getGroup(L_GROUP, R_GROUP);
			} catch (ex_parse& e) {
				if (_parseString[_pos] != '\\') {
					args[i] = towstring(_parseString[_pos]);
					_pos++;
				} else {
					args[i] = getCommandWithArgs(getCommand());
				}
			}
		}

		if (_ignoreWhiteSpace)
			skipWhiteSpace();
	}
}

bool TeXParser::isValidName(const wstring& com) {
	if (com.empty())
		return false;

	if (com[0] != '\\')
		return false;

	wchar_t c = L'\0';
	int p = 1;
	int l = com.length();
	while (p < l) {
		c = com[p];
		if (!isalpha(c) && (_atIsLetter == 0 || c != '@'))
			break;
		p++;
	}

	return isalpha(c);
}

/**
 * Convert a character to roman-number if it is a digit localized
 * @param c Character to be converted
 */
wchar_t TeXParser::convert2RomanNumber(wchar_t c) {
	if (c == 0x66b) {// Arabic dot
		return '.';
	} else if (0x660 <= c && c <= 0x669) {// Arabic
		return (wchar_t) (c - (wchar_t) 0x630);
	} else if (0x6f0 <= c && c <= 0x6f9) {// Arabic
		return (wchar_t) (c - (wchar_t) 0x6c0);
	} else if (0x966 <= c && c <= 0x96f) {// Devanagari
		return (wchar_t) (c - (wchar_t) 0x936);
	} else if (0x9e6 <= c && c <= 0x9ef) {// Bengali
		return (wchar_t) (c - (wchar_t) 0x9b6);
	} else if (0xa66 <= c && c <= 0xa6f) {// Gurmukhi
		return (wchar_t) (c - (wchar_t) 0xa36);
	} else if (0xae6 <= c && c <= 0xaef) {// Gujarati
		return (wchar_t) (c - (wchar_t) 0xab6);
	} else if (0xb66 <= c && c <= 0xb6f) {// Oriya
		return (wchar_t) (c - (wchar_t) 0xb36);
	} else if (0xc66 <= c && c <= 0xc6f) {// Telugu
		return (wchar_t) (c - (wchar_t) 0xc36);
	} else if (0xd66 <= c && c <= 0xd6f) {// Malayalam
		return (wchar_t) (c - (wchar_t) 0xd36);
	} else if (0xe50 <= c && c <= 0xe59) {// Thai
		return (wchar_t) (c - (wchar_t) 0xe20);
	} else if (0xed0 <= c && c <= 0xed9) {// Lao
		return (wchar_t) (c - (wchar_t) 0xea0);
	} else if (0xf20 <= c && c <= 0xf29) {// Tibetan
		return (wchar_t) (c - (wchar_t) 0xe90);
	} else if (0x1040 <= c && c <= 0x1049) {// Myanmar
		return (wchar_t) (c - (wchar_t) 0x1010);
	} else if (0x17e0 <= c && c <= 0x17e9) {// Khmer
		return (wchar_t) (c - (wchar_t) 0x17b0);
	} else if (0x1810 <= c && c <= 0x1819) {// Mongolian
		return (wchar_t) (c - (wchar_t) 0x17e0);
	} else if (0x1b50 <= c && c <= 0x1b59) {// Balinese
		return (wchar_t) (c - (wchar_t) 0x1b20);
	} else if (0x1bb0 <= c && c <= 0x1bb9) {// Sundanese
		return (wchar_t) (c - (wchar_t) 0x1b80);
	} else if (0x1c40 <= c && c <= 0x1c49) {// Lepcha
		return (wchar_t) (c - (wchar_t) 0x1c10);
	} else if (0x1c50 <= c && c <= 0x1c59) {// Ol Chiki
		return (wchar_t) (c - (wchar_t) 0x1c20);
	} else if (0xa8d0 <= c && c <= 0xa8d9) {// Saurashtra
		return (wchar_t) (c - (wchar_t) 0xa8a0);
	}

	return c;
}

shared_ptr<Atom> TeXParser::processEscape() throw(ex_parse) {
	_spos = _pos;
	wstring command = getCommand();

	if (command.length() == 0)
		return shared_ptr<Atom>(new EmptyAtom());

	string cmd;
	wide2utf8(command.c_str(), cmd);
	auto it = MacroInfo::_commands.find(command);
	if (it != MacroInfo::_commands.end())
		return processCommands(command);

	try {
		return TeXFormula::get(command)->_root;
	} catch (ex_formula_not_found& e) {
		try {
			return SymbolAtom::get(cmd);
		} catch (ex_symbol_not_found& ex) {}
	}

	// not a valid command or symbol or predefined TeXFormula found
	if (!_isPartial)
		throw ex_parse("unknown symbol or command or predefined TeXFormula: '" + cmd + "'");
	shared_ptr<Atom> rm(new RomanAtom(TeXFormula(L"\\backslash " + command)._root));
	return shared_ptr<Atom>(new ColorAtom(rm, TRANS, RED));
}

shared_ptr<Atom> TeXParser::processCommands(const wstring& command) throw(ex_parse) {
	MacroInfo* mac = MacroInfo::_commands[command];
	int opts = 0;
	if (mac->_hasOptions)
		opts = mac->_posOpts;

	vector<wstring> args;
	getOptsArgs(mac->_nbArgs, opts, args);
	args[0] = command;

	if (NewCommandMacro::isMacro(command)) {
		auto ret = mac->invoke(*this, args);
		insert(_spos, _pos, args.back());
		return ret;
	}

	return mac->invoke(*this, args);
}

shared_ptr<Atom> TeXParser::getScripts(wchar_t f) throw(ex_parse) {
	_pos++;
	// get the first script
	shared_ptr<Atom> first = getArgument();
	shared_ptr<Atom> second(nullptr);
	wchar_t s = '\0';

	if (_pos < _len)
		s = _parseString[_pos];

	/*
	 * 4 conditions
	 *
	 * \cmd_{\sub}^{\super}
	 *
	 * \cmd_{\sub}
	 *
	 * \cmd^{\super}_{\sub}
	 *
	 * \cmd^{\super}
	 */
	if (f == SUPER_SCRIPT && s == SUPER_SCRIPT) {
		second = first;
		first = shared_ptr<Atom>(nullptr);
	} else if (f == SUB_SCRIPT && s == SUPER_SCRIPT) {
		_pos++;
		second = getArgument();
	} else if (f == SUPER_SCRIPT && s == SUB_SCRIPT) {
		_pos++;
		second = first;
		first = getArgument();
	} else if (f == SUPER_SCRIPT && s != SUB_SCRIPT) {
		second = first;
		first = nullptr;
	}

	shared_ptr<Atom> atom;
	RowAtom* rm = dynamic_cast<RowAtom*>(_formula->_root.get());
	if (rm != nullptr) {
		atom = rm->getLastAtom();
	} else if (_formula->_root == nullptr) {
		/*
		 * If there's no root exists, create one with phantom to place
		 * the scripts
		 */
		shared_ptr<Atom> in(new CharAtom(L'M', "mathnormal"));
		atom = shared_ptr<Atom>(new PhantomAtom(in, false, true, true));
	} else {
		atom = _formula->_root;
		_formula->_root = shared_ptr<Atom>(nullptr);
	}

	if (atom->getRightType() == TYPE_BIG_OPERATOR)
		return shared_ptr<Atom>(new BigOperatorAtom(atom, first, second));

	OverUnderDelimiter* del = dynamic_cast<OverUnderDelimiter*>(atom.get());
	if (del != nullptr) {
		if (del->isOver()) {
			if (second != nullptr) {
				del->addScript(second);
				return shared_ptr<Atom>(new ScriptsAtom(atom, first, shared_ptr<Atom>(nullptr)));
			}
		} else if (first != nullptr) {
			del->addScript(first);
			return shared_ptr<Atom>(new ScriptsAtom(atom, shared_ptr<Atom>(nullptr), second));
		}
	}

	return shared_ptr<Atom>(new ScriptsAtom(atom, first, second));
}

shared_ptr<Atom> TeXParser::getArgument() throw(ex_parse) {
	skipWhiteSpace();
	wchar_t ch;
	if (_pos < _len)
		ch = _parseString[_pos];
	else
		return shared_ptr<Atom>(new EmptyAtom());

	if (ch == L_GROUP) {
		TeXFormula tf;
		TeXFormula* tmp = _formula;
		_formula = &tf;
		_pos++;
		_group++;
		parse();
		_formula = tmp;
		if (_formula->_root == nullptr) {
			RowAtom* rm = new RowAtom();
			rm->add(tf._root);
			return shared_ptr<Atom>(rm);
		}
		return tf._root;
	}

	if (ch == ESCAPE) {
		auto atom = processEscape();
		if (_insertion) {
			_insertion = false;
			return getArgument();
		}
		return atom;
	}

	auto atom = convertCharacter(ch, true);
	_pos++;
	return atom;
}

pair<int, float> TeXParser::getLength() throw(ex_parse) {
	if (_pos == _len)
		return make_pair(-1.f, -1.f);

	int spos;
	wchar_t ch = L'\0';

	skipWhiteSpace();
	spos = _pos;
	while (_pos < _len && ch != ' ') {
		ch = _parseString[_pos++];
	}
	skipWhiteSpace();

	return SpaceAtom::getLength(_parseString.substr(spos, _pos - spos - 1));
}

bool TeXParser::replaceScript() {
	wchar_t ch = _parseString[_pos];
	auto it = SUP_SCRIPT_MAP.find(ch);
	if (it != SUP_SCRIPT_MAP.end()) {
		wstring sup = wstring(L"\\mathcumsup{").append(1, (wchar_t)(it->second)).append(L"}");
		_parseString.replace(_pos, 1, sup);
		_len = _parseString.length();
		_pos += sup.size();
		return true;
	}
	it = SUB_SCRIPT_MAP.find(ch);
	if (it != SUB_SCRIPT_MAP.end()) {
		wstring sub = wstring(L"\\mathcumsub{").append(1, (wchar_t)(it->second)).append(L"}");
		_parseString.replace(_pos, 1, sub);
		_len = _parseString.length();
		_pos += sub.size();
		return true;
	}
	return false;
}

void TeXParser::firstpass() throw(ex_parse) {
	if (_len == 0)
		return;

	wchar_t ch;
	wstring com;
	int spos;
	vector<wstring> args;
	MacroInfo* mac;
	while (_pos < _len) {
		if (replaceScript())
			continue;

		ch = _parseString[_pos];
		switch (ch) {
		case ESCAPE: {
			spos = _pos;
			com = getCommand();
			if (com == L"newcommand" || com == L"renewcommand") {
				getOptsArgs(2, 2, args);
				mac = MacroInfo::_commands[com];
				try {
					mac->invoke(*this, args);
				} catch (ex_parse& e) {
					if (!_isPartial)
						throw;
				}
				_parseString.erase(spos, _pos - spos);
				_len = _parseString.length();
				_pos = spos;
			} else if (NewCommandMacro::isMacro(com)) {
				mac = MacroInfo::_commands[com];
				getOptsArgs(mac->_nbArgs, mac->_hasOptions ? 1 : 0, args);
				args[0] = com;
				try {
					mac->invoke(*this, args);
					// retrieve the last element as return
					wstring x = args.back();
					_parseString.replace(spos, _pos - spos, x);
				} catch (ex_parse& e) {
					if (!_isPartial)
						throw;
					spos += com.length() + 1;
				}
				_len = _parseString.length();
				_pos = spos;
			} else if (com == L"begin") {
				getOptsArgs(1, 0, args);
				wstring env = args[1] + L"@env";
				auto it = MacroInfo::_commands.find(env);
				if (it == MacroInfo::_commands.end()) {
					if (!_isPartial)
						throw ex_parse("unknown environment: " + wide2utf8(args[1].c_str()) + " at position " + tostring(getLine()) + ":" + tostring(getCol()));
				} else {
					mac = it->second;
					try {
						vector<wstring> optarg;
						getOptsArgs(mac->_nbArgs - 1, 0, optarg);
						wstring grp = getGroup(L"\\begin{" + args[1] + L"}", L"\\end{" + args[1] + L"}");
						wstring expr = L"{\\makeatletter \\" + args[1] + L"@env";
						for (int i = 1; i <= mac->_nbArgs - 1; i++)
							expr += L"{" + optarg[i] + L"}";
						expr += L"{" + grp + L"}\\makeatother}";
						_parseString.replace(spos, _pos - spos, expr);
						_len = _parseString.length();
						_pos = spos;
					} catch (ex_parse& e) {
						if (!_isPartial)
							throw;
					}
				}
			} else if (com == L"makeatletter") {
				_atIsLetter++;
			} else if (com == L"makeatother") {
				_atIsLetter--;
			} else if (_unparsedContents.find(com) != _unparsedContents.end()) {
				getOptsArgs(1, 0, args);
			}
			args.clear();
		}
		break;
		case PERCENT: {
			spos = _pos++;
			wchar_t chr;
			while (_pos < _len) {
				chr = _parseString[_pos++];
				if (chr == '\r' || chr == '\n')
					break;
			}
			if (_pos < _len)
				_pos--;
			_parseString.replace(spos, _pos - spos, L"");
			_len = _parseString.length();
			_pos = spos;
		}
		break;
		case DEGRE: {
			_parseString.replace(_pos, 1, L"^\\circ");
			_len = _parseString.length();
			_pos++;
		}
		break;
		default:
			_pos++;
			break;
		}
	}
	_pos = 0;
	_len = _parseString.length();
}

void TeXParser::parse() throw(ex_parse) {
	if (_len == 0) {
		if (_formula->_root == nullptr && !_arrayMode)
			_formula->add(shared_ptr<Atom>(new EmptyAtom()));
		return;
	}

	wchar_t ch;
	while (_pos < _len) {
		ch = _parseString[_pos];

		switch (ch) {
		case '\n':
			_line++;
			_col = _pos;
		case '\t':
		case '\r':
			_pos++;
			break;
		case ' ': {
			_pos++;
			if (!_ignoreWhiteSpace) { // we are in mbox
				_formula->add(shared_ptr<Atom>(new SpaceAtom()));
				_formula->add(shared_ptr<Atom>(new BreakMarkAtom()));
				while (_pos < _len) {
					ch = _parseString[_pos];
					if (ch != ' ' || ch != '\t' || ch != '\r')
						break;
					_pos++;
				}
			}
		}
		break;
		case DOLLAR: {
			_pos++;
			if (!_ignoreWhiteSpace) { // we are in mbox
				int style = STYLE_TEXT;
				bool doubleDollar = false;
				if (_parseString[_pos] == DOLLAR) {
					style = STYLE_DISPLAY;
					doubleDollar = true;
					_pos++;
				}

				_formula->add(shared_ptr<Atom>(new MathAtom(TeXFormula(*this, getDollarGroup(DOLLAR), false)._root, style)));
				if (doubleDollar) {
					if (_parseString[_pos] == DOLLAR)
						_pos++;
				}
			}
		}
		break;
		case ESCAPE: {
			shared_ptr<Atom> atom = processEscape();
			_formula->add(atom);
			HlineAtom* h = dynamic_cast<HlineAtom*>(atom.get());
			if (_arrayMode && h != nullptr) {
				((ArrayOfAtoms*)_formula)->addRow();
			}
			if (_insertion)
				_insertion = false;
		}
		break;
		case L_GROUP: {
			auto atom = getArgument();
			if (atom != nullptr)
				atom->_type = TYPE_ORDINARY;
			_formula->add(atom);
		}
		break;
		case R_GROUP: {
			_group--;
			_pos++;
			if (_group == -1)
				throw ex_parse("found a closing '" + tostring((char)R_GROUP) + "' without an opening " + tostring((char)L_GROUP) + "'!");
		}
		return;
		case SUPER_SCRIPT: {
			_formula->add(getScripts(ch));
		}
		break;
		case SUB_SCRIPT: {
			if (_ignoreWhiteSpace) {
				_formula->add(getScripts(ch));
			} else {
				_formula->add(shared_ptr<Atom>(new UnderScoreAtom()));
				_pos++;
			}
		}
		break;
		case '&': {
			if (!_arrayMode)
				throw ex_parse("character '&' is only available in array mode!");
			((ArrayOfAtoms*)_formula)->addCol();
			_pos++;
		}
		break;
		case PRIME: {
			if (_ignoreWhiteSpace)
				_formula->add(shared_ptr<Atom>(new CumulativeScriptsAtom(getLastAtom(), shared_ptr<Atom>(nullptr), SymbolAtom::get("prime"))));
			else
				_formula->add(convertCharacter(PRIME, true));
			_pos++;
		}
		break;
		case BACKPRIME: {
			if (_ignoreWhiteSpace)
				_formula->add(shared_ptr<Atom>(new CumulativeScriptsAtom(getLastAtom(), shared_ptr<Atom>(nullptr), SymbolAtom::get("backprime"))));
			else
				_formula->add(convertCharacter(BACKPRIME, true));
			_pos++;
		}
		break;
		case DQUOTE: {
			if (_ignoreWhiteSpace) {
				_formula->add(shared_ptr<Atom>(new CumulativeScriptsAtom(getLastAtom(), shared_ptr<Atom>(nullptr), SymbolAtom::get("prime"))));
				_formula->add(shared_ptr<Atom>(new CumulativeScriptsAtom(getLastAtom(), shared_ptr<Atom>(nullptr), SymbolAtom::get("prime"))));
			} else {
				_formula->add(convertCharacter(PRIME, true));
				_formula->add(convertCharacter(PRIME, true));
			}
			_pos++;
		}
		break;
		default: {
			_formula->add(convertCharacter(ch, false));
			_pos++;
		}
		break;
		}
	}
}

/**
 * Convert a character to a atom
 * @param c Character to be convert
 * @param oneChar If Convert only one char in the parse string
 */
shared_ptr<Atom> TeXParser::convertCharacter(wchar_t c, bool oneChar) throw(ex_parse) {
	if (_ignoreWhiteSpace) {
		// the unicode Greek Letters in math mode are not drawn with the Greek font
		if (c >= 945 && c <= 969) { // Greek small letter
			return SymbolAtom::get(TeXFormula::_symbolMappings[c]);
		} else if (c >= 913 && c <= 937) { // Greek capital letter
			wstring ltx;
			utf82wide(TeXFormula::_symbolFormulaMappings[c].c_str(), ltx);
			return TeXFormula(ltx)._root;
		}
	}

	c = convert2RomanNumber(c);

	/*
	 * None alphanumeric character
	 */
	if ((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z')) {
		/*
		 * Find from registered UNICODE-table
		 */
		const UnicodeBlock& block = UnicodeBlock::of(c);
#ifdef __DEBUG
		int idx = indexOf(DefaultTeXFont::_loadedAlphabets, block);
		__log << "block of char: " << c << " is " << idx << endl;
#endif // __DEBUG
		bool exist = (indexOf(DefaultTeXFont::_loadedAlphabets, block) != -1);
		if (!_isLoading && !exist) {
			auto it = DefaultTeXFont::_registeredAlphabets.find(block);
			if (it != DefaultTeXFont::_registeredAlphabets.end())
				DefaultTeXFont::addAlphabet(DefaultTeXFont::_registeredAlphabets[block]);
		}

		auto sit = TeXFormula::_symbolMappings.find(c);
		auto fit = TeXFormula::_symbolFormulaMappings.find(c);

		/*
		 * Character not in the symbol-mapping and not in the formula-mapping, find from
		 * external font-mapping
		 */
		if (sit == TeXFormula::_symbolMappings.end() && fit == TeXFormula::_symbolFormulaMappings.end()) {
			FontInfos* fontInfos = nullptr;
			bool isLatin = UnicodeBlock::BASIC_LATIN == block;
			if ((isLatin && TeXFormula::isRegisteredBlock(UnicodeBlock::BASIC_LATIN)) || !isLatin) {
				fontInfos = TeXFormula::getExternalFont(block);
			}
			if (fontInfos != nullptr) {
				if (oneChar)
					return shared_ptr<Atom>(new TextRenderingAtom(towstring(c), fontInfos));
				int start = _pos++;
				int en = _len - 1;
				while (_pos < _len) {
					c = _parseString[_pos];
					if (!block.in(c)) {
						en = --_pos;
						break;
					}
					_pos++;
				}
				return shared_ptr<Atom>(new TextRenderingAtom(_parseString.substr(start, en - start + 1), fontInfos));
			}

			if (!_isPartial)
				throw ex_parse("unknown character : '" + tostring(c) + "'");
			else {
				if (_hideUnknownChar)
					return shared_ptr<Atom>(nullptr);

				shared_ptr<Atom> rm(new RomanAtom(TeXFormula(L"\\text{(unknown char " + towstring((int)c) + L")}")._root));
				return shared_ptr<Atom>(new ColorAtom(rm, TRANS, RED));
			}
		} else {
			/*
			 * In text mode (with command \text{})
			 */
			if (!_ignoreWhiteSpace) {
				auto it = TeXFormula::_symbolTextMappings.find(c);
				if (it != TeXFormula::_symbolTextMappings.end()) {
					auto atom = SymbolAtom::get(it->second);
					atom->setUnicode(c);
					return atom;
				}
			}
			auto it = TeXFormula::_symbolFormulaMappings.find(c);
			if (it != TeXFormula::_symbolFormulaMappings.end()) {
				wstring wstr;
				utf82wide(it->second.c_str(), wstr);
				return TeXFormula(wstr)._root;
			}

			if (sit != TeXFormula::_symbolMappings.end()) {
				string symbolName = sit->second;
				try {
					return SymbolAtom::get(symbolName);
				} catch (ex_symbol_not_found& e) {
					throw ex_parse("the character '" + tostring(c) + "' was mapped to an unknown symbol with the name '" + symbolName + "'!", e);
				}
			}
		}
	} else {
		/*
		 * Alphanumeric character
		 */
		FontInfos* infos = nullptr;
		auto it = TeXFormula::_externalFontMap.find(UnicodeBlock::BASIC_LATIN);
		if (it != TeXFormula::_externalFontMap.end()) {
			infos = it->second;
			if (oneChar)
				return shared_ptr<Atom>(new TextRenderingAtom(towstring(c), infos));

			int start = _pos++;
			int en = _len - 1;
			while (_pos < _len) {
				c = _parseString[_pos];
				if ((c < '0' || c > '9') && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z')) {
					en = --_pos;
					break;
				}
				_pos++;
			}
			return shared_ptr<Atom>(new TextRenderingAtom(_parseString.substr(start, en - start + 1), infos));
		}
	}
	return shared_ptr<Atom>(new CharAtom(c, _formula->_textStyle));
}
