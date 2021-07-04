#include <utility>

#include "unimath/uni_font.h"
#include "unimath/uni_symbol.h"
#include "utils/utils.h"

using namespace tex;
using namespace std;

OtfFont::OtfFont(i32 id, string fontFile, const string& clmFile)
  : _id(id), _fontFile(std::move(fontFile)), _otf(sptr<const Otf>(Otf::fromFile(clmFile.c_str()))) {}

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

#define style(name, digit, latinSmall, latinCapital, greekSmall, greekCapital)                   \
  {                                                                                              \
    #name, sptrOf<const MathVersion>(digit, latinSmall, latinCapital, greekSmall, greekCapital)  \
  }

map<string, sptr<const MathVersion>> FontContext::_mathStyles{
  style(TeX, '0', 0x1D44E, 0x1D434, 0x1D6FC, 0x0391),
  style(ISO, '0', 0x1D44E, 0x1D434, 0x1D6FC, 0x1D6E2),
  style(French, '0', 0x1D44E, 'A', 0x03B1, 0x0391),
  style(upright, '0', 'a', 'A', 0x03B1, 0x0391),
};

#define version(style, digit, latinSmall, latinCapital, greekSmall, greekCapital)                \
  {                                                                                              \
    style, sptrOf<const MathVersion>(digit, latinSmall, latinCapital, greekSmall, greekCapital)  \
  }

map<FontStyle, sptr<const MathVersion>> FontContext::_mathVersions{
  {FontStyle::none, _mathStyles["TeX"]},
  version(FontStyle::rm, '0', 'a', 'A', 0x03B1, 0x0391),
  version(FontStyle::bf, 0x1D7CE, 0x1D41A, 0x1D400, 0x1D6C2, 0x1D6A8),
  version(FontStyle::it, '0', 0x1D44E, 0x1D434, 0x1D6FC, 0x1D6E2),
  version(FontStyle::cal, '0', 0x1D4B6, 0x1D49C, 0x03B1, 0x0391),
  version(FontStyle::frak, '0', 0x1D51E, 0x1D504, 0x03B1, 0x0391),
  version(FontStyle::bb, 0x1D7D8, 0x1D552, 0x1D538, 0x03B1, 0x0391),
  version(FontStyle::sf, 0x1D7E2, 0x1D5BA, 0x1D5A0, 0x03B1, 0x0391),
  version(FontStyle::tt, 0x1D7F6, 0x1D68A, 0x1D670, 0x03B1, 0x0391),
  // composed styles
  version(FontStyle::bfit, '0', 0x1D482, 0x1D468, 0x1D736, 0x1D71C),
  version(FontStyle::bfcal, '0', 0x1D4EA, 0x1D4D0, 0x03B1, 0x0391),
  version(FontStyle::bffrak, '0', 0x1D586, 0x1D552, 0x03B1, 0x0391),
  version(FontStyle::sfbf, 0x1D7EC, 0x1D5EE, 0x1D5D4, 0x1D770, 0x1D756),
  version(FontStyle::sfit, '0', 0x1D622, 0x1D608, 0x03B1, 0x0391),
  version(FontStyle::sfbfit, '0', 0x1D656, 0x1D63C, 0x1D7AA, 0x1D790),
};

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

FontStyle FontContext::findClosestStyle(const FontStyle src) {
  static const FontStyle composedStyles[]{
    FontStyle::bfit, FontStyle::bfcal, FontStyle::bffrak,
    FontStyle::sfbf, FontStyle::sfit, FontStyle::sfbfit,
  };
  u32 similarity = 0;
  FontStyle target = FontStyle::none;
  for (FontStyle style: composedStyles) {
    const auto n = countSetBits(static_cast<u16>(src) & static_cast<u16>(style));
    if (n > similarity) {
      target = style;
      similarity = n;
    }
  }
  return target;
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

void FontContext::setMathStyle(const string& styleName) {
  auto it = _mathStyles.find(styleName);
  if (it == _mathStyles.end()) return;
  _mathVersions[FontStyle::none] = it->second;
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
  // TODO custom symbol
  const auto code = symbol.unicode;
  return getChar(code, style, true);
}

Char FontContext::getChar(c32 code, const string& styleName, bool isMathMode) const {
  const auto style = isMathMode ? mathFontStyleOf(styleName) : mainFontStyleOf(styleName);
  return getChar(code, style, isMathMode);
}

Char FontContext::getChar(c32 code, FontStyle style, bool isMathMode) const {
  if (isMathMode) {
    const auto it = _mathVersions.find(style);
    const MathVersion& version = (
      it == _mathVersions.end()
      ? *_mathVersions[findClosestStyle(style)]
      : *it->second
    );
    const c32 unicode = version.map(code);
    return {code, unicode, _mathFont->_id, _mathFont->otf().glyphId(unicode)};
  } else {
    sptr<const OtfFont> font = _mainFont == nullptr ? nullptr : _mainFont->get(style);
    if (font == nullptr && _mainFont != nullptr) font = _mainFont->get(FontStyle::none);
    // fallback to math font, at least we have a math font
    if (font == nullptr) font = _mathFont;
    return {code, code, font->_id, font->otf().glyphId(code)};
  }
}
