#include "core/macro.h"
#include "common.h"
#include "core/macro_impl.h"

#include <string>

using namespace std;
using namespace tex;

bool NewCommandMacro::isMacro(const wstring& name) {
    auto it = _macrocode.find(name);
    return (it != _macrocode.end());
}

void NewCommandMacro::addNewCommand(
    const wstring& name, const wstring& code, int nbargs) throw(ex_parse) {
    _macrocode[name] = code;
    auto x = MacroInfo::_commands.find(name);
    if (x != MacroInfo::_commands.end()) delete x->second;
    MacroInfo::_commands[name] = new MacroInfo(_instance, nbargs);
}

void NewCommandMacro::addNewCommand(
    const wstring& name,
    const wstring& code,
    int nbargs,
    const wstring& def) throw(ex_parse) {
    auto it = _macrocode.find(name);
    if (it != _macrocode.end()) {
        throw ex_parse(
            "Command " + wide2utf8(name.c_str()) +
            " already exists! Use renewcommand instead!");
    }
    _macrocode[name] = code;
    _macroreplacement[name] = def;
    MacroInfo::_commands[name] = new MacroInfo(_instance, nbargs, 1);
}

void NewCommandMacro::addRenewCommand(
    const wstring& name, const wstring& code, int nbargs) throw(ex_parse) {
    if (!isMacro(name)) {
        throw ex_parse(
            "Command " + wide2utf8(name.c_str()) +
            " is no defined! Use newcommand instead!");
    }
    _macrocode[name] = code;
    delete MacroInfo::_commands[name];
    MacroInfo::_commands[name] = new MacroInfo(_instance, nbargs);
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
    int nbargs) throw(ex_parse) {
    wstring n = name + L"@env";
    wstring def = begdef + L" #" + towstring(nbargs + 1) + L" " + enddef;
    addNewCommand(n, def, nbargs + 1);
}

void NewEnvironmentMacro::addRenewEnvironment(
    const wstring& name,
    const wstring& begdef, const wstring& enddef,
    int nbargs) throw(ex_parse) {
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

void MacroInfo::_free_() {
    for (auto i : _commands) delete i.second;
}

sptr<Atom> PredefMacroInfo::invoke(
    _out_ TeXParser& tp,
    _out_ vector<wstring>& args) throw(ex_parse) {
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
