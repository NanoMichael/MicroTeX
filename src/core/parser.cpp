#include "core/parser.h"
#include "atom/atom.h"
#include "atom/atom_basic.h"
#include "atom/atom_font.h"
#include "atom/atom_delim.h"
#include "atom/atom_scripts.h"
#include "atom/atom_operator.h"
#include "core/formula.h"
#include "macro/macro.h"
#include "graphic/graphic.h"
#include "env/units.h"
#include "utils/string_utils.h"
#include "utils/utf.h"

using namespace std;
using namespace tex;

const char TeXParser::ESCAPE = '\\';
const char TeXParser::L_GROUP = '{';
const char TeXParser::R_GROUP = '}';
const char TeXParser::L_BRACK = '[';
const char TeXParser::R_BRACK = ']';
const char TeXParser::DOLLAR = '$';
const char TeXParser::DQUOTE = '\"';
const char TeXParser::PERCENT = '%';
const char TeXParser::SUB_SCRIPT = '_';
const char TeXParser::SUPER_SCRIPT = '^';
const char TeXParser::PRIME = '\'';
const char TeXParser::BACKPRIME = '`';

void TeXParser::init(
  bool isPartial,
  const string& latex,
  Formula* formula,
  bool firstPass
) {
  _pos = _spos = _len = 0;
  _line = _col = 0;
  _group = 0;
  _atIsLetter = 0;
  _insertion = _arrayMode = _isMathMode = false;
  _isPartial = true;

  _formula = formula;
  _isMathMode = true;
  _isPartial = isPartial;
  if (!latex.empty()) {
    _latex = latex;
    _len = latex.length();
    _pos = 0;
    if (firstPass) preprocess();
  } else {
    _latex = "";
    _pos = 0;
    _len = 0;
  }
  _arrayMode = formula->isArrayMode();
}

void TeXParser::reset(const string& latex) {
  _latex = latex;
  _len = latex.length();
  _formula->_root = nullptr;
  _pos = 0;
  _spos = 0;
  _line = 0;
  _col = 0;
  _group = 0;
  _insertion = false;
  _atIsLetter = 0;
  _arrayMode = false;
  _isMathMode = true;
  preprocess();
}

sptr<Atom> TeXParser::popLastAtom() const {
  auto a = _formula->_root;
  auto* ra = dynamic_cast<RowAtom*>(a.get());
  if (ra != nullptr) return ra->popBack();
  _formula->_root = nullptr;
  return a;
}

sptr<Atom> TeXParser::popFormulaAtom() const {
  auto a = _formula->_root;
  _formula->_root = nullptr;
  return a;
}

void TeXParser::addAtom(const sptr<Atom>& atom) const {
  _formula->add(atom);
}

void TeXParser::addRow() const {
  if (!_arrayMode) throw ex_parse("Can not add row in none-array mode!");
  ((ArrayFormula*) _formula)->addRow();
}

string TeXParser::getDollarGroup(char openclose) {
  int spos = _pos;
  char ch;

  do {
    ch = _latex[_pos++];
    if (ch == ESCAPE) _pos++;
  } while (_pos < _len && ch != openclose);

  if (ch == openclose) return _latex.substr(spos, _pos - spos - 1);
  return _latex.substr(spos, _pos - spos);
}

string TeXParser::getGroup(char open, char close) {
  if (_pos == _len) return "";

  int group, spos;
  char ch = _latex[_pos];

  if (_pos < _len && ch == open) {
    group = 1;
    spos = _pos;
    while (_pos < _len - 1 && group != 0) {
      _pos++;
      ch = _latex[_pos];
      if (ch == open)
        group++;
      else if (ch == close)
        group--;
      else if (ch == ESCAPE && _pos != _len - 1)
        _pos++;
    }

    _pos++;

    if (group != 0) return _latex.substr(spos + 1, _pos - spos - 1);
    return _latex.substr(spos + 1, _pos - spos - 2);
  }
  throw ex_parse("Missing '" + tostring((char) open) + "'!");
}

string TeXParser::getGroup(const string& open, const string& close) {
  int group = 1;
  int ol = open.length();
  int cl = close.length();
  bool lastO = isValidCharInCmd(open[ol - 1]);
  bool lastC = isValidCharInCmd(close[cl - 1]);
  int oc = 0;
  int cc = 0;
  int startC = 0;
  char prev = L'\0';
  string buf;

  while (_pos < _len && group != 0) {
    char c = _latex[_pos];
    char c1;

    if (prev != ESCAPE && c == ' ') {
      while (_pos < _len && _latex[_pos++] == ' ') buf.append(1, L' ');
      c = _latex[--_pos];
      if (isValidCharInCmd(prev) && isValidCharInCmd(c)) {
        oc = cc = 0;
      }
    }

    if (c == open[oc]) oc++; else oc = 0;

    if (c == close[cc]) {
      if (cc == 0) startC = _pos;
      cc++;
    } else {
      cc = 0;
    }

    if (_pos + 1 < _len) {
      c1 = _latex[_pos + 1];

      if (oc == ol) {
        if (!lastO || !isValidCharInCmd(c1)) group++;
        oc = 0;
      }

      if (cc == cl) {
        if (!lastC || !isValidCharInCmd(c1)) group--;
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
    if (_isPartial) return buf;
    throw ex_parse("Parse string not closed correctly!");
  }

  return buf.substr(0, buf.length() - _pos + startC);
}

string TeXParser::getOverArgument() {
  if (_pos == _len) return "";

  int ogroup = 1, spos;
  char ch = L'\0';

  spos = _pos;
  while (_pos < _len && ogroup != 0) {
    ch = _latex[_pos];
    switch (ch) {
      case L_GROUP:
        ogroup++;
        break;
      case '&':
        // if a & is encountered at the same level as \over we must break the argument
        if (ogroup == 1) ogroup--;
        break;
      case R_GROUP:
        ogroup--;
        break;
      case ESCAPE:
        _pos++;
        // if a \\ or \cr is encountered at the same level as \over
        // we must break the argument
        if (_pos < _len && _latex[_pos] == '\\' && ogroup == 1) {
          ogroup--;
          _pos--;
        } else if (
          _pos < _len - 1
          && _latex[_pos] == 'c'
          && _latex[_pos + 1] == 'r'
          && ogroup == 1) {
          ogroup--;
          _pos--;
        }
      default:
        break;
    }
    _pos++;
  }

  // end of string reached, bu not processed properly
  if (ogroup >= 2) throw ex_parse("Illegal end, missing '}'!");

  string str;
  if (ogroup == 0) {
    str = _latex.substr(spos, _pos - spos - 1);
  } else {
    str = _latex.substr(spos, _pos - spos);
    ch = '\0';
  }

  if (ch == '&' || ch == '\\' || ch == R_GROUP) _pos--;

  return str;
}

string TeXParser::getCommand() {
  int spos = ++_pos;
  char ch = L'\0';

  while (_pos < _len) {
    ch = _latex[_pos];
    if ((ch < 'a' || ch > 'z')
        && (ch < 'A' || ch > 'Z')
        && (_atIsLetter == 0 || ch != '@')
      )
      break;

    _pos++;
  }

  if (ch == L'\0') return "";

  if (_pos == spos) _pos++;

  string com = _latex.substr(spos, _pos - spos);
  if (com == "cr" && _pos < _len && _latex[_pos] == ' ') _pos++;

  return com;
}

void TeXParser::insert(int beg, int end, const string& formula) {
  _latex.replace(beg, end - beg, formula);
  _len = _latex.length();
  _pos = beg;
  _insertion = true;
}

string TeXParser::getCommandWithArgs(const string& command) {
  if (command == "left") return getGroup("\\left", "\\right");

  auto mac = MacroInfo::get(command);
  if (mac == nullptr) {
    return "\\" + command;
  }
  int mac_opts = mac->_posOpts;

  // return as format: \cmd[opt][...]{arg}{...}

  vector<string> mac_args;
  getOptsArgs(mac->_argc, mac_opts, mac_args);
  string mac_arg("\\");
  mac_arg.append(command);
  for (int j = 0; j < mac->_posOpts; j++) {
    string arg_t = mac_args[mac->_argc + j + 1];
    if (!arg_t.empty()) {
      mac_arg.append("[").append(arg_t).append("]");
    }
  }

  for (int j = 0; j < mac->_argc; j++) {
    string arg_t = mac_args[j + 1];
    if (!arg_t.empty()) {
      mac_arg.append("{").append(arg_t).append("}");
    }
  }

  return mac_arg;
}

void TeXParser::skipWhiteSpace(int count) {
  count = count >= 0 ? count : std::numeric_limits<int>::max();
  char c;
  while (_pos < _len && count > 0) {
    c = _latex[_pos];
    if (c != ' ' && c != '\t' && c != '\n' && c != '\r') break;
    if (c == '\n') {
      _line++;
      _col = _pos;
    }
    _pos++;
    count--;
  }
}

string TeXParser::forwardBalancedGroup() {
  if (_group == 0) {
    const string& sub = _latex.substr(_pos);
    finish();
    return sub;
  }
  int closing = _group;
  auto i = _latex.length() - 1;
  for (; i >= _pos; i--) {
    auto ch = _latex[i];
    if (ch == R_GROUP) {
      closing--;
      if (closing == 0) break;
    }
  }
  if (closing != 0) {
    throw ex_parse("Found a closing '}' without an opening '{'!");
  }
  const string& sub = _latex.substr(_pos, i - _pos);
  _pos = i;
  return sub;
}

void TeXParser::getOptsArgs(int argc, int opts, Args& args) {
  // A maximum of 10 options can be passed to a command,
  // the value will be added at the tail of the args if found any,
  // the last (maximum to 12th) value is reserved for returned value
  args.resize(argc + 10 + 1 + 1);

  auto getOpts = [&]() {
    int j = argc + 1;
    try {
      for (; j < argc + 11; j++) {
        skipWhiteSpace();
        args[j] = getGroup(L_BRACK, R_BRACK);
      }
    } catch (ex_parse& e) {
      args[j] = "";
    }
  };

  auto getArg = [&](int i) { // NOLINT(misc-no-recursion)
    skipWhiteSpace();
    try {
      args[i] = getGroup(L_GROUP, R_GROUP);
    } catch (ex_parse& e) {
      if (_latex[_pos] != '\\') {
        args[i] = tostring(_latex[_pos]);
        _pos++;
      } else {
        args[i] = getCommandWithArgs(getCommand());
      }
    }
  };

  if (argc != 0) {
    // we get the options just after the command name
    if (opts == 1) getOpts();
    // we get the first argument
    getArg(1);
    // we get the options after the first argument
    if (opts == 2) getOpts();
    // we get the next arguments
    for (int i = 2; i <= argc; i++) {
      getArg(i);
    }
    // TODO skip whitespace?
    // if (_isMathMode) skipWhiteSpace();
  }
}

bool TeXParser::isValidName(const string& com) const {
  if (com.empty()) return false;
  if (com[0] != '\\') return false;

  char c = L'\0';
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

sptr<Atom> TeXParser::processEscape() {
  _spos = _pos;
  const string command = getCommand();

  if (command.length() == 0) return sptrOf<EmptyAtom>();

  auto mac = MacroInfo::get(command);
  if (mac != nullptr) {
    return processCommands(command, mac);
  }

  sptr<Formula> predef = Formula::get(command);
  if (predef != nullptr) return predef->_root;

  sptr<Atom> sym = SymbolAtom::get(command);
  if (sym != nullptr) return sym;

  // not a valid command or symbol or predefined Formula found
  if (!_isPartial) {
    throw ex_parse("Unknown symbol or command or predefined Formula: '" + command + "'");
  }
  auto rm = sptrOf<FontStyleAtom>(
    FontStyle::tt,
    false,
    Formula("\\text{{\\backslash}" + command + "}")._root
  );
  return sptrOf<ColorAtom>(rm, TRANSPARENT, RED);
}

sptr<Atom> TeXParser::processCommands(const string& cmd, MacroInfo* mac) {
  int opts = mac->_posOpts;

  Args args;
  getOptsArgs(mac->_argc, opts, args);
  // we promise the first argument is the command name itself
  args[0] = cmd;

  if (NewCommandMacro::isMacro(cmd)) {
    // The last value in "args" is the replacement string
    auto ret = mac->invoke(*this, args);
    insert(_spos, _pos, args.back());
    return ret;
  }

  return mac->invoke(*this, args);
}

sptr<Atom> TeXParser::getScripts(char first) {
  _pos++;
  // get the sub script (assume the first is the sub script)
  sptr<Atom> sub = getArgument();
  sptr<Atom> sup(nullptr);
  char second = '\0';

  if (_pos < _len) second = _latex[_pos];

  // 4 conditions:
  // 1. \cmd_{\sub}^{\super}
  // 2. \cmd^{\super}_{\sub}
  // 3. \cmd_{\sub}
  // 4. \cmd^{\super}
  if (first == SUPER_SCRIPT && second == SUPER_SCRIPT) {
    sup = sub;
    sub = nullptr;
  } else if (first == SUB_SCRIPT && second == SUPER_SCRIPT) {
    _pos++;
    sup = getArgument();
  } else if (first == SUPER_SCRIPT && second == SUB_SCRIPT) {
    _pos++;
    sup = sub;
    sub = getArgument();
  } else if (first == SUPER_SCRIPT && second != SUB_SCRIPT) {
    sup = sub;
    sub = nullptr;
  }

  sptr<Atom> atom;
  RowAtom* rm = nullptr;
  if (_formula->_root == nullptr) {
    // If there's no root exists, passing a null atom to ScriptsAtom as base is OK,
    // the ScriptsAtom will handle it
    return sptrOf<ScriptsAtom>(nullptr, sub, sup);
  } else if ((rm = dynamic_cast<RowAtom*>(_formula->_root.get()))) {
    atom = rm->popBack();
  } else {
    atom = _formula->_root;
    _formula->_root = nullptr;
  }

  // Check if previous atom is CumulativeScriptsAtom
  auto* ca = dynamic_cast<CumulativeScriptsAtom*>(atom.get());
  if (ca != nullptr) {
    ca->addSubscript(sub);
    ca->addSuperscript(sup);
    return atom;
  }

  if (atom->rightType() == AtomType::bigOperator) {
    return sptrOf<OperatorAtom>(atom, sub, sup);
  }

  return sptrOf<ScriptsAtom>(atom, sub, sup);
}

sptr<Atom> TeXParser::getArgument() {
  skipWhiteSpace();
  char ch;
  if (_pos < _len) ch = _latex[_pos];
  else return sptrOf<EmptyAtom>();

  if (ch == L_GROUP) {
    Formula tf;
    Formula* tmp = _formula;
    _formula = &tf;
    _pos++;
    _group++;
    parse();
    _formula = tmp;
    if (_formula->_root == nullptr) {
      auto* rm = new RowAtom();
      rm->add(tf._root);
      return sptr<Atom>(rm);
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

  auto atom = getCharAtom();
  _pos++;
  return atom;
}

Dimen TeXParser::getDimen() {
  if (_pos == _len) return {0.f, UnitType::none};

  char ch = L'\0';

  skipWhiteSpace();
  const int start = _pos;
  while (_pos < _len && ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n' && ch != ESCAPE) {
    ch = _latex[_pos++];
  }
  const int end = _pos;
  if (ch == '\\') _pos--;
  else skipWhiteSpace();

  return Units::getDimen(_latex.substr(start, end - start - 1));
}

void TeXParser::preprocess(string& cmd, Args& args, int& pos) {
  if (cmd == "newcommand" || cmd == "renewcommand") {
    preprocessNewCmd(cmd, args, pos);
  } else if (cmd == "newenvironment" || cmd == "renewenvironment") {
    preprocessNewCmd(cmd, args, pos);
  } else if (NewCommandMacro::isMacro(cmd)) {
    inflateNewCmd(cmd, args, pos);
  } else if (cmd == "begin") {
    inflateEnv(cmd, args, pos);
  } else if (cmd == "makeatletter") {
    _atIsLetter++;
  } else if (cmd == "makeatother") {
    _atIsLetter--;
  }
}

void TeXParser::preprocessNewCmd(string& cmd, Args& args, int& pos) {
  // The macro must exists
  auto mac = MacroInfo::get(cmd);
  getOptsArgs(mac->_argc, mac->_posOpts, args);
  mac->invoke(*this, args);
  _latex.erase(pos, _pos - pos);
  _len = _latex.length();
  _pos = pos;
}

void TeXParser::inflateNewCmd(string& cmd, Args& args, int& pos) {
  // The macro must exists
  auto mac = MacroInfo::get(cmd);
  getOptsArgs(mac->_argc, mac->_posOpts, args);
  args[0] = cmd;
  try {
    mac->invoke(*this, args);
    // The last element is the returned value (after inflated macro)
    _latex.replace(pos, _pos - pos, args.back());
  } catch (ex_parse& e) {
    if (!_isPartial) throw;
    pos += cmd.length() + 1;
  }
  _len = _latex.length();
  _pos = pos;
}

void TeXParser::inflateEnv(string& cmd, Args& args, int& pos) {
  getOptsArgs(1, 0, args);
  string env = args[1] + "@env";
  auto mac = MacroInfo::get(env);
  if (mac == nullptr) {
    throw ex_parse(
      "Unknown environment: "
      + args[1]
      + " at position " + tostring(getLine())
      + ":" + tostring(getCol())
    );
  }
  vector<string> optargs;
  getOptsArgs(mac->_argc - 1, 0, optargs);
  string grp = getGroup("\\begin{" + args[1] + "}", "\\end{" + args[1] + "}");
  string expr = "{\\makeatletter \\" + args[1] + "@env";
  for (int i = 1; i <= mac->_argc - 1; i++) expr += "{" + optargs[i] + "}";
  expr += "{" + grp + "}\\makeatother}";
  _latex.replace(pos, _pos - pos, expr);
  _len = _latex.length();
  _pos = pos;
}

void TeXParser::preprocess() {
  if (_len == 0) return;

  char ch;
  int spos;
  vector<string> args;
  while (_pos < _len) {
    ch = _latex[_pos];
    switch (ch) {
      case ESCAPE: {
        spos = _pos;
        string cmd = getCommand();
        try {
          preprocess(cmd, args, spos);
        } catch (ex_parse& e) {
          if (!_isPartial) throw e;
        }
        args.clear();
        break;
      }
      case PERCENT: {
        spos = _pos++;
        char chr;
        while (_pos < _len) {
          chr = _latex[_pos++];
          if (chr == '\r' || chr == '\n') break;
        }
        if (_pos < _len) _pos--;
        _latex.replace(spos, _pos - spos, "");
        _len = _latex.length();
        _pos = spos;
        break;
      }
      default:
        _pos++;
        break;
    }
  }
  _pos = 0;
  _len = _latex.length();
}

void TeXParser::parse() {
  if (_len == 0) {
    if (_formula->_root == nullptr && !_arrayMode)
      _formula->add(sptrOf<EmptyAtom>());
    return;
  }

  char ch;
  while (_pos < _len) {
    ch = _latex[_pos];

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
        if (!_isMathMode) {  // we are in text mode
          _formula->add(sptrOf<SpaceAtom>());
          _formula->add(sptrOf<BreakMarkAtom>());
          while (_pos < _len) {
            ch = _latex[_pos];
            if (ch != ' ' || ch != '\t' || ch != '\r') break;
            _pos++;
          }
        }
      }
        break;
      case DOLLAR: {
        _pos++;
        if (!_isMathMode) {  // we are in text mode
          TexStyle style = TexStyle::text;
          bool doubleDollar = false;
          if (_latex[_pos] == DOLLAR) {
            style = TexStyle::display;
            doubleDollar = true;
            _pos++;
          }

          auto atom = sptrOf<MathAtom>(
            Formula(*this, getDollarGroup(DOLLAR), false)._root,
            style
          );
          _formula->add(atom);
          if (doubleDollar) {
            if (_latex[_pos] == DOLLAR) _pos++;
          }
        }
      }
        break;
      case ESCAPE: {
        sptr<Atom> atom = processEscape();
        _formula->add(atom);
        auto* h = dynamic_cast<HlineAtom*>(atom.get());
        if (_arrayMode && h != nullptr) ((ArrayFormula*) _formula)->addRow();
        if (_insertion) _insertion = false;
      }
        break;
      case L_GROUP: {
        auto atom = getArgument();
        if (atom != nullptr) {
          atom->_type = AtomType::ordinary;
        }
        _formula->add(atom);
      }
        break;
      case R_GROUP: {
        _group--;
        _pos++;
        if (_group == -1) {
          throw ex_parse("Found a closing '}' without an opening '{'!");
        }
        // End of a group
        return;
      }
      case SUPER_SCRIPT:
      case SUB_SCRIPT: {
        if (_isMathMode) {
          _formula->add(getScripts(ch));
        } else {
          _formula->add(getCharAtom());
          _pos++;
        }
      }
        break;
      case '&': {
        if (!_arrayMode) {
          throw ex_parse("Character '&' is only available in array mode!");
        }
        ((ArrayFormula*) _formula)->addCol();
        _pos++;
      }
        break;
      case '~': {
        _formula->add(sptrOf<SpaceAtom>());
        _pos++;
      }
        break;
      case PRIME:
      case BACKPRIME: {
        // special case for ` and '
        if (_isMathMode) {
          auto atom = sptrOf<CumulativeScriptsAtom>(
            popLastAtom(),
            nullptr,
            getSimpleScripts(ch != BACKPRIME)
          );
          _formula->add(atom);
        } else {
          _formula->add(getCharAtom());
          _pos++;
        }
      }
        break;
      case DQUOTE: {
        if (_isMathMode) {
          auto atom = sptrOf<CumulativeScriptsAtom>(
            popLastAtom(),
            nullptr,
            sptrOf<CharAtom>(0x02033, _isMathMode)
          );
          _formula->add(atom);
        } else {
          _formula->add(getCharAtom());
          _pos++;
        }
      }
        break;
      default: {
        _formula->add(getCharAtom());
        _pos++;
      }
        break;
    }
  }
}

sptr<Atom> TeXParser::getCharAtom() {
  int i = _pos;
  const c32 a = tex::nextUnicode(_latex, i);
  int j = i + 1;
  const c32 b = tex::nextUnicode(_latex, j);
  if (tex::isVariationSelector(b)) {
    auto atom = sptrOf<TextAtom>(_latex.substr(_pos, j - _pos + 1), _isMathMode);
    _pos = j;
    return atom;
  }
  return getCharAtom(a);
}

sptr<Atom> TeXParser::getCharAtom(c32 chr) {
  const c32 code = tex::convertToRomanNumber(chr);
  if (_isMathMode) {
    const auto it = Formula::_charToSymbol.find(code);
    if (it != Formula::_charToSymbol.end()) {
      return SymbolAtom::get(it->second);
    }
  }
  return sptrOf<CharAtom>(code, _isMathMode);
}

sptr<Atom> TeXParser::getSimpleScripts(bool isPrime) {
  int count = 1;
  while (true) {
    ++_pos;
    if (_pos >= _len) break;
    const auto chr = _latex[_pos];
    if ((isPrime && chr != PRIME) || (!isPrime && chr != BACKPRIME)) {
      break;
    }
    ++count;
  }

  static const c32 primes[] = {0x02032, 0x02033, 0x02034, 0x02057};
  static const c32 backprimes[] = {0x02035, 0x02036, 0x02037};

  const auto arr = isPrime ? primes : backprimes;
  const auto cnt = isPrime ? 4 : 3;

  if (count <= cnt) {
    return sptrOf<CharAtom>(arr[count - 1], _isMathMode);
  }
  const auto row = sptrOf<RowAtom>();
  for (int i = 0; i < count; i++) {
    row->add(sptrOf<CharAtom>(arr[0], _isMathMode));
  }
  return row;
}
