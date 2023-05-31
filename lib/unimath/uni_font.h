#ifndef UNI_FONT_INCLUDED
#define UNI_FONT_INCLUDED

#include <map>
#include <set>

#include "graphic/font_style.h"
#include "otf/otf.h"
#include "unimath/font_meta.h"
#include "unimath/font_src.h"
#include "unimath/math_type.h"
#include "unimath/uni_char.h"
#include "unimath/uni_symbol.h"
#include "utils/utils.h"

namespace microtex {

/** Represents an open-type font with font-file and font-spec */
struct OtfFont final {
  const i32 id;
  /**
   * The font file (descriptor), may be empty if glyphs are drawn by graphical-paths.
   *
   * See [lib/unimath/font_src.h: FontSrc] for details.
   */
  const std::string fontFile;
  const sptr<const Otf> otfSpec;

  OtfFont(i32 id, sptr<const Otf> spec, std::string fontFile) noexcept;

  inline const Otf& otf() const { return *otfSpec; }
};

struct FontFamily final {
private:
  std::map<FontStyle, sptr<const OtfFont>> _styles;

public:
  no_copy_assign(FontFamily);

  FontFamily() = default;

  static const std::vector<FontStyle>& supportedStyles();

  /** Get the FontStyle from the given style name */
  static FontStyle fontStyleOf(const std::string& name);

  /** Add a font to this family with it's given style */
  void add(const sptr<const OtfFont>& font);

  /** Get the font corresponding to the given style, return null if not found */
  sptr<const OtfFont> get(FontStyle style) const;
};

/**************************************************************************************************/

class FontContext {
private:
  sptr<FontFamily> _mainFont = nullptr;
  sptr<const OtfFont> _mathFont = nullptr;

public:
  no_copy_assign(FontContext);

  FontContext() = default;

  static constexpr i32 NO_FONT = -1;

  /**
   * Get the math font style from the given name
   *
   * @param name the style name
   * @return the corresponding FontStyle or FontStyle::none if not found
   */
  static FontStyle mathFontStyleOf(const std::string& name);

  /**
   * Get the main font style from the given name
   *
   * @param name the style name
   * @return the corresponding FontStyle or FontStyle::none if not found
   */
  static FontStyle mainFontStyleOf(const std::string& name);

  /** Add a font to context, returns its meta info. */
  static FontMeta addFont(const FontSrc& src);

  /** Check if has math font */
  static bool hasMathFont();

  /** Test if given math font exists. */
  static bool isMathFontExists(const std::string& name);

  /** Test if given main font exists. */
  static bool isMainFontExists(const std::string& familyName);

  /** Get math font meta info by given font name. */
  static FontMeta mathFontMetaOf(const std::string& name);

  /** Get all the loaded math font names. */
  static std::vector<std::string> mathFontNames();

  /** Get all the loaded main font family names. */
  static std::vector<std::string> mainFontFamilies();

  /** Get font-spec from given id, return nullptr if not found. */
  static sptr<const OtfFont> getFont(i32 id);

  /** Select math font by the given name, returns false if not found. */
  bool selectMathFont(const std::string& name);

  /**
   * Select main font by the given name, empty to use math font and
   * returns false, and returns false if given family can not be found.
   */
  bool selectMainFont(const std::string& familyName);

  /** Get the math font currently in use */
  inline const OtfFont& mathFont() const { return *_mathFont; }

  /** Get the space size of main font. */
  u16 mainSpace();

  /** Get the id of the math font currently in use */
  inline i32 mathFontId() const { return _mathFont->id; }

  /** Get the char-object from given symbol */
  Char getChar(const Symbol& symbol, FontStyle style) const;

  /** Get the char-object from given code and styleName */
  Char getChar(c32 code, const std::string& styleName, bool isMathMode) const;

  /** Get the char-object from the given code and style */
  Char getChar(c32 code, FontStyle style, bool isMathMode) const;
};

}  // namespace microtex

#endif
