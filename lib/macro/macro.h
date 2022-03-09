#ifndef MACRO_H_INCLUDED
#define MACRO_H_INCLUDED

#include "atom/atom.h"
#include "utils/utils.h"

#include <map>
#include <string>

namespace microtex {

class Parser;

class Macro {
public:
  virtual void execute(Parser& tp, std::vector<std::string>& args) = 0;

  virtual ~Macro() = default;
};

class NewCommandMacro : public Macro {
protected:
  static std::map<std::string, std::string> _codes;
  static std::map<std::string, std::string> _replacements;
  static Macro* _instance;

  static void checkNew(const std::string& name);

  static void checkRenew(const std::string& name);

public:
  /**
   * If notify a fatal error when defining a new command but it has been
   * defined already or redefine a command but it has not been defined,
   * default is true.
   */
  static bool _errIfConflict;

  void execute(Parser& tp, std::vector<std::string>& args) override;

  static void addNewCommand(
    const std::string& name,
    const std::string& code,
    int argc
  );

  static void addNewCommand(
    const std::string& name,
    const std::string& code,
    int argc,
    const std::string& def
  );

  static void addRenewCommand(
    const std::string& name,
    const std::string& code,
    int argc
  );

  static void addRenewCommand(
    const std::string& name,
    const std::string& code,
    int argc,
    const std::string& def
  );

  static bool isMacro(const std::string& name);

  static void _init_();

  static void _free_();

  ~NewCommandMacro() override = default;
};

class NewEnvironmentMacro : public NewCommandMacro {
public:
  static void addNewEnvironment(
    const std::string& name,
    const std::string& begDef,
    const std::string& endDef,
    int argc
  );

  static void addRenewEnvironment(
    const std::string& name,
    const std::string& begDef,
    const std::string& endDef,
    int argc
  );
};

class MacroInfo {
public:
  static std::map<std::string, MacroInfo*> _commands;

  /** Add a macro, replace it if the macro is exists. */
  static void add(const std::string& name, MacroInfo* mac);

  /** Get the macro info from given name, return nullptr if not found. */
  static MacroInfo* get(const std::string& name);

  // Number of arguments
  const int _argc;
  // Options' position, can be  0, 1 and 2
  // 0 represents this macro has no options
  // 1 represents the options appear after the command name, e.g.:
  //      \sqrt[3]{2}
  // 2 represents the options appear after the first argument, e.g.:
  //      \scalebox{0.5}[2]{\LaTeX}
  const int _posOpts;

  no_copy_assign(MacroInfo);

  MacroInfo() : _argc(0), _posOpts(0) {}

  MacroInfo(int argc, int posOpts) : _argc(argc), _posOpts(posOpts) {}

  explicit MacroInfo(int argc) : _argc(argc), _posOpts(0) {}

  virtual sptr<Atom> invoke(
    Parser& tp,
    std::vector<std::string>& args) {
    return nullptr;
  }

  virtual ~MacroInfo() = default;

  static void _free_();
};

class InflationMacroInfo : public MacroInfo {
private:
  // The actual macro to execute
  Macro* const _macro;

public:
  no_copy_assign(InflationMacroInfo);

  InflationMacroInfo(Macro* macro, int argc)
    : _macro(macro), MacroInfo(argc) {}

  InflationMacroInfo(Macro* macro, int argc, int posOpts)
    : _macro(macro), MacroInfo(argc, posOpts) {}

  sptr<Atom> invoke(
    Parser& tp,
    std::vector<std::string>& args
  ) override {
    _macro->execute(tp, args);
    return nullptr;
  }
};

typedef sptr<Atom> (* MacroDelegate)(
  Parser& tp,
  std::vector<std::string>& args
);

class PreDefMacro : public MacroInfo {
private:
  MacroDelegate _delegate;

public:
  no_copy_assign(PreDefMacro);

  PreDefMacro(int argc, int posOpts, MacroDelegate delegate)
    : MacroInfo(argc, posOpts), _delegate(delegate) {}

  PreDefMacro(int argc, MacroDelegate delegate)
    : MacroInfo(argc), _delegate(delegate) {}

  sptr<Atom> invoke(
    Parser& tp,
    std::vector<std::string>& args
  ) override;
};

}  // namespace microtex

#endif  // MACRO_H_INCLUDED
