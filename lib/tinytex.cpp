#include "tinytex.h"

#include "core/formula.h"
#include "otf/fontsense.h"
#include "macro/macro.h"
#include "utils/exceptions.h"
#include "utils/string_utils.h"
#include "render/builder.h"

using namespace std;
using namespace tinytex;

namespace tinytex {

struct Config {
  bool isInited;
  std::string defaultMainFontFamily;
  std::string defaultMathFontName;
  bool renderGlyphUsePath;
};

static Config TINYTEX_CONFIG{false, "", "", false};

} // namespace tinytex

Config* TinyTeX::_config = &tinytex::TINYTEX_CONFIG;

std::string TinyTeX::version() {
  return std::to_string(TINYTEX_VERSION_MAJOR) + "."
         + std::to_string(TINYTEX_VERSION_MINOR) + "."
         + std::to_string(TINYTEX_VERSION_PATCH);
}

#ifdef HAVE_AUTO_FONT_FIND

struct InitVisitor {

  FontMeta operator()(const FontSrc* src) {
    auto meta = FontContext::addFont(*src);
    if (!meta.isMathFont) {
      throw ex_invalid_param("'" + meta.name + "' is not a math font!");
    }
    return meta;
  }

  FontMeta operator()(const string& name) {
    fontsenseLookup();
    if (!FontContext::isMathFontExists(name)) {
      throw ex_invalid_param("Math font '" + name + "' does not exists!");
    }
    return FontContext::mathFontMetaOf(name);
  }

  FontMeta operator()(const InitFontSenseAuto& sense) {
    auto mathFont = fontsenseLookup();
    if (!mathFont.has_value()) {
      throw ex_invalid_param("No math font found by font-sense.");
    }
    return mathFont.value();
  }
};

FontMeta TinyTeX::init(const Init& init) {
  if (_config->isInited) return {};
  auto meta = std::visit(InitVisitor(), init);
  _config->defaultMathFontName = meta.name;
  _config->isInited = true;
  NewCommandMacro::_init_();
  return meta;
}

#endif // HAVE_AUTO_FONT_FIND

FontMeta TinyTeX::init(const FontSrc& mathFontSrc) {
  if (_config->isInited) return {};
  auto meta = FontContext::addFont(mathFontSrc);
  if (!meta.isValid()) {
    throw ex_invalid_param("'" + meta.name + "' is not a math font!");
  }
  _config->defaultMathFontName = meta.name;
  _config->isInited = true;
  NewCommandMacro::_init_();
  return meta;
}

bool TinyTeX::isInited() {
  return _config->isInited;
}

void TinyTeX::release() {
  MacroInfo::_free_();
  NewCommandMacro::_free_();
}

FontMeta TinyTeX::addFont(const FontSrc& src) {
  auto meta = FontContext::addFont(src);
  if (meta.isMathFont && _config->defaultMathFontName.empty()) {
    _config->defaultMathFontName = meta.name;
  }
  if (!meta.isMathFont && _config->defaultMainFontFamily.empty()) {
    _config->defaultMainFontFamily = meta.family;
  }
  return meta;
}

bool TinyTeX::setDefaultMathFont(const std::string& name) {
  if (!FontContext::isMathFontExists(name)) return false;
  _config->defaultMathFontName = name;
  return true;
}

bool TinyTeX::setDefaultMainFont(const std::string& family) {
  if (family.empty() || FontContext::isMainFontExists(family)) {
    _config->defaultMainFontFamily = family;
    return true;
  }
  return false;
}

std::vector<std::string> TinyTeX::mathFontNames() {
  return FontContext::mathFontNames();
}

std::vector<std::string> TinyTeX::mainFontFamilies() {
  return FontContext::mainFontFamilies();
}

void TinyTeX::overrideTexStyle(bool enable, TexStyle style) {
  RenderBuilder::overrideTexStyle(enable, style);
}

bool TinyTeX::hasGlyphPathRender() {
#ifdef HAVE_GLYPH_RENDER_PATH
  return true;
#else
  return false;
#endif
}

void TinyTeX::setRenderGlyphUsePath(bool use) {
#if GLYPH_RENDER_TYPE == 0
  _config->renderGlyphUsePath = use;
#endif
}

bool TinyTeX::isRenderGlyphUsePath() {
#if GLYPH_RENDER_TYPE == 0
  return _config->renderGlyphUsePath;
#elif GLYPH_RENDER_TYPE == 1
  return true;
#else
  return false;
#endif
}

Render* TinyTeX::parse(
  const string& latex, float width, float textSize, float lineSpace, color fg,
  const string& mathFontName, const string& mainFontFamily
) {
  Formula formula(latex);
  const auto isInline = !startswith(latex, "$$") && !startswith(latex, "\\[");
  const auto align = isInline ? Alignment::left : Alignment::center;
  Render* render = RenderBuilder()
    .setStyle(isInline ? TexStyle::text : TexStyle::display)
    .setTextSize(textSize)
    .setMathFontName(
      mathFontName.empty()
      ? _config->defaultMathFontName
      : mathFontName
    )
    .setMainFontName(
      mainFontFamily.empty()
      ? _config->defaultMainFontFamily
      : mainFontFamily
    )
    .setWidth({width, UnitType::pixel}, align)
    .setIsMaxWidth(isInline)
    .setLineSpace({lineSpace, UnitType::pixel})
    .setForeground(fg)
    .build(formula);
  return render;
}
