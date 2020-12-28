#ifndef FORMULA_H_INCLUDED
#define FORMULA_H_INCLUDED

#include <string>

#include "atom/atom_basic.h"
#include "core/parser.h"
#include "fonts/alphabet.h"
#include "graphic/graphic.h"

namespace tex {

class TeXFont;
class CellSpecifier;
class TeXParser;

struct FontInfos {
  const std::string _sansserif;
  const std::string _serif;

  FontInfos(const std::string& ss, const std::string& s) : _sansserif(ss), _serif(s) {}
};

/**
 * Represents a logical mathematical formula that will be displayed (by creating
 * a TeXRender from it and painting it) using algorithms that are based
 * on the TeX algorithms.
 *
 * These formula's can be built using the built-in primitive TeX parser (methods
 * with String arguments) or using other TeXFormula objects. Most methods have
 * (an) equivalent(s) where one or more TeXFormula arguments are replaced with
 * String arguments. These are just shorter notations, because all they do is
 * parse the string(s) to TeXFormula's and call an equivalent method with (a)
 * TeXFormula argument(s). Most methods also come in 2 variants. One kind will
 * use this TeXFormula to build another mathematical construction and then
 * change this object to represent the newly build construction. The other kind
 * will only use other TeXFormula's (or parse strings), build a mathematical
 * construction with them and insert this newly build construction at the end of
 * this TeXFormula. Because all the provided methods return a pointer to this
 * (modified) TeXFormula, method chaining is also possible.
 *
 * Important: All the provided methods modify this TeXFormula object, but
 * all the TeXFormula arguments of these methods will remain unchanged and
 * independent of this TeXFormula object!
 */
class TeXFormula {
private:
  TeXParser _parser;

  void addImpl(const TeXFormula* f);

public:
  std::map<std::string, std::string> _xmlMap;
  // point-to-pixel conversion
  static float PIXELS_PER_POINT;

  // predefined TeX formulas
  static std::map<std::wstring, sptr<TeXFormula>> _predefinedTeXFormulas;
  static std::map<std::wstring, std::wstring> _predefinedTeXFormulasAsString;

  // character-to-symbol and character-to-delimiter mappings
  static std::map<int, std::string> _symbolMappings;
  static std::map<int, std::string> _symbolTextMappings;
  static std::map<int, std::string> _symbolFormulaMappings;
  static std::map<UnicodeBlock, FontInfos*> _externalFontMap;

  std::list<sptr<MiddleAtom>> _middle;
  // the root atom of the "atom tree" that represents the formula
  sptr<Atom> _root;
  // the current text style
  std::string _textStyle;

  TeXFormula(const TeXParser& tp);

  /**
   * Creates a new TeXFormula by parsing the given string (using a primitive
   * TeX parser).
   *
   * @param tp the given TeXParser
   * @param latex the string to be parsed
   *
   * @throw ex_parse if the string could not be parsed correctly
   */
  TeXFormula(const TeXParser& tp, const std::wstring& latex);

  TeXFormula(const TeXParser& tp, const std::wstring& s, bool firstpass);

  /**
   * Creates a TeXFormula by parsing the given string in the given text style.
   * Used when a text style command was found in the parse string.
   */
  TeXFormula(const TeXParser& tp, const std::wstring& latex, const std::string& textStyle);

  TeXFormula(
    const TeXParser& tp, const std::wstring& latex,
    const std::string& textStyle, bool firstpass,
    bool ignoreWhiteSpace  //
  );

  /** Create an empty TeXFormula */
  TeXFormula();

  /**
   * Creates a new TeXFormula by parsing the given string (using a primitive
   * TeX parser).
   *
   * @param latex the string to be parsed
   *
   * @throw ex_parse if the string could not be parsed correctly
   */
  TeXFormula(const std::wstring& latex);

  TeXFormula(const std::wstring& latex, bool firstpass);

  /**
   * Creates a TeXFormula by parsing the given string in the given text style.
   * Used when a text style command was found in the parse string.
   */
  TeXFormula(const std::wstring& latex, const std::string& textStyle);

  TeXFormula(
    const std::wstring& latex, const std::string& textStyle,
    bool firstpass, bool ignoreWhiteSpace  //
  );

  /**
   * Creates a new TeXFormula that is a copy of the given TeXFormula.
   *
   * Both TeXFormula's are independent of one another!
   *
   * @param f the formula to be copied
   */
  TeXFormula(const TeXFormula* f);

  /**
   * Change the text of the TeXFormula and regenerate the root atom.
   *
   * @param latex the latex formula
   */
  void setLaTeX(const std::wstring& latex);

  /** Inserts an atom at the end of the current formula. */
  TeXFormula* add(const sptr<Atom>& atom);

  TeXFormula* append(const std::wstring& latex);

  TeXFormula* append(bool isPartial, const std::wstring& latex);

  /** Convert this TeXFormula into a box, with the given style */
  sptr<Box> createBox(TeXEnvironment& style);

  /**
   * Changes the background color of the <i>current</i> TeXFormula into the
   * given color. By default, a TeXFormula has no background color, it's
   * transparent. The backgrounds of subformula's will be painted on top of
   * the background of the whole formula! Any changes that will be made to
   * this TeXFormula after this background color was set, will have the
   * default background color (unless it will also be changed into another
   * color afterwards)!
   *
   * @param bg the desired background color for the <i>current</i> TeXFormula
   * @return the modified TeXFormula
   */
  TeXFormula* setBackground(color bg);

  /**
   * Changes the (foreground) color of the <i>current</i> TeXFormula into the
   * given color. By default, the foreground color of a TeXFormula is the
   * foreground color of the component on which the TeXRender (created from this
   * TeXFormula) will be painted. The color of subformula's overrides the
   * color of the whole formula. Any changes that will be made to this
   * TeXFormula after this color was set, will be painted in the default color
   * (unless the color will also be changed afterwards into another color)!
   *
   * @param fg the desired foreground color for the <i>current</i> TeXFormula
   * @return the modified TeXFormula
   */
  TeXFormula* setColor(color fg);

  /**
   * Sets a fixed left and right type of the current TeXFormula. This has an
   * influence on the glue that will be inserted before and after this
   * TeXFormula.
   *
   * @param left the left type of this formula @see TeXConstants
   * @param right the right type of this formula @see TeXConstants
   * @return the modified TeXFormula
   *
   * @throw ex_invalid_atom_type
   *      if the given value does not represent a valid atom type
   */
  TeXFormula* setFixedTypes(int left, int right);

  /** Test if this formula is in array mode. */
  virtual bool isArrayMode() const { return false; }

  /**
   * Get a predefined TeXFormula.
   *
   * @param name the name of the predefined TeXFormula
   * @return a <b>copy</b> of the predefined TeXFormula
   *
   * @throw ex_formula_not_found
   *      if no predefined TeXFormula is found with the given name
   */
  static sptr<TeXFormula> get(const std::wstring& name);

  /**
   * Set the DPI of target
   *
   * @param dpi the target DPI
   */
  static void setDPITarget(float dpi);

  /** Check if the given unicode-block is registered. */
  static bool isRegisteredBlock(const UnicodeBlock& block);

  static FontInfos* getExternalFont(const UnicodeBlock& block);

  static void addSymbolMappings(const std::string& file);

  /** Enable or disable debug mode. */
  static void setDEBUG(bool b);

  static void _init_();

  static void _free_();

  virtual ~TeXFormula() {}
};

class ArrayOfAtoms : public TeXFormula {
private:
  size_t _row, _col;

public:
  std::vector<std::vector<sptr<Atom>>> _array;
  std::map<int, std::vector<sptr<CellSpecifier>>> _rowSpecifiers;
  std::map<std::string, std::vector<sptr<CellSpecifier>>> _cellSpecifiers;

  ArrayOfAtoms();

  void addCol();

  void addCol(int n);

  void insertAtomIntoCol(int col, const sptr<Atom>& atom);

  void addRow();

  void addRowSpecifier(const sptr<CellSpecifier>& spe);

  void addCellSpecifier(const sptr<CellSpecifier>& spe);

  int rows() const;

  int cols() const;

  sptr<VRowAtom> getAsVRow();

  void checkDimensions();

  virtual bool isArrayMode() const override { return true; }

  virtual ~ArrayOfAtoms() {}
};

}  // namespace tex

#endif  // FORMULA_H_INCLUDED
