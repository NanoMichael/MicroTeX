#include "macro/macro_impl.h"

#include <memory>

#include "graphic/graphic.h"
#include "env/env.h"
#include "core/debug_config.h"
#include "atom/atom_zstack.h"

using namespace tex;
using namespace std;

namespace tex {

macro(kern) {
  auto[unit, value] = tp.getLength();
  return sptrOf<SpaceAtom>(unit, value, 0.f, 0.f);
}

macro(hvspace) {
  auto[unit, value] = Units::getLength(args[1]);
  return (
    args[0][0] == L'h'
    ? sptrOf<SpaceAtom>(unit, value, 0.f, 0.f)
    : sptrOf<SpaceAtom>(unit, 0.f, value, 0.f)
  );
}

macro(rule) {
  auto[wu, w] = Units::getLength(args[1]);
  auto[hu, h] = Units::getLength(args[2]);
  auto[ru, r] = Units::getLength(args[3]);

  return sptrOf<RuleAtom>(wu, w, hu, h, ru, -r);
}

macro(newcommand) {
  wstring newcmd(args[1]);
  int argc = 0;
  if (!tp.isValidName(newcmd))
    throw ex_parse("Invalid name for the command '" + wide2utf8(newcmd));

  if (!args[3].empty()) valueof(args[3], argc);

  if (args[4].empty()) {
    NewCommandMacro::addNewCommand(newcmd.substr(1), args[2], argc);
  } else {
    NewCommandMacro::addNewCommand(newcmd.substr(1), args[2], argc, args[4]);
  }

  return nullptr;
}

macro(renewcommand) {
  wstring newcmd(args[1]);
  int argc = 0;
  if (!tp.isValidName(newcmd))
    throw ex_parse("Invalid name for the command: " + wide2utf8(newcmd));

  if (!args[3].empty()) valueof(args[3], argc);

  if (args[4].empty()) {
    NewCommandMacro::addRenewCommand(newcmd.substr(1), args[2], argc);
  } else {
    NewCommandMacro::addRenewCommand(newcmd.substr(1), args[2], argc, args[4]);
  }

  return nullptr;
}

macro(raisebox) {
  auto[ru, r] = Units::getLength(args[1]);
  auto[hu, h] = Units::getLength(args[3]);
  auto[du, d] = Units::getLength(args[4]);
  return sptrOf<RaiseAtom>(Formula(tp, args[2])._root, ru, r, hu, h, du, d);
}

macro(romannumeral) {
  int numbers[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
  string letters[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
  string roman;

  int num;
  string x = wide2utf8(args[1]);
  valueof(trim(x), num);
  for (int i = 0; i < 13; i++) {
    while (num >= numbers[i]) {
      roman += letters[i];
      num -= numbers[i];
    }
  }

  if (args[0][0] == 'r') {
    tolower(roman);
  }

  const wstring str = utf82wide(roman);
  return Formula(str, false)._root;
}

macro(setmathfont) {
  auto mathStyle = MathStyle::TeX;
  const auto& options = parseOption(wide2utf8(args[2]));
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
  return sptrOf<MathFontAtom>(mathStyle, wide2utf8(args[1]));
}

macro(debug) {
  auto& config = DebugConfig::INSTANCE;
  const auto& options = parseOption(wide2utf8(args[1]));
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
  if (args[1] == L"c") {
    halign = Alignment::center;
  } else if (args[1] == L"r") {
    halign = Alignment::right;
  }
  const auto&[hu, hv] = Units::getLength(args[2]);
  const ZStackArgs hargs{halign, hu, hv};

  auto valign = Alignment::top;
  if (args[3] == L"c") {
    valign = Alignment::center;
  } else if (args[3] == L"b") {
    valign = Alignment::bottom;
  } else if (args[3] == L"B") {
    valign = Alignment::none;
  }
  const auto&[vu, vv] = Units::getLength(args[4]);
  const ZStackArgs& vargs{valign, vu, vv};

  const auto atom = Formula(tp, args[5], false)._root;
  const auto anchor = Formula(tp, args[6], false)._root;

  return sptrOf<ZStackAtom>(hargs, vargs, atom, anchor);
}

}  // namespace tex
