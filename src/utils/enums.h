#ifndef ENUMS_INCLUDED
#define ENUMS_INCLUDED

#include "utils/utils.h"

namespace tex {

/** Formula alignment. */
enum class Alignment : i8 {
  /** Extra space will be added to the right of the formula. */
  left,
  /** Extra space will be added to the left of the formula. */
  right,
  /**
   * The formula will be centered in the middle. This constant can
   * be used for both horizontal and vertical alignment.
   */
  center,
  /** Extra space will be added under the formula. */
  top,
  /** Extra space will be added above the formula. */
  bottom,
  none = -1
};

/** Space amount between formulas. */
enum class SpaceType : i8 {
  thinMuSkip = 1,
  medMuSkip = 2,
  thickMuSkip = 3,
  negThinMuSkip = -1,
  negMedMuSkip = -2,
  negThickMuSkip = -3,
  quad = 3,
  none = 0
};

/** Script display type */
enum class LimitsType : i8 {
  normal,
  noLimits,
  limits
};

enum class AtomType : i8 {
  /** Ordinary symbol, e.g. "slash" */
  ordinary,
  /** Big operator, e.g. "sum" */
  bigOperator,
  /** Binary operator, e.g. "plus" */
  binaryOperator,
  /** Relation, e.g. "equals" */
  relation,
  /** Opening symbol, e.g. "lbrace" */
  opening,
  /** Closing symbol, e.g. "rbrace" */
  closing,
  /** Punctuation, e.g. "comma" */
  punctuation,
  /** Inner atom (NOT FOR SYMBOLS) */
  inner,
  /** Accent, e.g. "hat" */
  accent = 10,
  /** Inter-text in matrix environment */
  interText,
  /** Multi-column in matrix environment */
  multiColumn,
  /** Horizontal line in matrix environment */
  hline,
  /** Multi-row in matrix environment */
  multiRow,
  /** No type specified */
  none = -1
};

enum class TexStyle : i8 {
  /**
   * The larger versions of big operators are used and limits are placed under
   * and over these operators (default). Symbols are rendered in the largest
   * size.
   */
  display,
  display1,
  /**
   * The small versions of big operator are used and limits are attached to
   * these operators as scripts (default). The same size as in the display
   * style is used to render symbols.
   */
  text,
  text1,
  /** The same as the the text style, but symbols are rendered in a smaller size. */
  script,
  script1,
  /** The same as the script style, but symbols are rendered in a smaller size. */
  scriptScript,
  scriptScript1
};

enum class UnitType : i8 {
  /** 1 em = the width of the capital 'M' in the current font */
  em,
  /** 1 ex = the height of the character 'x' in the current font */
  ex,
  pixel,
  /** postscript point */
  point,
  /** 1 pica = 12 point*/
  pica,
  /** 1 mu = 1/18 em */
  mu,
  /** 1 cm = 28.346456693 point */
  cm,
  /** 1 mm = 1/10 cm */
  mm,
  /** 1 in = 72 point */
  in,
  /** 1 sp = 65536 point */
  sp,
  pt,
  dd,
  cc,
  /** 1 x8 = 1 default rule thickness */
  x8,
  none = -1
};

}  // namespace tex

#endif
