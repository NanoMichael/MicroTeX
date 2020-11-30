#ifndef MATH_INCLUDED
#define MATH_INCLUDED

#include "utils/types.h"

namespace tex {

/** 
 * Defines a number of constants required to properly position elements of mathematical formulas.
 * See [https://docs.microsoft.com/en-us/typography/opentype/spec/math#mathconstants-table] for
 * details.
 */
struct MathConsts {
  uint16 scriptPercentScaleDown;
  uint16 scriptScriptPercentScaleDown;
  uint16 delimitedSubFormulaMinHeight;
  uint16 displayOperatorMinHeight;
  uint16 mathLeading;
  uint16 axisHeight;
  uint16 accentBaseHeight;
  uint16 flattenedAccentBaseHeight;
  uint16 subscriptShiftDown;
  uint16 subscriptTopMax;
  uint16 subscriptBaselineDropMin;
  uint16 superscriptShiftUp;
  uint16 superscriptShiftUpCramped;
  uint16 superscriptBottomMin;
  uint16 superscriptBaselineDropMax;
  uint16 subSuperscriptGapMin;
  uint16 superscriptBottomMaxWithSubscript;
  uint16 spaceAfterScript;
  uint16 upperLimitGapMin;
  uint16 upperLimitBaselineRiseMin;
  uint16 lowerLimitGapMin;
  uint16 lowerLimitBaselineDropMin;
  uint16 stackTopShiftUp;
  uint16 stackTopDisplayStyleShiftUp;
  uint16 stackBottomShiftDown;
  uint16 stackBottomDisplayStyleShiftDown;
  uint16 stackGapMin;
  uint16 stackDisplayStyleGapMin;
  uint16 stretchStackTopShiftUp;
  uint16 stretchStackBottomShiftDown;
  uint16 stretchStackGapAboveMin;
  uint16 stretchStackGapBelowMin;
  uint16 fractionNumeratorShiftUp;
  uint16 fractionNumeratorDisplayStyleShiftUp;
  uint16 fractionDenominatorShiftDown;
  uint16 fractionDenominatorDisplayStyleShiftDown;
  uint16 fractionNumeratorGapMin;
  uint16 fractionNumDisplayStyleGapMin;
  uint16 fractionRuleThickness;
  uint16 fractionDenominatorGapMin;
  uint16 fractionDenomDisplayStyleGapMin;
  uint16 skewedFractionHorizontalGap;
  uint16 skewedFractionVerticalGap;
  uint16 overbarVerticalGap;
  uint16 overbarRuleThickness;
  uint16 overbarExtraAscender;
  uint16 underbarVerticalGap;
  uint16 underbarRuleThickness;
  uint16 underbarExtraDescender;
  uint16 radicalVerticalGap;
  uint16 radicalDisplayStyleVerticalGap;
  uint16 radicalRuleThickness;
  uint16 radicalExtraAscender;
  uint16 radicalKernBeforeDegree;
  uint16 radicalKernAfterDegree;
  uint16 radicalDegreeBottomRaisePercent;
};

}  // namespace tex

#endif
