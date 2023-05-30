#include "macro/macro.h"

#include <string>

#include "macro/macro_misc.h"

using namespace std;
using namespace microtex;

bool NewCommandMacro::_errIfConflict = true;

bool NewCommandMacro::isMacro(const string& name) {
  auto it = _codes.find(name);
  return (it != _codes.end());
}

void NewCommandMacro::checkNew(const string& name) {
  if (_errIfConflict && isMacro(name))
    throw ex_parse("Command " + name + " already exists! Use renewcommand instead!");
}

void NewCommandMacro::checkRenew(const string& name) {
  if (NewCommandMacro::_errIfConflict && !isMacro(name))
    throw ex_parse("Command " + name + " is no defined! Use newcommand instead!");
}

void NewCommandMacro::addNewCommand(const string& name, const string& code, int argc) {
  checkNew(name);
  _codes[name] = code;
  MacroInfo::add(name, new InflationMacroInfo(_instance, argc));
}

void NewCommandMacro::addNewCommand(
  const string& name,
  const string& code,
  int argc,
  const string& def
) {
  checkNew(name);
  _codes[name] = code;
  _replacements[name] = def;
  MacroInfo::add(name, new InflationMacroInfo(_instance, argc, 1));
}

void NewCommandMacro::addRenewCommand(const string& name, const string& code, int argc) {
  checkRenew(name);
  _codes[name] = code;
  MacroInfo::add(name, new InflationMacroInfo(_instance, argc));
}

void NewCommandMacro::addRenewCommand(
  const string& name,
  const string& code,
  int argc,
  const string& def
) {
  checkRenew(name);
  _codes[name] = code;
  _replacements[name] = def;
  MacroInfo::add(name, new InflationMacroInfo(_instance, argc, 1));
}

void NewCommandMacro::execute(Parser& tp, vector<string>& args) {
  string code = _codes[args[0]];
  string rep;
  size_t argc = args.size() - 12;
  int dec = 0;

  auto it = _replacements.find(args[0]);

  // FIXME
  // Keep slash "\" and dollar "$" signs?
  // Example:
  //      \newcommand{\cmd}[2][\sqrt{e^x}]{ #2 - #1 }
  // we want the optional argument "\sqrt{e^x}" keep the slash sign
  if (!args[argc + 1].empty()) {
    dec = 1;
    // quotereplace(args[argc + 1], rep);
    replaceAll(code, "#1", args[argc + 1]);
  } else if (it != _replacements.end()) {
    dec = 1;
    // quotereplace(it->second, rep);
    replaceAll(code, "#1", it->second);
  }

  for (int i = 1; i <= argc; i++) {
    rep = args[i];
    replaceAll(code, "#" + toString(i + dec), rep);
  }
  // push back as returned value (inflated macro)
  args.push_back(code);
}

void NewEnvironmentMacro::addNewEnvironment(
  const string& name,
  const string& begDef,
  const string& endDef,
  int argc
) {
  string n = name + "@env";
  string def = begDef + " #" + toString(argc + 1) + " " + endDef;
  addNewCommand(n, def, argc + 1);
}

void NewEnvironmentMacro::addRenewEnvironment(
  const string& name,
  const string& begDef,
  const string& endDef,
  int argc
) {
  if (_codes.find(name + "@env") == _codes.end()) {
    throw ex_parse("Environment " + name + "is not defined! Use newenvironment instead!");
  }
  addRenewCommand(name + "@env", begDef + " #" + toString(argc + 1) + " " + endDef, argc + 1);
}

void NewCommandMacro::_free_() {
  delete _instance;
}

void MacroInfo::add(const string& name, MacroInfo* mac) {
  auto it = _commands.find(name);
  if (it != _commands.end()) delete it->second;
  _commands[name] = mac;
}

MacroInfo* MacroInfo::get(const std::string& name) {
  auto it = _commands.find(name);
  if (it == _commands.end()) return nullptr;
  return it->second;
}

void MacroInfo::_free_() {
  for (const auto& i : _commands) delete i.second;
}

sptr<Atom> PreDefMacro::invoke(Parser& tp, vector<string>& args) {
  try {
    return _delegate(tp, args);
  } catch (ex_parse& e) {
    throw ex_parse("Problem with command: " + args[0] + "\n caused by: " + e.what());
  }
}
