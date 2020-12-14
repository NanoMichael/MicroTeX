#include "fonts/fonts.h"

/**
 * General parameters used in the TeX algorithms, 
 * specific for the computer modern font family
 */
std::map<std::string, float> tex::DefaultTeXFont::_parameters = {
    {"num1", 0.676508},
    {"num2", 0.393732},
    {"num3", 0.443731},
    {"denom1", 0.685951},
    {"denom2", 0.344841},
    {"sup1", 0.412892},
    {"sup2", 0.362892},
    {"sup3", 0.288889},
    {"sub1", 0.15},
    {"sub2", 0.247217},
    {"supdrop", 0.386108},
    {"subdrop", 0.05},
    {"axisheight", 0.25},
    {"defaultrulethickness", 0.039999},
    {"bigopspacing1", 0.111112},
    {"bigopspacing2", 0.166667},
    {"bigopspacing3", 0.2},
    {"bigopspacing4", 0.6},
    {"bigopspacing5", 0.1},
};

#define __id(name) FontInfo::__id(#name)

void tex::DefaultTeXFont::__default_general_settings() {
  tex::DefaultTeXFont::_generalSettings = {
      {"mufontid", __id(cmsy10)},
      {"spacefontid", __id(cmr10)},
      {"textfactor", 1},
      {"scriptfactor", 0.7},
      {"scriptscriptfactor", 0.5},
  };
}

#define cf(c, f) new CharFont(c, __id(f))

void tex::DefaultTeXFont::__default_text_style_mapping() {
  tex::DefaultTeXFont::_textStyleMappings = {
      {"mathnormal", {cf(48, cmr10), cf(65, cmmi10), cf(97, cmmi10), cf(0, cmmi10)}},
      {"mathfrak", {cf(48, eufm10), cf(65, eufm10), cf(97, eufm10), nullptr}},
      {"mathcal", {nullptr, cf(65, cmsy10), nullptr, nullptr}},
      {"mathbb", {nullptr, cf(65, msbm10), nullptr, nullptr}},
      {"mathscr", {nullptr, cf(65, rsfs10), nullptr, nullptr}},
      {"mathds", {nullptr, cf(65, dsrom10), nullptr, nullptr}},
      {"oldstylenums", {cf(48, cmmi10), nullptr, nullptr, nullptr}},
  };
  tex::DefaultTeXFont::_defaultTextStyleMappings = new std::string[4];
  for (int i = 0; i < 4; i++)
    tex::DefaultTeXFont::_defaultTextStyleMappings[i] = "mathnormal";
}
