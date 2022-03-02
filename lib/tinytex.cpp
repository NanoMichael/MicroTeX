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
  std::string defaultMainFontName;
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

  string operator()(const FontSrc* src) {
    auto name = FontContext::addMathFont(*src);
    if (name.empty()) {
      throw ex_invalid_param("Given font is not a math font!");
    }
    return name;
  }

  string operator()(const string& name) {
    fontsenseLookup();
    if (!FontContext::isMathFontExists(name)) {
      throw ex_invalid_param("Math font '" + name + "' does not exists!");
    }
    return name;
  }

  string operator()(const InitFontSenseAuto& sense) {
    auto mathFont = fontsenseLookup();
    if (!mathFont.has_value()) {
      throw ex_invalid_param("No math font found by font-sense.");
    }
    return mathFont.value();
  }
};

string TinyTeX::init(const Init& init) {
  if (_config->isInited) return "";
  auto name = std::visit(InitVisitor(), init);
  _config->defaultMathFontName = name;
  _config->isInited = true;
  NewCommandMacro::_init_();
  return name;
}

#endif // HAVE_AUTO_FONT_FIND

string TinyTeX::init(const FontSrc& mathFontSrc) {
  if (_config->isInited) return "";
  const auto& name = FontContext::addMathFont(mathFontSrc);
  if (name.empty()) {
    throw ex_invalid_param("Given font is not a math font!");
  }
  _config->defaultMathFontName = name;
  _config->isInited = true;
  NewCommandMacro::_init_();
  return name;
}

bool TinyTeX::isInited() {
  return _config->isInited;
}

void TinyTeX::release() {
  MacroInfo::_free_();
  NewCommandMacro::_free_();
}

void TinyTeX::addMainFont(
  const std::string& familyName,
  const FontSrcList& srcs
) {
  FontContext::addMainFont(familyName, srcs);
  if (_config->defaultMainFontName.empty()) {
    _config->defaultMainFontName = familyName;
  }
}

string TinyTeX::addMathFont(const FontSrc& src) {
  return FontContext::addMathFont(src);
}

bool TinyTeX::setDefaultMathFont(const std::string& name) {
  if (!FontContext::isMathFontExists(name)) return false;
  _config->defaultMathFontName = name;
  return true;
}

bool TinyTeX::setDefaultMainFont(const std::string& name) {
  if (name.empty() || FontContext::isMainFontExists(name)) {
    _config->defaultMainFontName = name;
    return true;
  }
  return false;
}

std::vector<std::string> TinyTeX::mathFontNames() {
  return FontContext::mathFonts();
}

std::vector<std::string> TinyTeX::mainFontNames() {
  return FontContext::mainFonts();
}

void TinyTeX::overrideTexStyle(bool enable, TexStyle style) {
  RenderBuilder::overrideTexStyle(enable, style);
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
  const string& mathFontName, const string& mainFontName
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
      mainFontName.empty()
      ? _config->defaultMainFontName
      : mainFontName
    )
    .setWidth({width, UnitType::pixel}, align)
    .setIsMaxWidth(isInline)
    .setLineSpace({lineSpace, UnitType::pixel})
    .setForeground(fg)
    .build(formula);
  return render;
}
