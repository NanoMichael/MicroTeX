#ifndef NUMS_H_INCLUDED
#define NUMS_H_INCLUDED

#include <cmath>
#include <limits>

namespace tex {

/** Positive infinity */
static const float POS_INF = std::numeric_limits<float>::infinity();
/** Negative infinity */
static const float NEG_INF = -POS_INF;
/** Max float value */
static const float F_MAX = std::numeric_limits<float>::max();
/** Min float value */
static const float F_MIN = -F_MAX;
/** Pi */
static const double PI = atan(1.0) * 4;
/** Precision, for compare with 0.0f, if a value < PREC, we trade it as 0.0f */
static const float PREC = 0.0000001f;

}  // namespace tex

#endif
