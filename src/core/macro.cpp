#include "core/macro.h"
#include "common.h"
#include "core/macro_impl.h"

#include <string>

using namespace std;
using namespace tex;

bool NewCommandMacro::_errIfConflict = true;

bool NewCommandMacro::isMacro(const wstring& name) {
  auto it = _codes.find(name);
  return (it != _codes.end());
}

void NewCommandMacro::checkNew(const wstring& name) {
  if (_errIfConflict && isMacro(name))
    throw ex_parse(
      "Command " + wide2utf8(name)
      + " already exists! Use renewcommand instead!"
    );
}

void NewCommandMacro::checkRenew(const wstring& name) {
  if (NewCommandMacro::_errIfConflict && !isMacro(name))
    throw ex_parse(
      "Command " + wide2utf8(name)
      + " is no defined! Use newcommand instead!"
    );
}

void NewCommandMacro::addNewCommand(const wstring& name, const wstring& code, int argc) {
  checkNew(name);
  _codes[name] = code;
  MacroInfo::add(name, new InflationMacroInfo(_instance, argc));
}

void NewCommandMacro::addNewCommand(
  const wstring& name,
  const wstring& code,
  int argc,
  const wstring& def
) {
  checkNew(name);
  _codes[name] = code;
  _replacements[name] = def;
  MacroInfo::add(name, new InflationMacroInfo(_instance, argc, 1));
}

void NewCommandMacro::addRenewCommand(const wstring& name, const wstring& code, int argc) {
  checkRenew(name);
  _codes[name] = code;
  MacroInfo::add(name, new InflationMacroInfo(_instance, argc));
}

void NewCommandMacro::addRenewCommand(
  const wstring& name,
  const wstring& code,
  int argc,
  const wstring& def
) {
  checkRenew(name);
  _codes[name] = code;
  _replacements[name] = def;
  MacroInfo::add(name, new InflationMacroInfo(_instance, argc, 1));
}

void NewCommandMacro::execute(TeXParser& tp, vector<wstring>& args) {
  wstring code = _codes[args[0]];
  wstring rep;
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
    replaceall(code, L"#1", args[argc + 1]);
  } else if (it != _replacements.end()) {
    dec = 1;
    // quotereplace(it->second, rep);
    replaceall(code, L"#1", it->second);
  }

  for (int i = 1; i <= argc; i++) {
    rep = args[i];
    replaceall(code, L"#" + towstring(i + dec), rep);
  }
  // push back as returned value (inflated macro)
  args.push_back(code);
}

void NewEnvironmentMacro::addNewEnvironment(
  const wstring& name,
  const wstring& begDef, const wstring& endDef,
  int argc
) {
  wstring n = name + L"@env";
  wstring def = begDef + L" #" + towstring(argc + 1) + L" " + endDef;
  addNewCommand(n, def, argc + 1);
}

void NewEnvironmentMacro::addRenewEnvironment(
  const wstring& name,
  const wstring& begDef, const wstring& endDef,
  int argc
) {
  if (_codes.find(name + L"@env") == _codes.end()) {
    throw ex_parse(
      "Environment " + wide2utf8(name)
      + "is not defined! Use newenvironment instead!"
    );
  }
  addRenewCommand(
    name + L"@env",
    begDef + L" #" + towstring(argc + 1) + L" " + endDef,
    argc + 1
  );
}

void NewCommandMacro::_free_() {
  delete _instance;
}

void MacroInfo::add(const wstring& name, MacroInfo* mac) {
  auto it = _commands.find(name);
  if (it != _commands.end()) delete it->second;
  _commands[name] = mac;
}

MacroInfo* MacroInfo::get(const std::wstring& name) {
  auto it = _commands.find(name);
  if (it == _commands.end()) return nullptr;
  return it->second;
}

void MacroInfo::_free_() {
  for (const auto& i : _commands) delete i.second;
}

sptr<Atom> PreDefMacro::invoke(
  TeXParser& tp,
  vector<wstring>& args
) {
  try {
    return _delegate(tp, args);
  } catch (ex_parse& e) {
    throw ex_parse(
      "Problem with command "
      + wide2utf8(args[0])
      + " at position " + tostring(tp.getLine()) + ":"
      + tostring(tp.getCol()) + "\n caused by: " + e.what()
    );
  }
}
