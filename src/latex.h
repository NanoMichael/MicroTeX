#ifndef LATEX_H_INCLUDED
#define LATEX_H_INCLUDED

#include "common.h"
#include "graphic/graphic.h"
#include "graphic/graphic_basic.h"
#include "render.h"

#include <string>
#include <queue>
#include <sstream>

namespace tex {

class Formula;

class LaTeX {
private:
  static Formula* _formula;
  static TeXRenderBuilder* _builder;

protected:
  static std::string queryResourceLocation(std::string& custom_path);

public:
  /**
   * Initialize TeX context with given root path of the TeX resources
   *
   * @param res_root_path root path of the resources, default is 'res'
   */
  static void init(std::string res_root_path = "res");

  /**
   * Get the root path of the "TeX resources"
   */
  static const std::string& getResRootPath();

  /**
   * If open debug
   */
  static void setDebug(bool debug);

  /**
   * Parse TeX formatted string to TeXRender
   *
   * @param tex the TeX formatted string
   * @param width the width of the 2D graphics context
   * @param textSize the text size
   * @param lineSpace the line space
   * @param fg the foreground color
   */
  static TeXRender* parse(const std::wstring& tex, int width, float textSize, float lineSpace, color fg);

  /**
   * Release the LaTeX context
   */
  static void release();
};

}  // namespace tex

#endif  // LATEX_H_INCLUDED
