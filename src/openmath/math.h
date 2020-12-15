#ifndef MATH_INCLUDED
#define MATH_INCLUDED

#include "utils/utils.h"

namespace tex {

#define MATH_CONSTS_COUNT 57

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
  int16 _fields[MATH_CONSTS_COUNT];

public:
  __no_copy_assign(MathConsts);

  inline int16 scriptPercentScaleDown() const { return _fields[0]; }

  inline int16 scriptScriptPercentScaleDown() const { return _fields[1]; }

  inline int16 delimitedSubFormulaMinHeight() const { return _fields[2]; }

  inline int16 displayOperatorMinHeight() const { return _fields[3]; }

  inline int16 mathLeading() const { return _fields[4]; }

  inline int16 axisHeight() const { return _fields[5]; }

  inline int16 accentBaseHeight() const { return _fields[6]; }

  inline int16 flattenedAccentBaseHeight() const { return _fields[7]; }

  inline int16 subscriptShiftDown() const { return _fields[8]; }

  inline int16 subscriptTopMax() const { return _fields[9]; }

  inline int16 subscriptBaselineDropMin() const { return _fields[10]; }

  inline int16 superscriptShiftUp() const { return _fields[11]; }

  inline int16 superscriptShiftUpCramped() const { return _fields[12]; }

  inline int16 superscriptBottomMin() const { return _fields[13]; }

  inline int16 superscriptBaselineDropMax() const { return _fields[14]; }

  inline int16 subSuperscriptGapMin() const { return _fields[15]; }

  inline int16 superscriptBottomMaxWithSubscript() const { return _fields[16]; }

  inline int16 spaceAfterScript() const { return _fields[17]; }

  inline int16 upperLimitGapMin() const { return _fields[18]; }

  inline int16 upperLimitBaselineRiseMin() const { return _fields[19]; }

  inline int16 lowerLimitGapMin() const { return _fields[20]; }

  inline int16 lowerLimitBaselineDropMin() const { return _fields[21]; }

  inline int16 stackTopShiftUp() const { return _fields[22]; }

  inline int16 stackTopDisplayStyleShiftUp() const { return _fields[23]; }

  inline int16 stackBottomShiftDown() const { return _fields[24]; }

  inline int16 stackBottomDisplayStyleShiftDown() const { return _fields[25]; }

  inline int16 stackGapMin() const { return _fields[26]; }

  inline int16 stackDisplayStyleGapMin() const { return _fields[27]; }

  inline int16 stretchStackTopShiftUp() const { return _fields[28]; }

  inline int16 stretchStackBottomShiftDown() const { return _fields[29]; }

  inline int16 stretchStackGapAboveMin() const { return _fields[30]; }

  inline int16 stretchStackGapBelowMin() const { return _fields[31]; }

  inline int16 fractionNumeratorShiftUp() const { return _fields[32]; }

  inline int16 fractionNumeratorDisplayStyleShiftUp() const { return _fields[33]; }

  inline int16 fractionDenominatorShiftDown() const { return _fields[34]; }

  inline int16 fractionDenominatorDisplayStyleShiftDown() const { return _fields[35]; }

  inline int16 fractionNumeratorGapMin() const { return _fields[36]; }

  inline int16 fractionNumeratorDisplayStyleGapMin() const { return _fields[37]; }

  inline int16 fractionRuleThickness() const { return _fields[38]; }

  inline int16 fractionDenominatorGapMin() const { return _fields[39]; }

  inline int16 fractionDenominatorDisplayStyleGapMin() const { return _fields[40]; }

  inline int16 skewedFractionHorizontalGap() const { return _fields[41]; }

  inline int16 skewedFractionVerticalGap() const { return _fields[42]; }

  inline int16 overbarVerticalGap() const { return _fields[43]; }

  inline int16 overbarRuleThickness() const { return _fields[44]; }

  inline int16 overbarExtraAscender() const { return _fields[45]; }

  inline int16 underbarVerticalGap() const { return _fields[46]; }

  inline int16 underbarRuleThickness() const { return _fields[47]; }

  inline int16 underbarExtraDescender() const { return _fields[48]; }

  inline int16 radicalVerticalGap() const { return _fields[49]; }

  inline int16 radicalDisplayStyleVerticalGap() const { return _fields[50]; }

  inline int16 radicalRuleThickness() const { return _fields[51]; }

  inline int16 radicalExtraAscender() const { return _fields[52]; }

  inline int16 radicalKernBeforeDegree() const { return _fields[53]; }

  inline int16 radicalKernAfterDegree() const { return _fields[54]; }

  inline int16 radicalDegreeBottomRaisePercent() const { return _fields[55]; }

  inline int16 minConnectorOverlap() const { return _fields[56]; }
};

}  // namespace tex

#endif
