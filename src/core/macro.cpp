#include "core/macro.h"
#include "common.h"
#include "core/macro_impl.h"

#include <string>

using namespace std;
using namespace tex;

bool NewCommandMacro::_errIfConflict = true;

bool NewCommandMacro::isMacro(const wstring& name) {
  auto it = _macrocode.find(name);
  return (it != _macrocode.end());
}

void NewCommandMacro::checkNew(const wstring& name) {
  if (_errIfConflict && isMacro(name)) throw ex_parse(
      "Command " + wide2utf8(name.c_str()) +
      " already exists! Use renewcommand instead!");
}

void NewCommandMacro::checkRenew(const wstring& name) {
  if (NewCommandMacro::_errIfConflict && !isMacro(name)) throw ex_parse(
      "Command " + wide2utf8(name.c_str()) +
      " is no defined! Use newcommand instead!");
}

void NewCommandMacro::addNewCommand(
    const wstring& name, const wstring& code, int nbargs) {
  checkNew(name);
  _macrocode[name] = code;
  MacroInfo::addMacro(name, new InflationMacroInfo(_instance, nbargs));
}

void NewCommandMacro::addNewCommand(
    const wstring& name,
    const wstring& code,
    int nbargs,
    const wstring& def) {
  checkNew(name);
  _macrocode[name] = code;
  _macroreplacement[name] = def;
  MacroInfo::addMacro(name, new InflationMacroInfo(_instance, nbargs, 1));
}

void NewCommandMacro::addRenewCommand(
    const wstring& name, const wstring& code, int nbargs) {
  checkRenew(name);
  _macrocode[name] = code;
  MacroInfo::addMacro(name, new InflationMacroInfo(_instance, nbargs));
}

void NewCommandMacro::addRenewCommand(
    const wstring& name,
    const wstring& code,
    int nbargs,
    const wstring& def) {
  checkRenew(name);
  _macrocode[name] = code;
  _macroreplacement[name] = def;
  MacroInfo::addMacro(name, new InflationMacroInfo(_instance, nbargs, 1));
}

void NewCommandMacro::execute(_out_ TeXParser& tp, _out_ vector<wstring>& args) {
  wstring code = _macrocode[args[0]];
  wstring rep;
  int nbargs = args.size() - 12;
  int dec = 0;

  auto it = _macroreplacement.find(args[0]);

  // FIXME
  // Keep slash "\" and dollar "$" signs?
  // Example:
  //      \newcommand{\cmd}[2][\sqrt{e^x}]{ #2 - #1 }
  // we want the optional argument "\sqrt{e^x}" keep the slash sign
  if (!args[nbargs + 1].empty()) {
    dec = 1;
    // quotereplace(args[nbargs + 1], rep);
    replaceall(code, L"#1", args[nbargs + 1]);
  } else if (it != _macroreplacement.end()) {
    dec = 1;
    // quotereplace(it->second, rep);
    replaceall(code, L"#1", it->second);
  }

  for (int i = 1; i <= nbargs; i++) {
    rep = args[i];
    replaceall(code, L"#" + towstring(i + dec), rep);
  }
  // push back as returned value (inflated macro)
  args.push_back(code);
}

void NewEnvironmentMacro::addNewEnvironment(
    const wstring& name,
    const wstring& begdef, const wstring& enddef,
    int nbargs) {
  wstring n = name + L"@env";
  wstring def = begdef + L" #" + towstring(nbargs + 1) + L" " + enddef;
  addNewCommand(n, def, nbargs + 1);
}

void NewEnvironmentMacro::addRenewEnvironment(
    const wstring& name,
    const wstring& begdef, const wstring& enddef,
    int nbargs) {
  if (_macrocode.find(name + L"@env") == _macrocode.end()) {
    throw ex_parse(
        "Environment " + wide2utf8(name.c_str()) +
        "is not defined! Use newenvironment instead!");
  }
  addRenewCommand(
      name + L"@env",
      begdef + L" #" + towstring(nbargs + 1) + L" " + enddef,
      nbargs + 1);
}

void NewCommandMacro::_free_() {
  delete _instance;
}

void MacroInfo::addMacro(const wstring& name, MacroInfo* mac) {
  auto it = _commands.find(name);
  if (it != _commands.end()) delete it->second;
  _commands[name] = mac;
}

void MacroInfo::_free_() {
  for (auto i : _commands) delete i.second;
}

sptr<Atom> PredefMacroInfo::invoke(
    _out_ TeXParser& tp,
    _out_ vector<wstring>& args) {
  try {
    return _delegate(tp, args);
  } catch (ex_parse& e) {
    throw ex_parse(
        "Problem with command " +
        wide2utf8(args[0].c_str()) +
        " at position " + tostring(tp.getLine()) + ":" +
        tostring(tp.getCol()) + "\n caused by: " + e.what());
  }
}
