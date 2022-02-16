#include "tinytex.h"

#include "core/formula.h"
#include "macro/macro.h"
#include "utils/string_utils.h"

using namespace std;
using namespace tinytex;

volatile bool TinyTeX::_isInited = false;
std::string TinyTeX::_defaultMathFontName;
std::string TinyTeX::_defaultMainFontName;

void TinyTeX::init(const FontSrc& mathFontSrc) {
  FontContext::addMathFont(mathFontSrc);
  _defaultMathFontName = mathFontSrc.name;
  if (_isInited) return;
  _isInited = true;
  NewCommandMacro::_init_();
}

bool TinyTeX::isInited() {
  return _isInited;
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
  if (_defaultMainFontName.empty()) {
    _defaultMainFontName = name;
  }
}

void TinyTeX::addMathFont(const FontSrc& src) {
  FontContext::addMathFont(src);
}

void TinyTeX::setDefaultMathFont(const std::string& name) {
  _defaultMathFontName = name;
}

void TinyTeX::setDefaultMainFont(const std::string& name) {
  _defaultMainFontName = name;
}

void TinyTeX::overrideTexStyle(bool enable, TexStyle style) {
  RenderBuilder::overrideTexStyle(enable, style);
}

#if GLYPH_RENDER_TYPE == 0

bool TinyTeX::_renderGlyphUsePath = false;

void TinyTeX::setRenderGlyphUsePath(bool use) {
  _renderGlyphUsePath = use;
}

bool TinyTeX::isRenderGlyphUsePath() {
  return _renderGlyphUsePath;
}

#endif

Render* TinyTeX::parse(
  const string& latex, int width, float textSize, float lineSpace, color fg,
  const string& mathFontName, const string& mainFontName
) {
  Formula formula(latex);
  const auto isInline = !startswith(latex, "$$") && !startswith(latex, "\\[");
  const auto align = isInline ? Alignment::left : Alignment::center;
  Render* render = RenderBuilder()
    .setStyle(isInline ? TexStyle::text : TexStyle::display)
    .setTextSize(textSize)
    .setMathFontName(mathFontName.empty() ? _defaultMathFontName : mathFontName)
    .setMainFontName(mainFontName.empty() ? _defaultMainFontName : mainFontName)
    .setWidth(UnitType::pixel, width, align)
    .setIsMaxWidth(isInline)
    .setLineSpace(UnitType::pixel, lineSpace)
    .setForeground(fg)
    .build(formula);
  return render;
}
