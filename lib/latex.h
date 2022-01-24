#ifndef LATEX_LATEX_H
#define LATEX_LATEX_H

#include <string>

#include "config.h"
#include "unimath/uni_font.h"
#include "render.h"

namespace tex {

class CLATEXMATH_EXPORT LaTeX {
private:
  static volatile bool _isInited;
  static std::string _defaultMainFontName;
  static std::string _defaultMathFontName;

#if GLYPH_RENDER_TYPE == 0
  static bool _renderGlyphUsePath;
#endif

public:
  /**
   * Initialize LaTeX context with given math font spec, at least we need
   * a math font to layout formulas.
   *
   * @param mathFontSrc the font source to load math font
   */
  static void init(const FontSrc& mathFontSrc);

  /** Check if context is initialized */
  static bool isInited();

  /**
   * Add main font (collection) to context.
   *
   * @param name the name of the font (collection)
   * @param srcs font spec to load
   */
  static void addMainFont(
    const std::string& name,
    const FontSrcList& srcs
  );

  /**
   * Add math font to context.
   *
   * @param src font source to load
   */
  static void addMathFont(const FontSrc& src);

  /**
   * Set the default math font to show formulas while math font
   * was not given.
   *
   * @param name the math font name, must exists or an `ex_invalid_param`
   * will be thrown when parsing.
   */
  static void setDefaultMathFont(const std::string& name);

  /**
   * Set the default main font to show formulas while main font
   * was not given. However, if no main font was loaded, the engine
   * will use the math font to render the glyphs wrapped by command
   * `text`.
   *
   * @param name the main font name, this function will takes no
   * effect if it is empty, or if the font was not loaded, an
   * `ex_invalid_param` will be thrown when parsing.
   */
  static void setDefaultMainFont(const std::string& name);

  /**
   * Override the style to display formulas. If #enable is true, the '$', '$$',
   * '\(' and '\[' will be ignored, and force to use the given overridden style.
   *
   * @param enable whether enable the overridden style, false to cancel
   * @param style the target style to override
   */
  static void overrideTexStyle(bool enable, TexStyle style = TexStyle::text);

#if GLYPH_RENDER_TYPE == 0

  /** Set if use path to render glyphs */
  static void setRenderGlyphUsePath(bool use);

  /** Test if currently use path to render glyphs */
  static bool isRenderGlyphUsePath();

#endif

  /**
   * Parse LaTeX string to Render
   *
   * @param tex the TeX formatted string
   * @param width the width of the 2D graphics context
   * @param textSize the text size
   * @param lineSpace the line space
   * @param fg the foreground color
   * @param mathFontName the math font name
   * @param mainFontName the main font name
   */
  static Render* parse(
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
