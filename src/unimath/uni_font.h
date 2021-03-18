#ifndef UNI_FONT_INCLUDED
#define UNI_FONT_INCLUDED

#include <map>

#include "common.h"
#include "utils/utils.h"
#include "otf/otf.h"

namespace tex {

enum class MathType : u8 {
  none,
  digit,
  latinSmall,
  latinCapital,
  greekSmall,
  greekCapital,
};

/** Represents math-version for digit, latin and greek alphabets */
struct MathVersion {
private:
  c32 _codepoints[6];

  /** Get the MathType and the version-specific offset of the given codepoint. */
  static std::pair<MathType, c32> ofChar(c32 codepoint);

public:
  no_copy_assign(MathVersion);

  MathVersion(
    c32 digit, c32 latinSmall, c32 latinCapital, c32 greekSmall, c32 greekCapital
  ) noexcept;

  /** Map an Unicode char to version-specific Unicode char. */
  c32 map(c32 codepoint) const;
};

/** Represents a open-type font with font-file and font-spec */
struct OtfFont final {
  const i32 _id;
  const std::string _fontFile;
  const sptr<const Otf> _otf;

  OtfFont(i32 id, std::string fontFile, const std::string& clmFile);

  inline const Otf& otf() const { return *_otf; }
};

/** Represents a character with its font and glyph id */
struct Char final {
  c32 _originCode;
  c32 _mappedCode;
  i32 _fontId;
  i32 _glyphId;
};

struct FontFamily final {
private:
  std::map<std::string, sptr<const OtfFont>> _styles;

public:
  no_copy_assign(FontFamily);

  FontFamily() = default;

  /** Add a font to this family with given style */
  void add(const std::string& styleName, const sptr<const OtfFont>& font);

  /** Get the font corresponding to the style */
  sptr<const OtfFont> get(const std::string& styleName) const;
};

/**
 * Parameters to load fonts.
 *
 * [style-name, font-file, clm-file]
 */
using FontSpec = std::tuple<std::string, std::string, std::string>;

/** Enum represents all font style */
enum class FontStyle : u16 {
  none = 0,
  rm = 0b1,
  bf = 0b10,
  it = 0b100,
  cal = 0b1000,
  frak = 0b10000,
  bb = 0b100000,
  sf = 0b1000000,
  tt = 0b10000000,
  // composed styles
  bfit = bf | it,
  bfcal = bf | cal,
  bffrak = bf | frak,
  sfbf = bf | sf,
  sfit = it | sf,
  sfbfit = bf | it | sf
};

class FontContext {
private:
  /** style name to version map */
  static std::map<std::string, sptr<const MathVersion>> _mathStyles;
  /** font style to version map */
  static std::map<FontStyle, sptr<const MathVersion>> _mathVersions;

  static int _lastId;
  static std::vector<sptr<const OtfFont>> _fonts;

  static std::map<std::string, sptr<FontFamily>> _mainFonts;
  static std::map<std::string, sptr<const OtfFont>> _mathFonts;

  /**
   * Get the font style from the given name
   *
   * @param name the style name
   * @return the corresponding FontStyle or FontStyle::none if not found
   */
  static FontStyle fontStyleOf(const std::string& name);

  sptr<FontFamily> _mainFont = nullptr;
  sptr<const OtfFont> _mathFont = nullptr;

public:
  static constexpr i32 NO_FONT = -1;

  /**
   * Add main font (collection) to context.
   *
   * @param versionName the name of this font (collection)
   * @param params font-spec to load
   */
  static void addMainFont(
    const std::string& versionName,
    const std::vector<FontSpec>& params
  );

  /** Add math font to context. */
  static void addMathFont(const FontSpec& params);

  /**
   * Set math style to display formulas.
   *
   * @param styleName one of the TeX, ISO, French, and upright
   */
  static void setMathStyle(const std::string& styleName);

  /** Get font-spec from given id, return nullptr if not found. */
  static sptr<const OtfFont> getFont(i32 id);

  /** Select math font by the given version name */
  void selectMathFont(const std::string& versionName);

  /** Select main font by the given version name */
  void selectMainFont(const std::string& versionName);

  /** Get the math font currently in use */
  inline const OtfFont& mathFont() { return *_mathFont; }

  /** Get the id of the math font currently in use */
  inline i32 mathFontId() const { return _mathFont->_id; }

  /** Get the char-object from given symbol */
  Char getChar(const std::string& symbol) const;

  /** Get the char-object from given code and styleName */
  Char getChar(c32 code, const std::string& styleName, bool isMathMode) const;

  /** Get the char-object from the given code and style */
  Char getChar(c32 code, FontStyle style, bool isMathMode) const;
};

}  // namespace tex

#endif
