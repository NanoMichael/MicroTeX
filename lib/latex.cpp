#include "latex.h"

#include "core/formula.h"
#include "macro/macro.h"
#include "utils/string_utils.h"

using namespace std;
using namespace tex;

volatile bool LaTeX::_isInited = false;
std::string LaTeX::_defaultMathFontName;

void LaTeX::init(const FontSpec& mathFontSpec) {
  FontContext::addMathFont(mathFontSpec);
  _defaultMathFontName = mathFontSpec.name;
  if (_isInited) return;
  _isInited = true;
  NewCommandMacro::_init_();
}

void LaTeX::init(const std::string& name, size_t len, const u8* data) {
  FontContext::addMathFont(name, len, data);
  _defaultMathFontName = name;
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

void LaTeX::addMainFont(const std::string& name, const std::vector<FontSpec>& params) {
  FontContext::addMainFonts(name, params);
}

void LaTeX::addMathFont(const FontSpec& params) {
  FontContext::addMathFont(params);
}

void LaTeX::addMathFont(const std::string& name, size_t len, const u8* data) {
  FontContext::addMathFont(name, len, data);
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
