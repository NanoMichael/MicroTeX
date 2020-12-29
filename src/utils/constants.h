#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

namespace tex {

enum TeXConstants {

  /********************************** space size constants **************************************/
  THINMUSKIP = 1,
  MEDMUSKIP = 2,
  THICKMUSKIP = 3,
  NEGTHINMUSKIP = -1,
  NEGMEDMUSKIP = -2,
  NEGTHICKMUSKP = -3,

  QUAD = 3,

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
