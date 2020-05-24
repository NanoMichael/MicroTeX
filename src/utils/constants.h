#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

namespace tex {

enum TeXConstants {

  /********************************* alignment constants ****************************************/
  /**
   * Extra space will be added to the right of the formula
   */
  ALIGN_LEFT = 0,
  /**
   * Extra space will be added to the left of the formula
   */
  ALIGN_RIGHT,  // =1
  /**
   * The formula will be centered in the middle. this constant
   * can be used for both horizontal and vertical alignment
   */
  ALIGN_CENTER,  // =2
  /**
   * Extra space will be added under the formula
   */
  ALIGN_TOP,  // =3
  /**
   * Extra space will be added above the formula
   */
  ALIGN_BOTTOM,  // =4
  /**
   * None
   */
  ALIGN_NONE,  // =5

  /********************************** space size constants **************************************/
  THINMUSKIP    = 1,
  MEDMUSKIP     = 2,
  THICKMUSKIP   = 3,
  NEGTHINMUSKIP = -1,
  NEGMEDMUSKIP  = -2,
  NEGTHICKMUSKP = -3,

  QUAD = 3,

  /****************************** script display type constants *********************************/
  SCRIPT_NORMAL = 0,
  SCRIPT_NOLIMITS,
  SCRIPT_LIMITS,

  /*********************************** atom type constants **************************************/
  /**
   * Ordinary symbol, e.g. "slash"
   */
  TYPE_ORDINARY = 0,
  /**
   * Big operator, e.g. "sum"
   */
  TYPE_BIG_OPERATOR,  // =1
  /**
   * Binary operator, e.g. "plus"
   */
  TYPE_BINARY_OPERATOR,  // =2
  /**
   * Relation, e.g. "equals"
   */
  TYPE_RELATION,  // =3
  /**
   * Opening symbol, e.g. "lbrace"
   */
  TYPE_OPENING,  // =4
  /**
   * Closing symbol, e.g. "rbrace"
   */
  TYPE_CLOSING,  // =5
  /**
   * Punctuation symbol, e.g. "comma"
   */
  TYPE_PUNCTUATION,  // =6
  /**
   * Atom type: inner atom (NOT FOR SYMBOLS)
   */
  TYPE_INNER,  // =7
  /**
   * Accent, e.g. "hat"
   */
  TYPE_ACCENT = 10,
  /**
   * Inter-text in matrix environment
   */
  TYPE_INTERTEXT,  // =11
  /**
   * Multi-column in matrix envrionment
   */
  TYPE_MULTICOLUMN,  // =12
  /**
   * Horizontal line in matrix envrionment
   */
  TYPE_HLINE,  // =13
  /**
   * Multi-row in matrix envrionment
   */
  TYPE_MULTIROW,  // =14

  /****************************** over and under delimiter type constants ***********************/
  DELIM_BRACE = 0,
  DELIM_SQUARE_BRACKET,
  DELIM_BRACKET,
  DELIM_LEFT_ARROW,
  DELIM_RIGHT_ARROW,
  DELIM_LEFT_RIGHT_ARROW,
  DELIM_DOUBLE_LEFT_ARROW,
  DELIM_DOUBLE_RIGHT_ARROW,
  DELIM_DOUBLE_LEFT_RIGHT_ARROW,
  DELIM_SIGNLE_LINE,
  DELIM_DOUBLE_LINE,

  /************************************* TeX style constants ************************************/
  /**
   * Display style
   * @par
   * The larger versions of big operators are used and limits are placed under
   * and over these operators (default). Symbols are rendered in the largest
   * size.
   */
  STYLE_DISPLAY = 0,
  /**
   * Text style
   * @par
   * The small versions of big operator are used and limits are attached to
   * these operators as scripts (default). The same size as in the display
   * style is used to render symbols.
   */
  STYLE_TEXT = 2,
  /**
   * Script style
   * @par
   * The same as the the text style, but symbols are rendered in a smaller size.
   */
  STYLE_SCRIPT = 4,
  /**
   * Script_script style
   * @par
   * The same as the script style, but symbols are rendered in a smaller size.
   */
  STYLE_SCRIPT_SCRIPT = 6,

  /************************************ TeX unit constants **************************************/
  /**
   * 1 em = the width of the capital 'M' in the current font
   */
  UNIT_EM = 0,
  /**
   * 1 ex = the height of the character 'x' in the current font
   */
  UNIT_EX,
  UNIT_PIXEL,
  /**
   * postscript point
   */
  UNIT_POINT,
  /**
   * 1 pica = 12 point
   */
  UNIT_PICA,
  /**
   * 1 mu = 1/18 em (em taken from the "mufont")
   */
  UNIT_MU,
  /**
   * 1 cm = 28.346456693 point
   */
  UNIT_CM,
  /**
   * 1 mm = 2.8346456693 point
   */
  UNIT_MM,
  /**
   * 1 in = 72 point
   */
  UNIT_IN,
  /**
   * 1 sp = 65536 point
   */
  UNIT_SP,
  UNIT_PT,
  UNIT_DD,
  UNIT_CC,
  /**
   * 1 x8 = 1 default rule thickness
   */
  UNIT_X8
};
}

#endif
