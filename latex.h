#ifndef LATEX_H_INCLUDED
#define LATEX_H_INCLUDED

#if defined (__clang__)
#include "port.h"
#include "port_basic.h"
#elif defined (__GNUC__)
#include "port/port.h"
#include "port/port_basic.h"
#endif // defined

#include "render.h"
#include "common.h"
#include <string>

using namespace std;
using namespace tex::port;

namespace tex {

namespace core {
class TeXFormula;
}

using namespace tex::core;

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
