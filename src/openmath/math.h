#ifndef MATH_INCLUDED
#define MATH_INCLUDED

#include "utils/types.h"

namespace tex {

#define MATH_CONSTS_COUNT 56

/** 
 * Defines a number of constants required to properly position elements of mathematical formulas.
 * See [https://docs.microsoft.com/en-us/typography/opentype/spec/math#mathconstants-table] for
 * details.
 * 
 * Device-table is JUST IGNORED, so all the fields are represent as signed int16 directly.
 */
struct MathConsts {
private:
  /** Array to represents all constants */
  int16 fields[MATH_CONSTS_COUNT];

public:
  __no_copy_assign(MathConsts);

  inline int16 scriptPercentScaleDown() const { return fields[0]; }

  inline int16 scriptScriptPercentScaleDown() const { return fields[1]; }

  inline int16 delimitedSubFormulaMinHeight() const { return fields[2]; }

  inline int16 displayOperatorMinHeight() const { return fields[3]; }

  inline int16 mathLeading() const { return fields[4]; }

  inline int16 axisHeight() const { return fields[5]; }

  inline int16 accentBaseHeight() const { return fields[6]; }

  inline int16 flattenedAccentBaseHeight() const { return fields[7]; }

  inline int16 subscriptShiftDown() const { return fields[8]; }

  inline int16 subscriptTopMax() const { return fields[9]; }

  inline int16 subscriptBaselineDropMin() const { return fields[10]; }

  inline int16 superscriptShiftUp() const { return fields[11]; }

  inline int16 superscriptShiftUpCramped() const { return fields[12]; }

  inline int16 superscriptBottomMin() const { return fields[13]; }

  inline int16 superscriptBaselineDropMax() const { return fields[14]; }

  inline int16 subSuperscriptGapMin() const { return fields[15]; }

  inline int16 superscriptBottomMaxWithSubscript() const { return fields[16]; }

  inline int16 spaceAfterScript() const { return fields[17]; }

  inline int16 upperLimitGapMin() const { return fields[18]; }

  inline int16 upperLimitBaselineRiseMin() const { return fields[19]; }

  inline int16 lowerLimitGapMin() const { return fields[20]; }

  inline int16 lowerLimitBaselineDropMin() const { return fields[21]; }

  inline int16 stackTopShiftUp() const { return fields[22]; }

  inline int16 stackTopDisplayStyleShiftUp() const { return fields[23]; }

  inline int16 stackBottomShiftDown() const { return fields[24]; }

  inline int16 stackBottomDisplayStyleShiftDown() const { return fields[25]; }

  inline int16 stackGapMin() const { return fields[26]; }

  inline int16 stackDisplayStyleGapMin() const { return fields[27]; }

  inline int16 stretchStackTopShiftUp() const { return fields[28]; }

  inline int16 stretchStackBottomShiftDown() const { return fields[29]; }

  inline int16 stretchStackGapAboveMin() const { return fields[30]; }

  inline int16 stretchStackGapBelowMin() const { return fields[31]; }

  inline int16 fractionNumeratorShiftUp() const { return fields[32]; }

  inline int16 fractionNumeratorDisplayStyleShiftUp() const { return fields[33]; }

  inline int16 fractionDenominatorShiftDown() const { return fields[34]; }

  inline int16 fractionDenominatorDisplayStyleShiftDown() const { return fields[35]; }

  inline int16 fractionNumeratorGapMin() const { return fields[36]; }

  inline int16 fractionNumDisplayStyleGapMin() const { return fields[37]; }

  inline int16 fractionRuleThickness() const { return fields[38]; }

  inline int16 fractionDenominatorGapMin() const { return fields[39]; }

  inline int16 fractionDenomDisplayStyleGapMin() const { return fields[40]; }

  inline int16 skewedFractionHorizontalGap() const { return fields[41]; }

  inline int16 skewedFractionVerticalGap() const { return fields[42]; }

  inline int16 overbarVerticalGap() const { return fields[43]; }

  inline int16 overbarRuleThickness() const { return fields[44]; }

  inline int16 overbarExtraAscender() const { return fields[45]; }

  inline int16 underbarVerticalGap() const { return fields[46]; }

  inline int16 underbarRuleThickness() const { return fields[47]; }

  inline int16 underbarExtraDescender() const { return fields[48]; }

  inline int16 radicalVerticalGap() const { return fields[49]; }

  inline int16 radicalDisplayStyleVerticalGap() const { return fields[50]; }

  inline int16 radicalRuleThickness() const { return fields[51]; }

  inline int16 radicalExtraAscender() const { return fields[52]; }

  inline int16 radicalKernBeforeDegree() const { return fields[53]; }

  inline int16 radicalKernAfterDegree() const { return fields[54]; }

  inline int16 radicalDegreeBottomRaisePercent() const { return fields[55]; }
};

}  // namespace tex

#endif
