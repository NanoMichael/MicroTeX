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

  /** Get the MathType and the version-specific offset of given codepoint. */
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
};

struct Char final {
  c32 _originCode;
  c32 _mappedCode;
  i32 _fontId;
};

struct UniGlyph {
private:
  c32 _unicode;
  const Glyph* _glyph;
  const sptr<const OtfFont> _font;

public:
  UniGlyph(c32 unicode, const sptr<const OtfFont>& font);

  inline c32 unicode() const { return _unicode; }

  inline const Glyph& glyph() const { return *_glyph; }

  inline const OtfFont& font() const { return *_font; }
};

struct TextFont {
private:
  std::map<std::string, sptr<const OtfFont>> _styles;

public:
  no_copy_assign(TextFont);

  TextFont() = default;

  sptr<const OtfFont>& operator[](const std::string& styleName);
};

/**
 * Parameters to load fonts.
 *
 * [style-name, font-file, clm-file]
 */
using FontSpec = std::tuple<std::string, std::string, std::string>;

/** Enum represents all font style */
enum class FontStyle : u8 {
  rm,
  bf,
  it,
  cal,
  frak,
  bb,
  sf,
  tt
};

class FontContext {
private:


  static const std::string _emptyVersionName;
  static const std::string _defaultVersionName;
  /** style name to version map */
  static std::map<std::string, sptr<const MathVersion>> _mathStyles;
  /** version name to version map */
  static std::map<std::string, sptr<const MathVersion>> _mathVersions;

  static int _lastId;
  static std::vector<sptr<const OtfFont>> _fonts;

  static std::map<std::string, sptr<TextFont>> _mainFonts;
  static std::map<std::string, sptr<const OtfFont>> _mathFonts;

  sptr<TextFont> _mainFont = nullptr;
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

  void selectMathFont(const std::string& versionName);

  void selectMainFont(const std::string& versionName);

  inline const OtfFont& mathFont() { return *_mathFont; }

  inline i32 muFontId() const { return _mathFont->_id; }

  UniGlyph glyphOf(c32 codepoint, const std::string& versionName, bool isMathMode) const;
};

}  // namespace tex

#endif
