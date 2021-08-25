#include "macro/macro_misc.h"

#include <memory>

#include "env/env.h"
#include "env/units.h"
#include "graphic/graphic.h"
#include "core/debug_config.h"
#include "atom/atom_zstack.h"

using namespace tex;
using namespace std;

namespace tex {

macro(char) {
  // TODO
  std::string x = args[1];
  int radix = 10;
  if (startswith(x, "0x") || startswith(x, "0X")) {
    x = x.substr(2);
    radix = 16;
  } else if (startswith(x, "x") || startswith(x, "X")) {
    x = x.substr(1);
    radix = 16;
  } else if (startswith(x, "0")) {
    x = x.substr(1);
    radix = 8;
  }
  int n = 0;
  str2int(x, n, radix);
  return tp.getCharAtom(n);
}

macro(cr) {
  if (tp.isArrayMode()) {
    tp.addRow();
  } else {
    ArrayFormula arr;
    arr.add(tp._formula->_root);
    arr.addRow();
    TeXParser parser(
      tp.isPartial(),
      tp.forwardBalancedGroup(),
      &arr,
      false,
      tp.isMathMode()
    );
    parser.parse();
    arr.checkDimensions();
    tp._formula->_root = arr.getAsVRow();
  }

  return nullptr;
}

macro(kern) {
  auto[value, unit] = tp.getDimen();
  return sptrOf<SpaceAtom>(unit, value, 0.f, 0.f);
}

macro(hvspace) {
  auto[value, unit] = Units::getDimen(args[1]);
  return (
    args[0][0] == L'h'
    ? sptrOf<SpaceAtom>(unit, value, 0.f, 0.f)
    : sptrOf<SpaceAtom>(unit, 0.f, value, 0.f)
  );
}

macro(rule) {
  auto w = Units::getDimen(args[1]);
  auto h = Units::getDimen(args[2]);
  auto r = Units::getDimen(args[3]);

  return sptrOf<RuleAtom>(w, h, -r);
}

macro(newcommand) {
  string newcmd(args[1]);
  int argc = 0;
  if (!tp.isValidName(newcmd))
    throw ex_parse("Invalid name for the command '" + newcmd);

  if (!args[3].empty()) valueof(args[3], argc);

  if (args[4].empty()) {
    NewCommandMacro::addNewCommand(newcmd.substr(1), args[2], argc);
  } else {
    NewCommandMacro::addNewCommand(newcmd.substr(1), args[2], argc, args[4]);
  }

  return nullptr;
}

macro(renewcommand) {
  string newcmd(args[1]);
  int argc = 0;
  if (!tp.isValidName(newcmd))
    throw ex_parse("Invalid name for the command: " + newcmd);

  if (!args[3].empty()) valueof(args[3], argc);

  if (args[4].empty()) {
    NewCommandMacro::addRenewCommand(newcmd.substr(1), args[2], argc);
  } else {
    NewCommandMacro::addRenewCommand(newcmd.substr(1), args[2], argc, args[4]);
  }

  return nullptr;
}

macro(raisebox) {
  auto r = Units::getDimen(args[1]);
  auto h = Units::getDimen(args[3]);
  auto d = Units::getDimen(args[4]);
  return sptrOf<RaiseAtom>(Formula(tp, args[2], false, tp.isMathMode())._root, -r, h, d);
}

macro(romannumeral) {
  int numbers[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
  string letters[] = {"M", "CM", "D", "CD", "C", "XC", "", "XL", "X", "IX", "V", "IV", "I"};
  string roman;

  int num;
  valueof(trim(args[1]), num);
  for (int i = 0; i < 13; i++) {
    while (num >= numbers[i]) {
      roman += letters[i];
      num -= numbers[i];
    }
  }

  if (args[0][0] == 'r') {
    tolower(roman);
  }

  return Formula(roman, false)._root;
}

macro(setmathfont) {
  auto mathStyle = MathStyle::TeX;
  const auto& options = parseOption(args[2]);
  const auto it = options.find("math-style");
  if (it != options.end()) {
    const auto& value = it->second;
    if (value == "TeX") {
      mathStyle = MathStyle::TeX;
    } else if (value == "ISO") {
      mathStyle = MathStyle::ISO;
    } else if (value == "French") {
      mathStyle = MathStyle::French;
    } else if (value == "upright") {
      mathStyle = MathStyle::upright;
    }
  }
  return sptrOf<MathFontAtom>(mathStyle, args[1]);
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

}  // namespace tex
