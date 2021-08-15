#ifndef LATEX_FORMULA_H
#define LATEX_FORMULA_H

#include <string>
#include <utility>

#include "atom/atom_basic.h"
#include "atom/atom_fence.h"
#include "atom/atom_vrow.h"
#include "core/parser.h"
#include "graphic/graphic.h"

namespace tex {

class CellSpecifier;

class TeXParser;

/**
 * Represents a logical mathematical formula that will be displayed (by creating
 * a TeXRender from it and painting it) using algorithms that are based
 * on the TeX algorithms.
 */
class Formula {
private:
  TeXParser _parser;

public:
  // predefined TeX formulas
  static std::map<std::string, sptr<Formula>> _predefFormulas;
  static std::map<std::string, std::string> _predefFormulaStrs;

  // character-to-symbol mappings
  static const std::map<c32, std::string> _charToSymbol;

  std::vector<sptr<MiddleAtom>> _middle;
  // the root atom of the "atom tree" that represents the formula
  sptr<Atom> _root;

  /** Create an empty Formula */
  Formula();

  /**
   * Creates a new Formula by parsing the given string (using a primitive
   * TeX parser).
   *
   * @param tp the given TeXParser
   * @param latex the string to be parsed
   * @param preprocess if do preprocessing
   * @param isMathMode if parse in math mode
   *
   * @throw ex_parse if the string could not be parsed correctly
   */
  Formula(
    const TeXParser& tp, const std::string& latex,
    bool preprocess = true, bool isMathMode = true
  );

  /**
   * Creates a new Formula by parsing the given string (using a primitive
   * TeX parser).
   *
   * @param latex the string to be parsed
   * @param preprocess if do preprocessing
   *
   * @throw ex_parse if the string could not be parsed correctly
   */
  explicit Formula(const std::string& latex, bool preprocess = true);

  /**
   * Change the text of the Formula and regenerate the root atom.
   *
   * @param latex the latex formula
   */
  void setLaTeX(const std::string& latex);

  /** Inserts an atom at the end of the current formula. */
  Formula* add(const sptr<Atom>& a);

  /** Convert this Formula into a box, with the given environment. */
  sptr<Box> createBox(Env& env);

  /** Test if this formula is in array mode. */
  virtual bool isArrayMode() const { return false; }

  /**
   * Get a predefined Formula.
   *
   * @param name the name of the predefined Formula
   * @return the predefined Formula or nullptr if not found
   */
  static sptr<Formula> get(const std::string& name);

  virtual ~Formula() = default;
};

/** Represents a formula in array mode. */
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

  ~ArrayFormula() override = default;
};

}  // namespace tex

#endif  // LATEX_FORMULA_H
