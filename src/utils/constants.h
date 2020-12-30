#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

namespace tex {

enum TeXConstants {

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
  STYLE_SCRIPT_SCRIPT = 6
};
}

#endif
