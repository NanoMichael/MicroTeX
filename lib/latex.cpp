#include "latex.h"

#include "core/formula.h"
#include "macro/macro.h"
#include "utils/string_utils.h"

using namespace std;
using namespace tex;

volatile bool LaTeX::_isInited = false;
std::string LaTeX::_defaultMathFontName;

void LaTeX::init(const FontSrc& mathFontSrc) {
  FontContext::addMathFont(mathFontSrc);
  _defaultMathFontName = mathFontSrc.name;
  if (_isInited) return;
  _isInited = true;
  NewCommandMacro::_init_();
}

bool LaTeX::isInited() {
  return _isInited;
}

void LaTeX::release() {
  MacroInfo::_free_();
  NewCommandMacro::_free_();
}

void LaTeX::addMainFont(
  const std::string& name,
  const std::vector<std::unique_ptr<FontSrc>>& srcs
) {
  FontContext::addMainFonts(name, srcs);
}

void LaTeX::addMathFont(const FontSrc& src) {
  FontContext::addMathFont(src);
}

void LaTeX::setDefaultMathFont(const std::string& name) {
  _defaultMathFontName = name;
}

void LaTeX::overrideTexStyle(bool enable, TexStyle style) {
  RenderBuilder::overrideTexStyle(enable, style);
}

#if GLYPH_RENDER_TYPE == 0

bool LaTeX::_renderGlyphUsePath = false;

void LaTeX::setRenderGlyphUsePath(bool use) {
  _renderGlyphUsePath = use;
}

bool LaTeX::isRenderGlyphUsePath() {
  return _renderGlyphUsePath;
}

#endif

Render* LaTeX::parse(
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
    .setMainFontName(mainFontName)
    .setWidth(UnitType::pixel, width, align)
    .setIsMaxWidth(isInline)
    .setLineSpace(UnitType::pixel, lineSpace)
    .setForeground(fg)
    .build(formula);
  return render;
}
