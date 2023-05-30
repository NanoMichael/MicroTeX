#ifndef MICROTEX_MATH_CONSTS_H
#define MICROTEX_MATH_CONSTS_H

#include "utils/utils.h"

namespace microtex {

#define TEX_MATH_CONSTS_COUNT 57

class CLMReader;

/**
 * Defines a number of constants required to properly position elements of mathematical formulas.
 * See
 * <a href="https://docs.microsoft.com/en-us/typography/opentype/spec/math#mathconstants-table">
 * math constants specification
 * </a>
 * for details.
 * <p>
 * Device-table is JUST IGNORED, so all the fields are represent as microtex::i16 directly.
 */
struct MathConsts {
private:
  /** Array to represents all constants */
  i16 _fields[TEX_MATH_CONSTS_COUNT]{};

  MathConsts() = default;

public:
  no_copy_assign(MathConsts);

  inline i16 scriptPercentScaleDown() const { return _fields[0]; }

  inline i16 scriptScriptPercentScaleDown() const { return _fields[1]; }

  inline i16 delimitedSubFormulaMinHeight() const { return _fields[2]; }

  inline i16 displayOperatorMinHeight() const { return _fields[3]; }

  inline i16 mathLeading() const { return _fields[4]; }

  inline i16 axisHeight() const { return _fields[5]; }

  inline i16 accentBaseHeight() const { return _fields[6]; }

  inline i16 flattenedAccentBaseHeight() const { return _fields[7]; }

  inline i16 subscriptShiftDown() const { return _fields[8]; }

  inline i16 subscriptTopMax() const { return _fields[9]; }

  inline i16 subscriptBaselineDropMin() const { return _fields[10]; }

  inline i16 superscriptShiftUp() const { return _fields[11]; }

  inline i16 superscriptShiftUpCramped() const { return _fields[12]; }

  inline i16 superscriptBottomMin() const { return _fields[13]; }

  inline i16 superscriptBaselineDropMax() const { return _fields[14]; }

  inline i16 subSuperscriptGapMin() const { return _fields[15]; }

  inline i16 superscriptBottomMaxWithSubscript() const { return _fields[16]; }

  inline i16 spaceAfterScript() const { return _fields[17]; }

  inline i16 upperLimitGapMin() const { return _fields[18]; }

  inline i16 upperLimitBaselineRiseMin() const { return _fields[19]; }

  inline i16 lowerLimitGapMin() const { return _fields[20]; }

  inline i16 lowerLimitBaselineDropMin() const { return _fields[21]; }

  inline i16 stackTopShiftUp() const { return _fields[22]; }

  inline i16 stackTopDisplayStyleShiftUp() const { return _fields[23]; }

  inline i16 stackBottomShiftDown() const { return _fields[24]; }

  inline i16 stackBottomDisplayStyleShiftDown() const { return _fields[25]; }

  inline i16 stackGapMin() const { return _fields[26]; }

  inline i16 stackDisplayStyleGapMin() const { return _fields[27]; }

  // region TODO stretch stacks not used
  inline i16 stretchStackTopShiftUp() const { return _fields[28]; }

  inline i16 stretchStackBottomShiftDown() const { return _fields[29]; }

  inline i16 stretchStackGapAboveMin() const { return _fields[30]; }

  inline i16 stretchStackGapBelowMin() const { return _fields[31]; }
  // endregion

  inline i16 fractionNumeratorShiftUp() const { return _fields[32]; }

  inline i16 fractionNumeratorDisplayStyleShiftUp() const { return _fields[33]; }

  inline i16 fractionDenominatorShiftDown() const { return _fields[34]; }

  inline i16 fractionDenominatorDisplayStyleShiftDown() const { return _fields[35]; }

  inline i16 fractionNumeratorGapMin() const { return _fields[36]; }

  inline i16 fractionNumeratorDisplayStyleGapMin() const { return _fields[37]; }

  inline i16 fractionRuleThickness() const { return _fields[38]; }

  inline i16 fractionDenominatorGapMin() const { return _fields[39]; }

  inline i16 fractionDenominatorDisplayStyleGapMin() const { return _fields[40]; }

  // TODO sfrac
  inline i16 skewedFractionHorizontalGap() const { return _fields[41]; }

  inline i16 skewedFractionVerticalGap() const { return _fields[42]; }

  inline i16 overbarVerticalGap() const { return _fields[43]; }

  inline i16 overbarRuleThickness() const { return _fields[44]; }

  inline i16 overbarExtraAscender() const { return _fields[45]; }

  inline i16 underbarVerticalGap() const { return _fields[46]; }

  inline i16 underbarRuleThickness() const { return _fields[47]; }

  inline i16 underbarExtraDescender() const { return _fields[48]; }

  inline i16 radicalVerticalGap() const { return _fields[49]; }

  inline i16 radicalDisplayStyleVerticalGap() const { return _fields[50]; }

  inline i16 radicalRuleThickness() const { return _fields[51]; }

  inline i16 radicalExtraAscender() const { return _fields[52]; }

  inline i16 radicalKernBeforeDegree() const { return _fields[53]; }

  inline i16 radicalKernAfterDegree() const { return _fields[54]; }

  inline i16 radicalDegreeBottomRaisePercent() const { return _fields[55]; }

  inline i16 minConnectorOverlap() const { return _fields[56]; }

  friend CLMReader;
};

}  // namespace microtex

#endif  // MICROTEX_MATH_CONSTS_H
