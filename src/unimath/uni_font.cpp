#include <utility>

#include "unimath/uni_font.h"
#include "unimath/uni_symbol.h"
#include "utils/utils.h"

using namespace tex;
using namespace std;

OtfFont::OtfFont(i32 id, string fontFile, const string& clmFile)
  : id(id), fontFile(std::move(fontFile)), otfSpec(sptr<const Otf>(Otf::fromFile(clmFile.c_str()))) {}

FontStyle FontFamily::fontStyleOf(const std::string& name) {
  // TODO: add composed styles
  static const map<string, FontStyle> nameStyle{
    {"",   FontStyle::none},
    {"rm", FontStyle::rm},
    {"bf", FontStyle::bf},
    {"it", FontStyle::it},
    {"sf", FontStyle::sf},
    {"tt", FontStyle::tt},
  };
  const auto it = nameStyle.find(name);
  if (it == nameStyle.end()) return FontStyle::none;
  return it->second;
}

void FontFamily::add(const std::string& styleName, const sptr<const OtfFont>& font) {
  _styles[fontStyleOf(styleName)] = font;
}

sptr<const OtfFont> FontFamily::get(FontStyle style) const {
  const auto it = _styles.find(style);
  return it == _styles.end() ? nullptr : it->second;
}

int FontContext::_lastId = 0;
vector<sptr<const OtfFont>> FontContext::_fonts;

map<string, sptr<FontFamily>> FontContext::_mainFonts;
map<string, sptr<const OtfFont>> FontContext::_mathFonts;

FontStyle FontContext::mathFontStyleOf(const std::string& name) {
  static const map<string, FontStyle> nameStyle{
    {"",           FontStyle::none},
    {"mathnormal", FontStyle::none},
    {"mathrm",     FontStyle::rm},
    {"mathbf",     FontStyle::bf},
    {"mathit",     FontStyle::it},
    {"mathcal",    FontStyle::cal},
    {"mathscr",    FontStyle::cal},
    {"mathfrak",   FontStyle::frak},
    {"mathbb",     FontStyle::bb},
    {"mathsf",     FontStyle::sf},
    {"mathtt",     FontStyle::tt},
    {"mathbfit",   FontStyle::bfit},
    {"mathbfcal",  FontStyle::bfcal},
    {"mathbffrak", FontStyle::bffrak},
    {"mathsfbf",   FontStyle::sfbf},
    {"mathbfsf",   FontStyle::sfbf},
    {"mathsfit",   FontStyle::sfit},
    {"mathsfbfit", FontStyle::sfbfit},
    {"mathbfsfit", FontStyle::sfbfit},
  };
  const auto it = nameStyle.find(name);
  if (it != nameStyle.end()) return it->second;
  return FontStyle::none;
}

FontStyle FontContext::mainFontStyleOf(const std::string& name) {
  return FontFamily::fontStyleOf(name);
}

void FontContext::addMainFont(const string& versionName, const vector<FontSpec>& params) {
  auto f = new FontFamily();
  for (const auto&[style, font, clm] : params) {
    auto otf = sptrOf<const OtfFont>(_lastId++, font, clm);
    _fonts.push_back(otf);
    f->add(style, otf);
  }
  _mainFonts[versionName] = sptr<FontFamily>(f);
}

void FontContext::addMathFont(const FontSpec& params) {
  const auto&[version, font, clm] = params;
  auto otf = sptrOf<OtfFont>(_lastId++, font, clm);
  _fonts.push_back(otf);
  _mathFonts[version] = otf;
}

bool FontContext::hasMathFont() {
  return !_mathFonts.empty();
}

sptr<const OtfFont> FontContext::getFont(i32 id) {
  if (id >= _fonts.size() || id < 0) return nullptr;
  return _fonts[id];
}

void FontContext::selectMathFont(const string& versionName) {
  const auto it = _mathFonts.find(versionName);
  _mathFont = it == _mathFonts.end() ? nullptr : it->second;
}

void FontContext::selectMainFont(const string& versionName) {
  const auto it = _mainFonts.find(versionName);
  _mainFont = it == _mainFonts.end() ? nullptr : it->second;
}

Char FontContext::getChar(const Symbol& symbol, FontStyle style) const {
  // TODO math mode?
  const auto code = symbol.unicode;
  return getChar(code, style, true);
}

Char FontContext::getChar(c32 code, const string& styleName, bool isMathMode) const {
  const auto style = isMathMode ? mathFontStyleOf(styleName) : mainFontStyleOf(styleName);
  return getChar(code, style, isMathMode);
}

Char FontContext::getChar(c32 code, FontStyle style, bool isMathMode) const {
  if (isMathMode) {
    const c32 unicode = MathVersion::map(style, code);
    return {code, unicode, _mathFont->id, _mathFont->otf().glyphId(unicode)};
  } else {
    sptr<const OtfFont> font = _mainFont == nullptr ? nullptr : _mainFont->get(style);
    if (font == nullptr && _mainFont != nullptr) font = _mainFont->get(FontStyle::none);
    // fallback to math font, at least we have a math font
    if (font == nullptr) font = _mathFont;
    return {code, code, font->id, font->otf().glyphId(code)};
  }
}
