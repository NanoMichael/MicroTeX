#include "unimath/uni_font.h"

#include <utility>

#include "unimath/uni_symbol.h"
#include "utils/exceptions.h"
#include "utils/log.h"
#include "utils/utils.h"

using namespace std;

namespace microtex {

OtfFont::OtfFont(i32 id, sptr<const Otf> spec, std::string fontFile) noexcept
    : id(id),
      fontFile(fontFile.empty() ? spec->family() : std::move(fontFile)),
      otfSpec(std::move(spec)) {}

/**************************************************************************************************/

const vector<FontStyle>& FontFamily::supportedStyles() {
  static const vector<FontStyle> styles{
    FontStyle::rm,
    FontStyle::bf,
    FontStyle::it,
    FontStyle::sf,
    FontStyle::tt,
    FontStyle::cal,
    FontStyle::frak,
    FontStyle::bfit,
    FontStyle::sfbfit,
  };
  return styles;
}

FontStyle FontFamily::fontStyleOf(const std::string& name) {
  // TODO: more composed styles
  static const map<string, FontStyle> nameStyle{
    {"",     FontStyle::rm  },
    {"rm",   FontStyle::rm  },
    {"bf",   FontStyle::bf  },
    {"it",   FontStyle::it  },
    {"sf",   FontStyle::sf  },
    {"tt",   FontStyle::tt  },
    {"cal",  FontStyle::cal },
    {"frak", FontStyle::frak},
    {"bfit", FontStyle::bfit},
  };
  const auto it = nameStyle.find(name);
  if (it == nameStyle.end()) return FontStyle::none;
  return it->second;
}

void FontFamily::add(const sptr<const OtfFont>& font) {
  const auto style = static_cast<FontStyle>(font->otfSpec->style());
#ifdef HAVE_LOG
  if (_styles.find(style) != _styles.end()) {
    loge("the style '%hx' has a font already, but you can replace it anyway\n", style);
  }
#endif
  _styles[style] = font;
}

sptr<const OtfFont> FontFamily::get(FontStyle style) const {
  const auto it = _styles.find(style);
  if (it == _styles.end()) {
    FontStyle closest = findClosestStyle(style);
    const auto t = _styles.find(closest);
    if (t == _styles.end()) {
      const auto rm = _styles.find(FontStyle::rm);
      return rm == _styles.end() ? nullptr : rm->second;
    }
    return t->second;
  }
  return it->second;
}

/**************************************************************************************************/

namespace {

int _lastId = 0;
vector<sptr<const OtfFont>> _fonts;

map<string, sptr<FontFamily>> _mainFonts;
map<string, sptr<const OtfFont>> _mathFonts;

sptr<FontFamily> getOrCreateFontFamily(const std::string& family) {
  sptr<FontFamily> f;
  auto it = _mainFonts.find(family);
  if (it == _mainFonts.end()) {
    f = sptrOf<FontFamily>();
    _mainFonts[family] = f;
  } else {
    f = it->second;
  }
  return f;
}

}  // namespace

FontStyle FontContext::mathFontStyleOf(const std::string& name) {
  static const map<string, FontStyle> nameStyle{
    {"",           FontStyle::none  },
    {"mathnormal", FontStyle::none  },
    {"mathrm",     FontStyle::rm    },
    {"mathbf",     FontStyle::bf    },
    {"mathit",     FontStyle::it    },
    {"mathcal",    FontStyle::cal   },
    {"mathscr",    FontStyle::cal   },
    {"mathfrak",   FontStyle::frak  },
    {"mathbb",     FontStyle::bb    },
    {"mathsf",     FontStyle::sf    },
    {"mathtt",     FontStyle::tt    },
    {"mathbfit",   FontStyle::bfit  },
    {"mathbfcal",  FontStyle::bfcal },
    {"mathbffrak", FontStyle::bffrak},
    {"mathsfbf",   FontStyle::sfbf  },
    {"mathbfsf",   FontStyle::sfbf  },
    {"mathsfit",   FontStyle::sfit  },
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

FontMeta FontContext::addFont(const FontSrc& src) {
  auto spec = src.loadOtf();
  auto otf = sptrOf<OtfFont>(_lastId++, spec, src.fontFile);
  _fonts.push_back(otf);
  if (spec->isMathFont()) {
    _mathFonts[spec->name()] = otf;
  } else {
    auto f = getOrCreateFontFamily(spec->family());
    f->add(otf);
  }
  return {
    spec->family(),
    spec->name(),
    static_cast<const FontStyle>(spec->style()),
    spec->isMathFont(),
  };
}

bool FontContext::hasMathFont() {
  return !_mathFonts.empty();
}

FontMeta FontContext::mathFontMetaOf(const std::string& name) {
  auto it = _mathFonts.find(name);
  if (it == _mathFonts.end()) {
    return {};
  }
  const auto& spec = it->second->otf();
  return {spec.family(), spec.name(), static_cast<FontStyle>(spec.style()), spec.isMathFont()};
}

bool FontContext::isMathFontExists(const std::string& name) {
  return _mathFonts.find(name) != _mathFonts.end();
}

bool FontContext::isMainFontExists(const std::string& familyName) {
  return _mainFonts.find(familyName) != _mainFonts.end();
}

std::vector<std::string> FontContext::mathFontNames() {
  return keys(_mathFonts);
}

std::vector<std::string> FontContext::mainFontFamilies() {
  return keys(_mainFonts);
}

sptr<const OtfFont> FontContext::getFont(i32 id) {
  if (id >= _fonts.size() || id < 0) return nullptr;
  return _fonts[id];
}

bool FontContext::selectMathFont(const string& name) {
  const auto it = _mathFonts.find(name);
  if (it == _mathFonts.end()) {
#ifdef HAVE_LOG
    loge("Math font '%s' does not exists!", name.c_str());
#endif
    return false;
  }
  _mathFont = it->second;
  return true;
}

bool FontContext::selectMainFont(const string& familyName) {
  if (familyName.empty()) {
    _mainFont = nullptr;
    return false;
  }
  const auto it = _mainFonts.find(familyName);
  if (it == _mainFonts.end()) {
#ifdef HAVE_LOG
    loge("Main font '%s' does not exists!", familyName.c_str());
#endif
    return false;
  }
  _mainFont = it->second;
  return true;
}

u16 FontContext::mainSpace() {
  if (_mainFont == nullptr) return mathFont().otf().space();
  for (auto style : FontFamily::supportedStyles()) {
    auto f = _mainFont->get(style);
    if (f != nullptr) {
      return f->otf().space();
    }
  }
  return mathFont().otf().space();
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
    // fallback to math font, at least we have one
    if (font == nullptr) font = _mathFont;
    return {code, code, font->id, font->otf().glyphId(code)};
  }
}

}  // namespace microtex
