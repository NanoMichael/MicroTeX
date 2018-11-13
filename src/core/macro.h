#ifndef MACRO_H_INCLUDED
#define MACRO_H_INCLUDED

#include "atom/atom.h"
#include "common.h"

#include <map>
#include <string>

using namespace tex;
using namespace std;

namespace tex {

class TeXParser;

class Macro {
public:
    virtual void execute(_out_ TeXParser& tp, _out_ vector<wstring>& args) = 0;

    virtual ~Macro() {}
};

class NewCommandMacro : public Macro {
protected:
    static map<wstring, wstring> _macrocode;
    static map<wstring, wstring> _macroreplacement;
    static Macro* _instance;

public:
    virtual void execute(_out_ TeXParser& tp, _out_ vector<wstring>& args) override;

    static void addNewCommand(
        const wstring& name,
        const wstring& code,
        int nbargs) throw(ex_parse);

    static void addNewCommand(
        const wstring& name,
        const wstring& code,
        int nbargs,
        const wstring& def) throw(ex_parse);

    static void addRenewCommand(
        const wstring& name,
        const wstring& code,
        int nbargs) throw(ex_parse);

    static bool isMacro(const wstring& name);

    static void _init_();

    static void _free_();

    virtual ~NewCommandMacro() {}
};

class NewEnvironmentMacro : public NewCommandMacro {
public:
    static void addNewEnvironment(
        const wstring& name,
        const wstring& begdef,
        const wstring& enddef,
        int nbargs) throw(ex_parse);

    static void addRenewEnvironment(
        const wstring& name,
        const wstring& begdef,
        const wstring& enddef,
        int nbargs) throw(ex_parse);
};

class MacroInfo {
public:
    static map<wstring, MacroInfo*> _commands;
    // The actual macro to execute
    Macro* _macro;
    // Number of arguments
    int _nbArgs;
    // If has options
    bool _hasOptions;
    // Options' position
    int _posOpts;

    MacroInfo()
        : _macro(nullptr), _nbArgs(0), _hasOptions(false), _posOpts(0) {}

    MacroInfo(Macro* macro, int nbargs)
        : _macro(macro), _nbArgs(nbargs), _hasOptions(false), _posOpts(0) {}

    MacroInfo(Macro* macro, int nbargs, int posOpts)
        : _macro(macro), _nbArgs(nbargs), _hasOptions(true), _posOpts(posOpts) {}

    MacroInfo(int nbargs, int posOpts)
        : _macro(nullptr), _nbArgs(nbargs), _hasOptions(true), _posOpts(posOpts) {}

    MacroInfo(int nbargs)
        : _macro(nullptr), _nbArgs(nbargs), _hasOptions(false), _posOpts(0) {}

    virtual shared_ptr<Atom> invoke(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
        _macro->execute(tp, args);
        return shared_ptr<Atom>(nullptr);
    }

    virtual ~MacroInfo() {}

    static void _free_();
};

typedef shared_ptr<Atom> (*MacroDelegate)(
    _out_ TeXParser& tp,
    _out_ vector<wstring>& args);

class PredefMacroInfo : public MacroInfo {
private:
    MacroDelegate _delegate;

public:
    PredefMacroInfo() = delete;

    PredefMacroInfo(int nbargs, int posOpts, MacroDelegate delegate)
        : MacroInfo(nbargs, posOpts), _delegate(delegate) {}

    PredefMacroInfo(int nbargs, MacroDelegate delegate)
        : MacroInfo(nbargs), _delegate(delegate) {}

    shared_ptr<Atom> invoke(
        _out_ TeXParser& tp,
        _out_ vector<wstring>& args) throw(ex_parse) override;
};

}  // namespace tex

#endif  // MACRO_H_INCLUDED
