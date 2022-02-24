#ifndef TINYTEX_TINYTEX_H
#define TINYTEX_TINYTEX_H

#include "config.h"
#include "version.h"
#include "render/render.h"
#include "unimath/font_src.h"

#include <string>
#include <variant>

namespace tinytex {

struct TINYTEX_EXPORT InitFontSenseAuto {
};

typedef std::variant<const FontSrc*, const std::string, InitFontSenseAuto> Init;

struct Config;

class TINYTEX_EXPORT TinyTeX {
private:
  static Config* _config;

public:
  /** The version of the library */
  static std::string version();

  /**
   * Initialize TinyTeX context with given math font spec, at least we need
   * a math font to layout formulas.
   *
   * @param mathFontSrc the font source to load math font
   */
  static void init(Init init);

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
   * was not given when parsing.
   *
   * @param name the math font name, must exists or an `ex_invalid_param`
   * will be thrown when parsing.
   */
  static void setDefaultMathFont(const std::string& name);

  /**
   * Set the default main font to show formulas while main font
   * was not given when parsing. However, if no main font was loaded, the
   * engine will use the math font to render the glyphs wrapped by command
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

  /**
   * Set if use path to render glyphs, only works when compile option
   * GLYPH_RENDER_TYPE equals to 0 (that means render glyphs use font and path
   * both), otherwise this function takes no effect.
   */
  static void setRenderGlyphUsePath(bool use);

  /** Test if currently use path to render glyphs. */
  static bool isRenderGlyphUsePath();

  /**
   * Parse LaTeX string to Render
   *
   * @param tex the LaTeX formatted string (in UTF-8)
   * @param width the width of the 2D graphics context (in pixel)
   * @param textSize the text size in pixel
   * @param lineSpace the line space in pixel
   * @param fg the foreground color
   * @param mathFontName the math font name, default is empty
   * @param mainFontName the main font name, default is empty
   */
  static Render* parse(
    const std::string& tex,
    float width,
    float textSize,
    float lineSpace,
    color fg,
    const std::string& mathFontName = "",
    const std::string& mainFontName = ""
  );

  /** Release the TinyTeX context */
  static void release();
};

}  // namespace tinytex

#endif  // TINYTEX_TINYTEX_H
