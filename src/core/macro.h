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

    static void checkNew(const wstring& name) throw(ex_parse);

    static void checkRenew(const wstring& name) throw(ex_parse);

public:
    /**
     * If notify a fatal error when defining a new command but it has been
     * defined already or redefine a command but it has not been defined,
     * default is true.
     */
    static bool _errIfConflict;

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

    static void addRenewCommand(
        const wstring& name,
        const wstring& code,
        int nbargs,
        const wstring& def) throw(ex_parse);

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

    /**
     * Add a macro, replace it if the macro is exists.
     */
    static void addMacro(const wstring& name, MacroInfo* mac);

    // Number of arguments
    const int _nbArgs;
    // Options' position, can be  0, 1 and 2
    // 0 represetns this macro has no options
    // 1 represents the options appear after the command name, e.g.:
    //      \sqrt[3]{2}
    // 2 represents the options appear after the first argument, e.g.:
    //      \scalebox{0.5}[2]{\LaTeX}
    const int _posOpts;

    MacroInfo() : _nbArgs(0), _posOpts(0) {}

    MacroInfo(int nbargs, int posOpts) : _nbArgs(nbargs), _posOpts(posOpts) {}

    MacroInfo(int nbargs) : _nbArgs(nbargs), _posOpts(0) {}

    inline bool hasOptions() const {
        return _posOpts != 0;
    }

    virtual sptr<Atom> invoke(
        _out_ TeXParser& tp,
        _out_ vector<wstring>& args) throw(ex_parse) {
        return nullptr;
    }

    virtual ~MacroInfo() {}

    static void _free_();
};

class InflationMacroInfo : public MacroInfo {
private:
    // The actual macro to execute
    Macro* const _macro;

public:
    InflationMacroInfo(Macro* macro, int nbargs)
        : _macro(macro), MacroInfo(nbargs) {}

    InflationMacroInfo(Macro* macro, int nbargs, int posOpts)
        : _macro(macro), MacroInfo(nbargs, posOpts) {}

    virtual sptr<Atom> invoke(
        _out_ TeXParser& tp,
        _out_ vector<wstring>& args) throw(ex_parse) override {
        _macro->execute(tp, args);
        return nullptr;
    }
};

typedef sptr<Atom> (*MacroDelegate)(
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

    sptr<Atom> invoke(
        _out_ TeXParser& tp,
        _out_ vector<wstring>& args) throw(ex_parse) override;
};

}  // namespace tex

#endif  // MACRO_H_INCLUDED
