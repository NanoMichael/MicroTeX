#ifndef FORMULA_H_INCLUDED
#define FORMULA_H_INCLUDED

#include <string>
#include <utility>

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

  FontInfos(std::string ss, std::string s) : _sansserif(std::move(ss)), _serif(std::move(s)) {}
};

/**
 * Represents a logical mathematical formula that will be displayed (by creating
 * a TeXRender from it and painting it) using algorithms that are based
 * on the TeX algorithms.
 * <p>
 * These formula's can be built using the built-in primitive TeX parser (methods
 * with String arguments) or using other Formula objects. Most methods have
 * (an) equivalent(s) where one or more Formula arguments are replaced with
 * string arguments. These are just shorter notations, because all they do is
 * parse the string(s) to Formula's and call an equivalent method with (a)
 * Formula argument(s). Most methods also come in 2 variants. One kind will
 * use this Formula to build another mathematical construction and then
 * change this object to represent the newly build construction. The other kind
 * will only use other Formula's (or parse strings), build a mathematical
 * construction with them and insert this newly build construction at the end of
 * this Formula. Because all the provided methods return a pointer to this
 * (modified) Formula, method chaining is also possible.
 * <p>
 * Important: All the provided methods modify this Formula object, but
 * all the Formula arguments of these methods will remain unchanged and
 * independent of this Formula object!
 */
class Formula {
private:
  TeXParser _parser;

public:
  std::map<std::string, std::string> _xmlMap;
  // point-to-pixel conversion
  static float PIXELS_PER_POINT;

  // predefined TeX formulas
  static std::map<std::wstring, sptr<Formula>> _predefinedTeXFormulas;
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

  /**
   * Creates a new Formula by parsing the given string (using a primitive
   * TeX parser).
   *
   * @param tp the given TeXParser
   * @param latex the string to be parsed
   *
   * @throw ex_parse if the string could not be parsed correctly
   */
  Formula(const TeXParser& tp, const std::wstring& latex);

  Formula(const TeXParser& tp, const std::wstring& latex, bool preprocess);

  Formula(
    const TeXParser& tp, const std::wstring& latex,
    const std::string& textStyle, bool preprocess,
    bool isMathMode
  );

  /** Create an empty Formula */
  Formula();

  /**
   * Creates a new Formula by parsing the given string (using a primitive
   * TeX parser).
   *
   * @param latex the string to be parsed
   *
   * @throw ex_parse if the string could not be parsed correctly
   */
  explicit Formula(const std::wstring& latex);

  Formula(const std::wstring& latex, bool preprocess);

  /**
   * Change the text of the Formula and regenerate the root atom.
   *
   * @param latex the latex formula
   */
  void setLaTeX(const std::wstring& latex);

  /** Inserts an a at the end of the current formula. */
  Formula* add(const sptr<Atom>& a);

  /** Convert this Formula into a box, with the given style */
  sptr<Box> createBox(Environment& style);

  /** Test if this formula is in array mode. */
  virtual bool isArrayMode() const { return false; }

  /**
   * Get a predefined Formula.
   *
   * @param name the name of the predefined Formula
   * @return a <b>copy</b> of the predefined Formula
   *
   * @throw ex_formula_not_found
   *      if no predefined Formula is found with the given name
   */
  static sptr<Formula> get(const std::wstring& name);

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

  virtual ~Formula() = default;
};

class ArrayFormula : public Formula {
private:
  size_t _row, _col;

public:
  std::vector<std::vector<sptr<Atom>>> _array;
  std::map<int, std::vector<sptr<CellSpecifier>>> _rowSpecifiers;
  std::map<std::string, std::vector<sptr<CellSpecifier>>> _cellSpecifiers;

  ArrayFormula();

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

  virtual ~ArrayFormula() {}
};

}  // namespace tex

#endif  // FORMULA_H_INCLUDED
