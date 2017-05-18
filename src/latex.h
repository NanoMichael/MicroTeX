#ifndef LATEX_H_INCLUDED
#define LATEX_H_INCLUDED

#include "port/port.h"
#include "port/port_basic.h"
#include "render.h"
#include "common.h"

#include <string>

using namespace std;
using namespace tex;

namespace tex {

class TeXFormula;

class LaTeX {
private:
	static TeXFormula* _formula;
	static TeXRenderBuilder* _builder;
public:
	static void init();

	static TeXRender* parse(const wstring& latex, int width, float textSize, float interlineSpacing, color fg);

	static void release();
};

}

#endif // LATEX_H_INCLUDED
