#include "unimath/uni_font.h"
#include "unimath/uni_symbol.h"
#include "utils/utils.h"
#include "utils/exceptions.h"
#include "utils/log.h"
#include <utility>

using namespace std;

namespace tex {

OtfFont::OtfFont(i32 id, sptr<const Otf> spec, std::string fontFile) noexcept
  : id(id), fontFile(std::move(fontFile)), otfSpec(std::move(spec)) {}

/*********************************************************************************/

FontStyle FontFamily::fontStyleOf(const std::string& name) {
  // TODO: more composed styles
  static const map<string, FontStyle> nameStyle{
    {"",     FontStyle::rm},
    {"rm",   FontStyle::rm},
    {"bf",   FontStyle::bf},
    {"it",   FontStyle::it},
    {"sf",   FontStyle::sf},
    {"tt",   FontStyle::tt},
    {"cal",  FontStyle::cal},
    {"frak", FontStyle::frak},
    {"bfit", FontStyle::bfit},
  };
  const auto it = nameStyle.find(name);
  if (it == nameStyle.end()) return FontStyle::none;
  return it->second;
}

void FontFamily::add(const std::string& styleName, const sptr<const OtfFont>& font) {
  const auto style = fontStyleOf(styleName);
#ifdef HAVE_LOG
  if (_styles.find(style) != _styles.end()) {
    loge("the style '%s' has a font already, but you can replace it anyway\n", styleName.c_str());
  }
#endif
  _styles[style] = font;
}

sptr<const OtfFont> FontFamily::get(FontStyle style) const {
  const auto it = _styles.find(style);
  if (it == _styles.end()) {
    const auto rm = _styles.find(FontStyle::rm);
    return rm == _styles.end() ? nullptr : rm->second;
  }
  return it->second;
}

/*********************************************************************************/

FontSrc::FontSrc(std::string name, std::string fontFile)
  : name(std::move(name)),
    fontFile(std::move(fontFile)) {}

FontSrcFile::FontSrcFile(std::string name, std::string clmFile, std::string fontFile)
  : FontSrc(std::move(name), std::move(fontFile)),
    clmFile(std::move(clmFile)) {}

sptr<Otf> FontSrcFile::loadOtf() const {
  return sptr<Otf>(Otf::fromFile(clmFile.c_str()));
}

FontSrcData::FontSrcData(std::string name, size_t len, const u8* data, std::string fontFile)
  : FontSrc(std::move(name), std::move(fontFile)),
    len(len),
    data(data) {}

sptr<Otf> FontSrcData::loadOtf() const {
  return sptr<Otf>(Otf::fromData(len, data));
}

/*********************************************************************************/

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

sptr<FontFamily> FontContext::getOrCreateFontFamily(const std::string& version) {
  sptr<FontFamily> f;
  auto it = _mainFonts.find(version);
  if (it == _mainFonts.end()) {
    f = sptrOf<FontFamily>();
    _mainFonts[version] = f;
  } else {
    f = it->second;
  }
  return f;
}

void FontContext::addMainFont(const std::string& versionName, const FontSrcList& srcs) {
  auto f = getOrCreateFontFamily(versionName);
  for (const auto& src : srcs) {
    auto spec = src->loadOtf();
    auto otf = sptrOf<OtfFont>(_lastId++, spec, src->fontFile);
    _fonts.push_back(otf);
    f->add(src->name, otf);
  }
}

void FontContext::addMathFont(const FontSrc& src) {
  const auto& name = src.name;
  if (_mathFonts.find(name) != _mathFonts.end()) {
    // already loaded
    return;
  }
  auto spec = src.loadOtf();
  auto otf = sptrOf<OtfFont>(_lastId++, spec, src.fontFile);
  _fonts.push_back(otf);
  _mathFonts[name] = otf;
}

bool FontContext::hasMathFont() {
  return !_mathFonts.empty();
}

sptr<const OtfFont> FontContext::getFont(i32 id) {
  if (id >= _fonts.size() || id < 0) return nullptr;
  return _fonts[id];
}

void FontContext::selectMathFont(const string& name) {
  const auto it = _mathFonts.find(name);
  if (it == _mathFonts.end()) {
    throw ex_invalid_param("Math font '" + name + "' does not exists!");
  }
  _mathFont = it->second;
}

void FontContext::selectMainFont(const string& name) {
  const auto it = _mainFonts.find(name);
  if (it == _mainFonts.end()) {
    throw ex_invalid_param("Main font '" + name + "' does not exists!");
  }
  _mainFont = it->second;
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

}
