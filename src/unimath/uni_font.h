#ifndef UNI_FONT_INCLUDED
#define UNI_FONT_INCLUDED

#include <map>

#include "common.h"
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
  );

  /** Map an Unicode char to version-specific Unicode char. */
  c32 map(c32 codepoint) const;
};

/** Represents a open-type font with font-file and clm-spec */
struct OtfFont {
private:
  const std::string _fontFile;
  const sptr<const Otf> _otf;

public:
  no_copy_assign(OtfFont);

  OtfFont(std::string fontFile, const std::string& clmFile);

  inline const std::string& fontFile() const { return _fontFile; }

  inline const Otf& otf() const { return *_otf; }
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

class FontContext {
private:
  static std::string _emptyVersionName;
  static std::string _defaultVersionName;
  /** style name to version map */
  static std::map<std::string, sptr<const MathVersion>> _mathStyles;
  /** version name to version map */
  static std::map<std::string, sptr<const MathVersion>> _mathVersions;

  static std::map<std::string, sptr<TextFont>> _mainFonts;
  static std::map<std::string, sptr<const OtfFont>> _mathFonts;

  sptr<TextFont> _mainFont = nullptr;
  sptr<const OtfFont> _mathFont = nullptr;

public:
  static void addMainFont(
    const std::string& versionName,
    const std::vector<FontSpec>& params
  );

  static void addMathFont(const FontSpec& params);

  static void setMathStyle(const std::string& styleName);

  void selectMathFont(const std::string& versionName);

  void selectMainFont(const std::string& versionName);

  inline const OtfFont& mathFont() { return *_mathFont; }

  UniGlyph glyphOf(c32 codepoint, const std::string& versionName, bool isMathMode) const;
};

}  // namespace tex

#endif
