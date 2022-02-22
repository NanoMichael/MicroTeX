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

void TinyTeX::init(Init init) {
  auto initialization = [&]() {
    if (_config->isInited) return;
    _config->isInited = true;
    NewCommandMacro::_init_();
  };

  const FontSrc** mathFontSrc = std::get_if<const FontSrc*>(&init);
  if (mathFontSrc != nullptr) {
    FontContext::addMathFont(**mathFontSrc);
    _config->defaultMathFontName = (*mathFontSrc)->name;
    return initialization();
  }

  {
    auto mathfont = FontsenseLookup();
    {
      const std::string* mathFontName = std::get_if<const std::string>(&init);
      if (mathFontName != nullptr) {
        _config->defaultMathFontName = *mathFontName;
        FontContext().selectMathFont(_config->defaultMathFontName);
        return initialization();
      }
    }

    if (mathfont) {
      _config->defaultMathFontName = mathfont.value();
    } else {
      throw ex_invalid_param("no math font found by fontsense");
    }
  }

  return initialization();
}

bool TinyTeX::isInited() {
  return _config->isInited;
}

void TinyTeX::release() {
  MacroInfo::_free_();
  NewCommandMacro::_free_();
}

void TinyTeX::addMainFont(
  const std::string& name,
  const FontSrcList& srcs
) {
  FontContext::addMainFont(name, srcs);
  if (_config->defaultMainFontName.empty()) {
    _config->defaultMainFontName = name;
  }
}

void TinyTeX::addMathFont(const FontSrc& src) {
  FontContext::addMathFont(src);
}

void TinyTeX::setDefaultMathFont(const std::string& name) {
  _config->defaultMathFontName = name;
}

void TinyTeX::setDefaultMainFont(const std::string& name) {
  _config->defaultMainFontName = name;
}

void TinyTeX::overrideTexStyle(bool enable, TexStyle style) {
  RenderBuilder::overrideTexStyle(enable, style);
}

void TinyTeX::setRenderGlyphUsePath(bool use) {
  _config->renderGlyphUsePath = use;
}

bool TinyTeX::isRenderGlyphUsePath() {
  return _config->renderGlyphUsePath;
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
