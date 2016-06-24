#if defined (__clang__)
#include "formula.h"
#include "fonts.h"
#include "core.h"
#include "macro.h"
#elif defined (__GNUC__)
#include "core/formula.h"
#include "fonts/fonts.h"
#include "core/core.h"
#include "core/macro.h"
#endif // defined

#include "latex.h"

#include "memcheck.h"

using namespace std;
using namespace tex;
using namespace tex::port;
using namespace tex::core;
using namespace tex::fonts;

string RES_BASE = "res";

TeXFormula* LaTeX::_formula = nullptr;
TeXRenderBuilder* LaTeX::_builder = nullptr;

void LaTeX::init() {
	if (_formula != nullptr)
		return;
	NewCommandMacro::_init_();
	DefaultTeXFont::_init_();
	SymbolAtom::_init_();
	Glue::_init_();
	TeXFormula::_init_();
	TextRenderingBox::_init_();
	DUMP_MEM_STATUS();

	_formula = new TeXFormula();
	_builder = new TeXRenderBuilder();
}

void LaTeX::release() {
	Glue::_free_();
	DefaultTeXFont::_free_();
	TeXFormula::_free_();
	MacroInfo::_free_();
	NewCommandMacro::_free_();
	TextRenderingBox::_free_();

	if (_formula != nullptr)
		delete _formula;
	if (_builder != nullptr)
		delete _builder;
}

TeXRender* LaTeX::parse(const wstring& latex, int width, float textSize, float interlineSpacing, color fg) {
	bool lined = true;
	if (startswith(latex, L"$$") || startswith(latex, L"\\["))
		lined = false;
	int align = lined ? ALIGN_LEFT : ALIGN_CENTER;
	_formula->setLaTeX(latex);
	TeXRender* render = _builder->setStyle(STYLE_DISPLAY)
	                    .setSize(textSize)
	                    .setWidth(UNIT_PIXEL, width, align)
	                    .setIsMaxWidth(lined)
	                    .setInterlineSpacing(UNIT_PIXEL, interlineSpacing)
	                    .setForeground(fg)
	                    .build(*_formula);
	return render;
}
