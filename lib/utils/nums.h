#ifndef NUMS_H_INCLUDED
#define NUMS_H_INCLUDED

#include <cmath>
#include <limits>

namespace microtex {

/** Positive infinity */
static constexpr float POS_INF = std::numeric_limits<float>::infinity();
/** Negative infinity */
static constexpr float NEG_INF = -POS_INF;
/** Max float value */
static constexpr float F_MAX = std::numeric_limits<float>::max();
/** Min float value */
static constexpr float F_MIN = -F_MAX;
/** Pi */
static constexpr double PI = 3.1415926535;
/** Precision, for compare with 0.0f, if a value < PREC, we trade it as 0.0f */
static constexpr float PREC = 0.01;

}  // namespace microtex

#endif
