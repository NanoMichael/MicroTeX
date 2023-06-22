#include "macro/macro_misc.h"

#include "atom/atom_font.h"
#include "atom/atom_zstack.h"
#include "core/debug_config.h"
#include "env/env.h"
#include "env/units.h"
#include "graphic/graphic.h"

using namespace microtex;
using namespace std;

namespace microtex {

macro(longdiv) {
  long dividend = 0;
  valueOf(args[1], dividend);
  long divisor = 0;
  valueOf(args[2], divisor);
  if (divisor == 0) throw ex_parse("Divisor must not be 0.");
  return sptrOf<LongDivAtom>(divisor, dividend);
}

macro(char) {
  // clang-format off
  const auto& str = tp.forward([](char ch) {
    return ch == '\'' || ch == '"'
           || (ch >= '0' && ch <= '9')
           || (ch >= 'a' && ch <= 'z')
           || (ch >= 'A' && ch <= 'Z');
  });
  // clang-format on
  int radix = 10;
  int offset = 0;
  if (startsWith(str, "'")) {
    radix = 8;
    offset = 1;
  } else if (startsWith(str, "\"")) {
    radix = 16;
    offset = 1;
  }
  int n = 0;
  str2int(str.c_str() + offset, str.length() - offset, n, radix);
  return tp.getCharAtom(n);
}

macro(cr) {
  if (tp.isArrayMode()) {
    tp.addRow();
  } else {
    ArrayFormula arr;
    arr.add(tp._formula->_root);
    arr.addRow();
    Parser parser(tp.isPartial(), tp.forwardBalancedGroup(), &arr, false, tp.isMathMode());
    parser.parse();
    arr.checkDimensions();
    tp._formula->_root = arr.getAsVRow();
  }

  return nullptr;
}

macro(kern) {
  auto [value, unit] = tp.getDimen();
  return sptrOf<SpaceAtom>(unit, value, 0.f, 0.f);
}

macro(hvspace) {
  auto [value, unit] = Units::getDimen(args[1]);
  return args[0][0] == 'h' ? sptrOf<SpaceAtom>(unit, value, 0.f, 0.f)
                           : sptrOf<SpaceAtom>(unit, 0.f, value, 0.f);
}

macro(rule) {
  auto w = Units::getDimen(args[1]);
  auto h = Units::getDimen(args[2]);
  auto r = Units::getDimen(args[3]);

  return sptrOf<RuleAtom>(w, h, -r);
}

template <typename F>
sptr<Atom> _def_cmd(Parser& tp, Args& args, F&& f) {
  string newcmd(args[1]);
  int argc = 0;
  if (!tp.isValidCmd(newcmd)) throw ex_parse("Invalid name for the command '" + newcmd);

  if (!args[3].empty()) valueOf(args[3], argc);

  const bool hasOption = !args[4].empty();
  if (hasOption && argc > 0) --argc;

  f(hasOption, newcmd.substr(1), args[2], argc, args[4]);

  return nullptr;
}

macro(newcommand) {
  return _def_cmd(
    tp,
    args,
    [](bool hasOption, const string& cmd, const string& def, int argc, const string& opt) {
      if (!hasOption) {
        NewCommandMacro::addNewCommand(cmd, def, argc);
      } else {
        NewCommandMacro::addNewCommand(cmd, def, argc, opt);
      }
    }
  );
}

macro(renewcommand) {
  return _def_cmd(
    tp,
    args,
    [](bool hasOption, const string& cmd, const string& def, int argc, const string& opt) {
      if (!hasOption) {
        NewCommandMacro::addRenewCommand(cmd, def, argc);
      } else {
        NewCommandMacro::addRenewCommand(cmd, def, argc, opt);
      }
    }
  );
}

macro(raisebox) {
  auto r = Units::getDimen(args[1]);
  auto h = Units::getDimen(args[3]);
  auto d = Units::getDimen(args[4]);
  return sptrOf<RaiseAtom>(Formula(tp, args[2], false, tp.isMathMode())._root, -r, h, d);
}

macro(romannumeral) {
  static const int numbers[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
  static const string letters[] =
    {"M", "CM", "D", "CD", "C", "XC", "", "XL", "X", "IX", "V", "IV", "I"};
  string roman;

  int num;
  valueOf(trim(args[1]), num);
  for (int i = 0; i < 13; i++) {
    while (num >= numbers[i]) {
      roman += letters[i];
      num -= numbers[i];
    }
  }

  if (args[0][0] == 'r') {
    toLower(roman);
  }

  return sptrOf<FontStyleAtom>(
    FontStyle::rm,
    tp.isMathMode(),
    Formula(tp, roman, false, tp.isMathMode())._root
  );
}

macro(debug) {
  auto& config = DebugConfig::INSTANCE;
  const auto& options = parseOption(args[1]);
  config.enable = true;
  const auto& showOnlyChar = options.find("showonlychar");
  if (showOnlyChar != options.end()) {
    config.showOnlyChar = showOnlyChar->second == "true";
  }
  const auto& boundColor = options.find("boundcolor");
  if (boundColor != options.end()) {
    config.boundColor = ColorAtom::getColor(boundColor->second);
  }
  const auto& baselineColor = options.find("baselinecolor");
  if (baselineColor != options.end()) {
    config.baselineColor = ColorAtom::getColor(baselineColor->second);
  }
  return nullptr;
}

macro(undebug) {
  auto& config = DebugConfig::INSTANCE;
  config.enable = false;
  return nullptr;
}

macro(zstack) {
  auto halign = Alignment::left;
  if (args[1] == "c") {
    halign = Alignment::center;
  } else if (args[1] == "r") {
    halign = Alignment::right;
  }
  const auto& h = Units::getDimen(args[2]);
  const ZStackArgs hargs{halign, h};

  auto valign = Alignment::top;
  if (args[3] == "c") {
    valign = Alignment::center;
  } else if (args[3] == "b") {
    valign = Alignment::bottom;
  } else if (args[3] == "B") {
    valign = Alignment::none;
  }
  const auto& v = Units::getDimen(args[4]);
  const ZStackArgs& vargs{valign, v};

  const auto atom = Formula(tp, args[5], false)._root;
  const auto anchor = Formula(tp, args[6], false)._root;

  return sptrOf<ZStackAtom>(hargs, vargs, atom, anchor);
}

}  // namespace microtex
