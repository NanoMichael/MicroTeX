#ifndef MICROTEX_MICROTEX_H
#define MICROTEX_MICROTEX_H

#include "microtexexport.h"
#include "microtexconfig.h"
#include "render/render.h"
#include "unimath/font_src.h"
#include "unimath/font_meta.h"

#include <string>
#include <vector>

#ifdef HAVE_AUTO_FONT_FIND

#include <variant>

namespace microtex {

struct MICROTEX_EXPORT InitFontSenseAuto {
};

/**
 * MicroTeX context initialization.
 * <ol>
 *  <li> If a FontSrc presents, the context will load math font from the given font source.
 *  <li> If a InitFontSenseAuto presents, fill the search dirs by following rules:
 *    <ul>
 *      <li> If environment variable `MICROTEX_FONTDIR` is set, add it into search dirs.
 *      <li> If environment variable `XDG_DATA_HOME` is set, add `${XDG_DATA_HOME}/microtex` into
 *           search dirs.
 *      <li> If environment variable `XDG_DATA_DIRS` is set, iterate over the list and add the sub
 *           dir `microtex` of each item into search dirs.
 *      <li> If the current platform is WIN32, add the dir `share/microtex` where its parent is the
 *           executable running dir into search dirs.
 *      <li> Otherwise, try the following ways:
 *      <ul>
 *        <li> If environment variable `HOME` is set, add dir `${HOME}/.local/share/microtex` into
 *             search dirs.
 *        <li> Add `/usr/local/share/microtex` into search dirs.
 *        <li> Add `/usr/share/microtex` into search dirs.
 *      </ul>
 *      And then iterate over the search dirs, add all found fonts to context, and select the first
 *      found math font as the default.
 *    </ul>
 *  <li> If a string presents, follow the above way to init the context but select the math font
 *       which its name was given by this string as the default.
 * </ol>
 */
using Init = std::variant<const FontSrc*, const std::string, InitFontSenseAuto>;

} // namespace microtex

#endif // HAVE_AUTO_FONT_FIND

namespace microtex {

struct Config;

struct MICROTEX_EXPORT OverrideTeXStyle {
  bool enable;
  TexStyle style;
};

class MICROTEX_EXPORT MicroTeX {
private:
  static Config* _config;

public:
  /** The version of the library */
  static std::string version();

#ifdef HAVE_AUTO_FONT_FIND

  /**
   * Initialize MicroTeX context by given Init, at least we need a math font
   * to layout formulas.
   *
   * @returns the math font meta info
   */
  static FontMeta init(const Init& init);

#endif // HAVE_AUTO_FONT_FIND

  /**
   * Initialize the context with given math font source, at least we need a
   * math font to layout formulas.
   *
   * @param mathFontSrc the font source to load
   * @returns the math font meta info
   */
  static FontMeta init(const FontSrc& mathFontSrc);

  /** Check if context is initialized */
  static bool isInited();

  /** Add a font to context, returns its meta info. */
  static FontMeta addFont(const FontSrc& src);

  /**
   * Set the default math font to show formulas, if no math font was given
   * when parsing, the context will use the given default math font. If it
   * is not exists, this function takes no effect.
   *
   * @param name the math font name
   * @returns true if given math font exists, false otherwise
   */
  static bool setDefaultMathFont(const std::string& name);

  /**
   * Set the default main font to show formulas, if no main font was given
   * when parsing, the context will use the given default main font. However,
   * if no main font was loaded, the context will use the math font to render
   * the glyphs wrapped by command `text*`.
   *
   * @param family the main font family, if it is empty, that means fallback to
   * the math font
   * @returns true if given font exists (special case: always returns true if
   * given name is empty), false otherwise.
   */
  static bool setDefaultMainFont(const std::string& family);

  /** Get all the loaded math font names. */
  static std::vector<std::string> mathFontNames();

  /** Get all the loaded main font family names. */
  static std::vector<std::string> mainFontFamilies();

  /**
   * Override the style to display formulas. If #enable is true, the '$', '$$',
   * '\(' and '\[' will be ignored, and force to use the given overridden style.
   *
   * @param enable whether enable the overridden style, false to cancel
   * @param style the target style
   */
  static void overrideTexStyle(bool enable, TexStyle style = TexStyle::text);

  /** Test if has the ability use path to render glyphs. */
  static bool hasGlyphPathRender();

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
   * @param tex the (La)TeX string to parse (in UTF-8 encoding)
   * @param width the width of the 2D graphics context (in pixel) to limit the
   *    formula layout, the engine will trying to wrap the layout if it overflows
   *    the width, but will fails if formula cannot be split. Pass 0 if it is
   *    unlimited.
   * @param textSize the text size in pixel
   * @param lineSpace the line space in pixel
   * @param fg the foreground color
   * @param fillWidth whether fill the graphics context if is in inter-line mode,
   *    default is true. If given width is unlimited, the engine will not trying
   *    to split the formula, and the parsed Render will has its own intrinsic
   *    width.
   * @param overrideTeXStyle whether override the preset TeX style
   * @param mathFontName the math font name, empty to use the preset (the font
   *    passed in method init and setDefaultMathFont) math font.
   * @param mainFontFamily the main font family name, empty to use the preset (the
   *    font passed in method setDefaultMainFont or math font if not given) main
   *    font family.
   */
  static Render* parse(
    const std::string& tex,
    float width,
    float textSize,
    float lineSpace,
    color fg,
    bool fillWidth = true,
    const OverrideTeXStyle& overrideTeXStyle = {false, TexStyle::text},
    const std::string& mathFontName = "",
    const std::string& mainFontFamily = ""
  );

  /** Release the MicroTeX context */
  static void release();
};

}  // namespace microtex

#endif  // MICROTEX_MICROTEX_H
