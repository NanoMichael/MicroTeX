#ifndef LATEX_H_INCLUDED
#define LATEX_H_INCLUDED

#include "common.h"
#include "graphic/graphic.h"
#include "graphic/graphic_basic.h"
#include "render.h"

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
  /**
   * Initialize TeX context with given root path of the TeX resources
   *
   * @param res_root_path
   *      root path of the resources, default is 'res'
   */
  static void init(const string& res_root_path = "res");

  /**
   * Get the root path of the "TeX resources"
   */
  static const string& getResRootPath();

  /**
   * If open debug
   */
  static void setDebug(bool debug);

  /**
   * Parse TeX formatted string to TeXRender
   *
   * @param tex
   *      the TeX formatted string
   * @param width
   *      the width of the 2D graphics context
   * @param textSize
   *      the text size
   * @param lineSpace
   *      the line space
   * @param fg
   *      the foreground color
   */
  static TeXRender* parse(const wstring& tex, int width, float textSize, float lineSpace, color fg);

  /**
   * Release the LaTeX context
   */
  static void release();
};

}  // namespace tex

#endif  // LATEX_H_INCLUDED
