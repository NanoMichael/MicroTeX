#include "unimath/uni_font.h"

#include <cstring>

using namespace tex;
using namespace std;

MathVersion::MathVersion(
  c32 digit, c32 latinSmall, c32 latinCapital, c32 greekSmall, c32 greekCapital  //
  ) : _codepoints{0, digit, latinSmall, latinCapital, greekSmall, greekCapital} {}

pair<MathType, c32> MathVersion::ofChar(c32 codepoint) {
  if (codepoint >= '0' && codepoint <= '9') return {MathType::digit, codepoint - '0'};
  if (codepoint >= 'a' && codepoint <= 'z') return {MathType::latinSmall, codepoint - 'a'};
  if (codepoint >= 'A' && codepoint <= 'Z') return {MathType::latinCapital, codepoint - 'A'};
  if (codepoint >= 0x03B1 && codepoint <= 0x03C9) {  // α - ω
    return {MathType::greekSmall, codepoint - 0x03B1};
  }
  if (codepoint >= 0x0391 && codepoint <= 0x03A9) {  // Α - Ω
    return {MathType::greekCapital, codepoint - 0x0391};
  }
  return {MathType::none, codepoint};
}

c32 MathVersion::map(const c32 codepoint) const {
  vector<int> x;
  x.begin();
  auto [type, offset] = ofChar(codepoint);
  return _codepoints[static_cast<u8>(type)] + offset;
}

OtfFont::OtfFont(const string& fontFile, const string& clmFile)
    : _fontFile(fontFile), _otf(sptr<const Otf>(Otf::fromFile(clmFile.c_str()))) {}

UniGlyph::UniGlyph(c32 unicode, const sptr<const OtfFont>& font)
    : _unicode(unicode), _font(font) {
  _glyph = font->otf().glyphOfUnicode(unicode);
}

inline sptr<const OtfFont>& TextFont::operator[](const string& styleName) {
  return _styles[styleName];
}

string FontContext::_emptyVersionName = "";
string FontContext::_defaultVersionName = "mathnormal";

#define version(name, digit, latinSmall, latinCapital, greekSmall, greekCapital)                 \
  {                                                                                              \
    ##name, sptrOf<const MathVersion>(digit, latinSmall, latinCapital, greekSmall, greekCapital) \
  }

map<string, sptr<const MathVersion>> FontContext::_mathStyles{
  version("TeX", '0', 0x1D44E, 0x1D434, 0x1D6FC, 0x0391),
  version("ISO", '0', 0x1D44E, 0x1D434, 0x1D6FC, 0x1D6E2),
  version("French", '0', 0x1D44E, 'A', 0x03B1, 0x0391),
  version("upright", '0', 'a', 'A', 0x03B1, 0x0391),
};

map<string, sptr<const MathVersion>> FontContext::_mathVersions{
  {_emptyVersionName, _mathStyles["TeX"]},
  {_defaultVersionName, _mathStyles["TeX"]},
  version("mathrm", '0', 'a', 'A', 0x03B1, 0x0391),
  version("mathbf", 0x1D7CE, 0x1D41A, 0x1D400, 0x1D6C2, 0x1D6A8),
  version("mathit", '0', 0x1D44E, 0x1D434, 0x1D6FC, 0x1D6E2),
  version("mathbfit", '0', 0x1D482, 0x1D468, 0x1D736, 0x1D71C),
  version("mathcal", '0', 0x1D4B6, 0x1D49C, 0x03B1, 0x0391),
  version("mathbfcal", '0', 0x1D4EA, 0x1D4D0, 0x03B1, 0x0391),
  version("mathfrak", '0', 0x1D51E, 0x1D504, 0x03B1, 0x0391),
  version("mathbb", 0x1D7D8, 0x1D552, 0x1D538, 0x03B1, 0x0391),
  version("mathbffrak", '0', 0x1D586, 0x1D552, 0x03B1, 0x0391),
  version("mathsf", 0x1D7E2, 0x1D5BA, 0x1D5A0, 0x03B1, 0x0391),
  version("mathsfbf", 0x1D7EC, 0x1D5EE, 0x1D5D4, 0x1D770, 0x1D756),
  version("mathsfit", '0', 0x1D622, 0x1D608, 0x03B1, 0x0391),
  version("mathsfbfit", '0', 0x1D656, 0x1D63C, 0x1D7AA, 0x1D790),
  version("mathtt", 0x1D7F6, 0x1D68A, 0x1D670, 0x03B1, 0x0391),
};

map<string, sptr<TextFont>> FontContext::_mainFonts;
map<string, sptr<const OtfFont>> FontContext::_mathFonts;

void FontContext::setMainFont(const string& versionName, const vector<FontSpec>& params) {
  TextFont* ptr = new TextFont();
  TextFont& f = *ptr;
  for (auto [style, font, clm] : params) {
    f[style] = sptrOf<const OtfFont>(font, clm);
  }
  _mainFonts[versionName] = sptr<TextFont>(ptr);
}

void FontContext::setMathFont(const FontSpec& params) {
  auto [version, font, clm] = params;
  _mathFonts[version] = sptrOf<const OtfFont>(font, clm);
}

void FontContext::setMathStyle(const string& styleName) {
  auto it = _mathStyles.find(styleName);
  if (it == _mathStyles.end()) return;
  _mathVersions[_defaultVersionName] = it->second;
}

void FontContext::selectMathFont(const string& versionName) {
  _mathFont = _mathFonts[versionName];
}

void FontContext::selectMainFont(const string& versionName) {
  _mainFont = _mainFonts[versionName];
}

UniGlyph FontContext::glyphOf(c32 codepoint, const string& versionName, bool isMathMode) const {
  if (isMathMode) {
    const auto ptr = _mathVersions[versionName];
    const MathVersion& version = ptr == nullptr ? *_mathVersions[""] : *ptr;
    const c32 unicode = version.map(codepoint);
    return UniGlyph(unicode, _mathFont);
  } else {
    sptr<const OtfFont> font = (*_mainFont)[versionName];
    if (font == nullptr) font = (*_mainFont)[""];
    if (font == nullptr) font = _mathFont;
    return UniGlyph(codepoint, font);
  }
}
