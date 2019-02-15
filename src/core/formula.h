#ifndef FORMULA_H_INCLUDED
#define FORMULA_H_INCLUDED

#include "atom/atom_basic.h"
#include "core/parser.h"
#include "fonts/alphabet.h"
#include "graphic/graphic.h"

#include <string>

using namespace std;
using namespace tex;

namespace tex {

class TeXFont;
class CellSpecifier;
class TeXParser;

struct FontInfos {
    const string _sansserif;
    const string _serif;

    FontInfos(const string& ss, const string& s) : _sansserif(ss), _serif(s) {}
};

/**
 * Represents a logical mathematical formula that will be displayed (by creating
 * a @link TeXRender @endlink from it and painting it) using algorithms that are based
 * on the TeX algorithms.
 * <p>
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
 * <p>
 * <b> Important: All the provided methods modify this TeXFormula object, but
 * all the TeXFormula arguments of these methods will remain unchanged and
 * independent of this TeXFormula object!</b>
 */
class TeXFormula {
private:
    TeXParser _parser;

    void addImpl(const TeXFormula* f);

public:
    map<string, string> _xmlMap;
    // point-to-pixel conversion
    static float PIXELS_PER_POINT;

    // predefined TeX formulas
    static map<wstring, sptr<TeXFormula>> _predefinedTeXFormulas;
    static map<wstring, wstring> _predefinedTeXFormulasAsString;

    // character-to-symbol and character-to-delimiter mappings
    static map<int, string> _symbolMappings;
    static map<int, string> _symbolTextMappings;
    static map<int, string> _symbolFormulaMappings;
    static map<UnicodeBlock, FontInfos*> _externalFontMap;

    list<sptr<MiddleAtom>> _middle;
    // the root atom of the "atom tree" that represents the formula
    sptr<Atom> _root;
    // the current text style
    string _textStyle;

    TeXFormula(const TeXParser& tp);

    /**
     * Creates a new TeXFormula by parsing the given string (using a primitive
     * TeX parser).
     *
     * @param tp
     *      the given TeXParser
     * @param s
     *      the string to be parsed
     * @throw ex_parse
     *      if the string could not be parsed correctly
     */
    TeXFormula(const TeXParser& tp, const wstring& s) throw(ex_parse);

    TeXFormula(const TeXParser& tp, const wstring& s, bool firstpass) throw(ex_parse);

    /**
     * Creates a TeXFormula by parsing the given string in the given text style.
     * Used when a text style command was found in the parse string.
     */
    TeXFormula(const TeXParser& tp, const wstring& s, const string& textStyle) throw(ex_parse);

    TeXFormula(
        const TeXParser& tp, const wstring& s,
        const string& textStyle, bool firstpass, bool space) throw(ex_parse);

    /**
     * Create an empty TeXFormula
     */
    TeXFormula();

    /**
     * Creates a new TeXFormula by parsing the given string (using a primitive
     * TeX parser).
     *
     * @param s
     *      the string to be parsed
     * @throw ex_parse
     *      if the string could not be parsed correctly
     */
    TeXFormula(const wstring& s) throw(ex_parse);

    TeXFormula(const wstring& s, bool firstpass) throw(ex_parse);

    /**
     * Creates a TeXFormula by parsing the given string in the given text style.
     * Used when a text style command was found in the parse string.
     */
    TeXFormula(const wstring& s, const string& textStyle) throw(ex_parse);

    TeXFormula(
        const wstring& s, const string& textStyle, bool firstpass, bool space) throw(ex_parse);

    /**
     * Creates a new TeXFormula that is a copy of the given TeXFormula.
     * <p>
     * <b>Both TeXFormula's are independent of one another!</b>
     *
     * @param f
     *      the formula to be copied
     */
    TeXFormula(const TeXFormula* f);

    /**
     * Change the text of the TeXFormula and regenerate the root atom.
     *
     * @param latex
     *      the latex formula
     */
    void setLaTeX(const wstring& latex) throw(ex_parse);

    /**
     * Inserts an atom at the end of the current formula.
     */
    TeXFormula* add(const sptr<Atom>& el);

    TeXFormula* append(const wstring& s) throw(ex_parse);

    TeXFormula* append(bool isPartial, const wstring& s) throw(ex_parse);

    /**
     * Convert this TeXFormula into a box, with the given style
     */
    sptr<Box> createBox(_out_ TeXEnvironment& style);

    /**
     * Changes the background color of the <i>current</i> TeXFormula into the
     * given color. By default, a TeXFormula has no background color, it's
     * transparent. The backgrounds of subformula's will be painted on top of
     * the background of the whole formula! Any changes that will be made to
     * this TeXFormula after this background color was set, will have the
     * default background color (unless it will also be changed into another
     * color afterwards)!
     *
     * @param c
     *      the desired background color for the <i>current</i> TeXFormula
     * @return the modified TeXFormula
     */
    TeXFormula* setBackground(color c);

    /**
     * Changes the (foreground) color of the <i>current</i> TeXFormula into the
     * given color. By default, the foreground color of a TeXFormula is the
     * foreground color of the component on which the TeXRender (created from this
     * TeXFormula) will be painted. The color of subformula's overrides the
     * color of the whole formula. Any changes that will be made to this
     * TeXFormula after this color was set, will be painted in the default color
     * (unless the color will also be changed afterwards into another color)!
     *
     * @param c
     *      the desired foreground color for the <i>current</i> TeXFormula
     * @return the modified TeXFormula
     */
    TeXFormula* setColor(color c);

    /**
     * Sets a fixed left and right type of the current TeXFormula. This has an
     * influence on the glue that will be inserted before and after this
     * TeXFormula.
     *
     * @param left
     *      the left type of this formula @see TeXConstants
     * @param right
     *      the right type of this formula @see TeXConstants
     * @return the modified TeXFormula
     * @throw ex_invalid_atom_type
     *      if the given value does not represent a valid atom type
     */
    TeXFormula* setFixedTypes(int left, int right) throw(ex_invalid_atom_type);

    /**
     * Test if this formula is in array mode.
     */
    virtual bool isArrayMode() const { return false; }

    /**
     * Get a predefined TeXFormula.
     *
     * @param name
     *      the name of the predefined TeXFormula
     * @return a <b>copy</b> of the predefined TeXFormula
     * @throw ex_formula_not_found
     *      if no predefined TeXFormula is found with the given name
     */
    static sptr<TeXFormula> get(const wstring& name) throw(ex_formula_not_found);

    /**
     * Set the DPI of target
     *
     * @param dpi
     *      the target DPI
     */
    static void setDPITarget(float dpi);

    /**
     * Check if the given unicode-block is registered.
     */
    static bool isRegisteredBlock(const UnicodeBlock& block);

    static FontInfos* getExternalFont(const UnicodeBlock& block);

    static void addSymbolMappings(const string& file) throw(ex_res_parse);

    /**
     * Enable or disable debug mode.
     */
    static void setDEBUG(bool b);

    static void _init_();

    static void _free_();

    virtual ~TeXFormula() {}
};

class ArrayOfAtoms : public TeXFormula {
private:
    size_t _row, _col;

public:
    vector<vector<sptr<Atom>>> _array;
    map<int, vector<sptr<CellSpecifier>>> _rowSpecifiers;
    map<string, vector<sptr<CellSpecifier>>> _cellSpecifiers;

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
