#include "latex.h"

#include "core/core.h"
#include "core/formula.h"
#include "core/macro.h"

using namespace std;
using namespace tex;

volatile bool LaTeX::_isInited = false;
std::string LaTeX::_defaultMathFontName;
Formula* LaTeX::_formula = nullptr;
TeXRenderBuilder* LaTeX::_builder = nullptr;

void LaTeX::init(const FontSpec& mathFontSpec) {
  FontContext::addMathFont(mathFontSpec);
  _defaultMathFontName = mathFontSpec.name;
  if (_isInited) return;
  _isInited = true;
  NewCommandMacro::_init_();
  TextRenderingBox::_init_();
}

bool LaTeX::isInited() {
  return _isInited;
}

void LaTeX::release() {
  MacroInfo::_free_();
  NewCommandMacro::_free_();
  TextRenderingBox::_free_();

  delete _formula;
  delete _builder;
}

void LaTeX::setDebug(bool debug) {
  Formula::setDebug(debug);
}

void LaTeX::addMainFont(const std::string& name, const std::vector<FontSpec>& params) {
  FontContext::addMainFont(name, params);
}

void LaTeX::addMathFont(const FontSpec& params) {
  FontContext::addMathFont(params);
}

TeXRender* LaTeX::parse(
  const wstring& latex, int width, float textSize, float lineSpace, color fg,
  const string& mathFontName, const string& mainFontName
) {
  if (_formula == nullptr) _formula = new Formula();
  if (_builder == nullptr) _builder = new TeXRenderBuilder();

  bool isInline = true;
  if (startswith(latex, L"$$") || startswith(latex, L"\\[")) {
    isInline = false;
  }
  Alignment align = isInline ? Alignment::left : Alignment::center;
  _formula->setLaTeX(latex);
  TeXRender* render =
    _builder->setStyle(isInline ? TexStyle::text : TexStyle::display)
      .setTextSize(textSize)
      .setMathFontName(mathFontName.empty() ? _defaultMathFontName : mathFontName)
      .setMainFontName(mainFontName)
      .setWidth(UnitType::pixel, width, align)
      .setIsMaxWidth(isInline)
      .setLineSpace(UnitType::pixel, lineSpace)
      .setForeground(fg)
      .build(*_formula);
  return render;
}
