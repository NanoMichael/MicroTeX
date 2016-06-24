#ifndef MACRO_H_INCLUDED
#define MACRO_H_INCLUDED

#include <string>
#include <map>
#include "common.h"

#if defined (__clang__)
#include "atom.h"
#elif defined (__GNUC__)
#include "atom/atom.h"
#endif // defined

using namespace tex;
using namespace std;

namespace tex {
namespace core {

// forward declare
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

	static void addNewCommand(const wstring& name, const wstring& code, int nbargs) throw(ex_parse);

	static void addNewCommand(const wstring& name, const wstring& code, int nbargs, const wstring& def) throw(ex_parse);

	static void addRenewCommand(const wstring& name, const wstring& code, int nbargs) throw(ex_parse);

	static bool isMacro(const wstring& name);

	static void _init_();

	static void _free_();

	virtual ~NewCommandMacro() {}
};

class NewEnvironmentMacro : public NewCommandMacro {
public:
	static void addNewEnvironment(const wstring& name, const wstring& begdef, const wstring& enddef, int nbargs) throw(ex_parse);

	static void addRenewEnvironment(const wstring& name, const wstring& begdef, const wstring& enddef, int nbargs) throw(ex_parse);
};

class MacroInfo {
public:
	static map<wstring, MacroInfo*> _commands;
	Macro* _macro;
	int _nbArgs;
	bool _hasOptions;
	int _posOpts;

	MacroInfo() :
		_macro(nullptr), _nbArgs(0), _hasOptions(false), _posOpts(0) {
	}

	MacroInfo(Macro* macro, int nbargs) :
		_macro(macro), _nbArgs(nbargs), _hasOptions(false), _posOpts(0) {
	}

	MacroInfo(Macro* macro, int nbargs, int posOpts) :
		_macro(macro), _nbArgs(nbargs), _hasOptions(true), _posOpts(posOpts) {
	}

	MacroInfo(int nbargs, int posOpts) :
		_macro(nullptr), _nbArgs(nbargs), _hasOptions(true), _posOpts(posOpts) {
	}

	MacroInfo(int nbargs) :
		_macro(nullptr), _nbArgs(nbargs), _hasOptions(false), _posOpts(0) {
	}

	virtual shared_ptr<Atom> invoke(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) {
		_macro->execute(tp, args);
		return shared_ptr<Atom>(nullptr);
	}

	virtual ~MacroInfo() {}

	static void _free_();
};

class PredefMacroInfo : public MacroInfo {
private:
	int _id;

	static shared_ptr<Atom> invoke(int id, _out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse);
public:
	PredefMacroInfo() = delete;

	PredefMacroInfo(int id, int nbargs, int posOpts) :
		MacroInfo(nbargs, posOpts), _id(id) {
	}

	PredefMacroInfo(int id, int nbargs) :
		MacroInfo(nbargs), _id(id) {
	}

	shared_ptr<Atom> invoke(_out_ TeXParser& tp, _out_ vector<wstring>& args) throw(ex_parse) override {
		return invoke(_id, tp, args);
	}
};

}
}

#endif // MACRO_H_INCLUDED
