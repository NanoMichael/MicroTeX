#ifndef MATH_INCLUDED
#define MATH_INCLUDED

#include "utils/types.h"

namespace tex {

/** 
 * Defines a number of constants required to properly position elements of mathematical formulas.
 * See [https://docs.microsoft.com/en-us/typography/opentype/spec/math#mathconstants-table] for
 * details.
 * 
 * All device-tables are JUST IGNORED, so all the fields are represent as signed int16 directly.
 */
struct MathConsts {
  int16 scriptPercentScaleDown;
  int16 scriptScriptPercentScaleDown;
  int16 delimitedSubFormulaMinHeight;
  int16 displayOperatorMinHeight;
  int16 mathLeading;
  int16 axisHeight;
  int16 accentBaseHeight;
  int16 flattenedAccentBaseHeight;
  int16 subscriptShiftDown;
  int16 subscriptTopMax;
  int16 subscriptBaselineDropMin;
  int16 superscriptShiftUp;
  int16 superscriptShiftUpCramped;
  int16 superscriptBottomMin;
  int16 superscriptBaselineDropMax;
  int16 subSuperscriptGapMin;
  int16 superscriptBottomMaxWithSubscript;
  int16 spaceAfterScript;
  int16 upperLimitGapMin;
  int16 upperLimitBaselineRiseMin;
  int16 lowerLimitGapMin;
  int16 lowerLimitBaselineDropMin;
  int16 stackTopShiftUp;
  int16 stackTopDisplayStyleShiftUp;
  int16 stackBottomShiftDown;
  int16 stackBottomDisplayStyleShiftDown;
  int16 stackGapMin;
  int16 stackDisplayStyleGapMin;
  int16 stretchStackTopShiftUp;
  int16 stretchStackBottomShiftDown;
  int16 stretchStackGapAboveMin;
  int16 stretchStackGapBelowMin;
  int16 fractionNumeratorShiftUp;
  int16 fractionNumeratorDisplayStyleShiftUp;
  int16 fractionDenominatorShiftDown;
  int16 fractionDenominatorDisplayStyleShiftDown;
  int16 fractionNumeratorGapMin;
  int16 fractionNumDisplayStyleGapMin;
  int16 fractionRuleThickness;
  int16 fractionDenominatorGapMin;
  int16 fractionDenomDisplayStyleGapMin;
  int16 skewedFractionHorizontalGap;
  int16 skewedFractionVerticalGap;
  int16 overbarVerticalGap;
  int16 overbarRuleThickness;
  int16 overbarExtraAscender;
  int16 underbarVerticalGap;
  int16 underbarRuleThickness;
  int16 underbarExtraDescender;
  int16 radicalVerticalGap;
  int16 radicalDisplayStyleVerticalGap;
  int16 radicalRuleThickness;
  int16 radicalExtraAscender;
  int16 radicalKernBeforeDegree;
  int16 radicalKernAfterDegree;
  int16 radicalDegreeBottomRaisePercent;
};

}  // namespace tex

#endif
