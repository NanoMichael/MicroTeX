#ifndef LATEX_LATEX_H
#define LATEX_LATEX_H

#include <string>

#include "unimath/uni_font.h"
#include "render.h"

namespace tex {

class Formula;

class LaTeX {
private:
  static volatile bool _isInited;
  static std::string _defaultMathFontName;
  static Formula* _formula;
  static TeXRenderBuilder* _builder;

public:
  /**
   * Initialize LaTeX context with given math font spec, at least we need
   * a math font to layout formulas.
   *
   * @param mathFontSpec the font spec to load math font
   */
  static void init(const FontSpec& mathFontSpec);

  /** Check if context is initialized */
  static bool isInited();

  /**
   * Add main font (collection) to context.
   *
   * @param name the name of the font (collection)
   * @param params font spec to load
   */
  static void addMainFont(const std::string& name, const std::vector<FontSpec>& params);

  /**
   * Add math font to context.
   *
   * @param params font spec to load
   */
  static void addMathFont(const FontSpec& params);

  /**
   * Parse TeX formatted string to TeXRender
   *
   * @param tex the TeX formatted string
   * @param width the width of the 2D graphics context
   * @param textSize the text size
   * @param lineSpace the line space
   * @param fg the foreground color
   * @param mathFontName the math font name
   * @param mainFontName the main font name
   */
  static TeXRender* parse(
    const std::string& tex,
    int width,
    float textSize,
    float lineSpace,
    color fg,
    const std::string& mathFontName = "",
    const std::string& mainFontName = ""
  );

  /** Release the LaTeX context */
  static void release();
};

}  // namespace tex

#endif  // LATEX_LATEX_H
