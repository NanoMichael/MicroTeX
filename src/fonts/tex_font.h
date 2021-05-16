#ifndef TEX_FONT_INCLUDED
#define TEX_FONT_INCLUDED

#include "common.h"
#include "fonts/font_basic.h"

namespace tex {

/**
 * An interface representing a "TeXFont", which is responsible for all the
 * necessary fonts and font information.
 */
class TeXFont {
public:
  static const int NO_FONT;

  // TODO
  virtual float getScaleFactor() = 0;

  virtual float getAxisHeight(TexStyle style) = 0;

  virtual float getBigOpSpacing1(TexStyle style) = 0;

  virtual float getBigOpSpacing2(TexStyle style) = 0;

  virtual float getBigOpSpacing3(TexStyle style) = 0;

  virtual float getBigOpSpacing4(TexStyle style) = 0;

  virtual float getBigOpSpacing5(TexStyle style) = 0;

  /**
   * Get a Char-object specifying the given character in the given text style
   * with metric information depending on the given "style".
   * @param c
   *      alphanumeric character
   * @param textStyle
   *      the text style in which the character should be drawn
   * @param style
   *      the style in which the atom should be drawn
   * @return
   *      the Char-object specifying the given character in the given text style
   * @throw ex_text_style_mapping_not_found
   *      if the text style mapping not found
   */
  virtual Char getChar(
    wchar_t c,
    const std::string& textStyle,
    TexStyle style) = 0;

  /**
   * Get a Char-object for this specific character containing the metric information
   * @param cf
   *      CharFont-object determine a specific character of a specific font
   * @param style
   *      the style in which the atom should be drawn
   * @return the Char-object for this character containing metric information
   */
  virtual Char getChar(const CharFont& cf, TexStyle style) = 0;

  /**
   * Get a Char-object for the given symbol with metric information depending on "style"
   * @param name
   *      the symbol name
   * @param style
   *      the style in which the atom should be drawn
   * @return a Char-object for this symbol with metric information
   */
  virtual Char getChar(const std::string& name, TexStyle style) = 0;

  /**
   * Get a Char-object specifying the given character in the default text style
   * with metric information depending on the given "style"
   * @param c
   *      alphanumeric character
   * @param style
   *      the style in which the atom should be drawn
   * @return the Char-object specifying the given character in the default text style
   */
  virtual Char getDefaultChar(wchar_t c, TexStyle style) = 0;

  virtual float getDefaultRuleThickness(TexStyle style) = 0;

  virtual float getDenom1(TexStyle style) = 0;

  virtual float getDenom2(TexStyle style) = 0;

  /**
   * Get an Extension-object for the given Char containing the 4 possible
   * parts to build an arbitrary large variant. This will only be called if
   * isExtensionChar(Char) returns true.
   * @param c
   *      a Char-object for a specific character
   * @param style
   *      the style in which the atom should be drawn
   * @return an extension object containing the 4 possible parts
   */
  virtual Extension* getExtension(const Char& c, TexStyle style) = 0;

  /**
   * Get the kern value to be inserted between the given characters in the
   * given style
   * @param left
   *      left character
   * @param right
   *      right character
   * @param style
   *      the style in which the atom should be drawn
   * @return the kern value between both characters (default 0)
   */
  virtual float getKern(const CharFont& left, const CharFont& right, TexStyle style) = 0;

  /**
   * Get the ligature that replaces both characters (if any).
   * @param left
   *      left character
   * @param right
   *      right character
   * @return a ligature replacing both characters (or null if no any ligature)
   */
  virtual sptr<CharFont> getLigature(const CharFont& left, const CharFont& right) = 0;

  /**
   * Get the id of mu font
   */
  virtual int getMuFontId() = 0;

  /**
   * Get the next larger version of the given character. This is only called
   * if hasNextLarger(Char) returns true.
   * @param c
   *      specified character
   * @param style
   *      the style in which the atom should be drawn
   * @return the next larger version of this character
   */
  virtual Char getNextLarger(const Char& c, TexStyle style) = 0;

  virtual float getNum1(TexStyle style) = 0;

  virtual float getNum2(TexStyle style) = 0;

  virtual float getNum3(TexStyle style) = 0;

  virtual float getQuad(TexStyle style, int fontCode) = 0;

  /**
   * @return the point size of this TeXFont
   */
  virtual float getSize() = 0;

  /**
   * Get the kern amount of the character defined by the given CharFont
   * followed by the "skewChar" of it's font. This is used in the algorithm
   * for placing an accent above a single character.
   * @param cf
   *      the character an it's font above which an accent has to be placed
   * @param style
   *      the render style
   * @return the kern amount of the character defined by cf followed by the
   * "skewChar" of it's font
   */
  virtual float getSkew(const CharFont& cf, TexStyle style) = 0;

  virtual float getSpace(TexStyle style) = 0;

  virtual float getSub1(TexStyle style) = 0;

  virtual float getSub2(TexStyle style) = 0;

  virtual float getSubDrop(TexStyle style) = 0;

  virtual float getSup1(TexStyle style) = 0;

  virtual float getSup2(TexStyle style) = 0;

  virtual float getSup3(TexStyle style) = 0;

  virtual float getSupDrop(TexStyle style) = 0;

  virtual float getXHeight(TexStyle style, int fontCode) = 0;

  virtual float getEM(TexStyle style) = 0;

  /**
   * Test if a character has larger version.
   * 
   * @param c
   *      a character
   * @return true if the given character has a larger version, false otherwise
   */
  virtual bool hasNextLarger(const Char& c) = 0;

  virtual bool hasSpace(int font) = 0;

  virtual void setBold(bool bold) = 0;

  /** Test if this font is bold */
  virtual bool isBold() = 0;

  virtual void setRoman(bool rm) = 0;

  /** Test if this font is roman */
  virtual bool isRoman() = 0;

  virtual void setTt(bool tt) = 0;

  /** Test if this font is type-writer */
  virtual bool isTt() = 0;

  virtual void setIt(bool it) = 0;

  /** Test if this font is italic */
  virtual bool isIt() = 0;

  virtual void setSs(bool ss) = 0;

  /** Test if this font is sans-serif */
  virtual bool isSs() = 0;

  /**
   * Test if the given character is an extension character.
   * 
   * @param c
   *      a character
   * @return true if the given character contains extension information to
   * build an arbitrary large version of this character.
   */
  virtual bool isExtensionChar(const Char& c) = 0;

  virtual sptr<TeXFont> copy() = 0;

  virtual ~TeXFont();
};

}  // namespace tex

#endif
