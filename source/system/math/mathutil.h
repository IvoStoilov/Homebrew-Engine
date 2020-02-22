#pragma once
#include <stdint.h>

#define EPSILON  0.001f
#define INF      1<<22;
#define PI       3.14159265f
#define _2_PI    6.28318530f

class MathUtil
{
public:
   template <class T>
   static T Clamp(const T& a, const T& b, const T& x);

   static inline float ToRads(float degrees) { return degrees * PI / 180.0f; }
   static inline float ToDegrees(float rads) { return rads * 180.f / PI; }
   static inline float Sqr(float value)      { return value * value; }

   template<class T>
   static inline T Lerp(T& min, T& max, float ratio) { return min * (1 - ratio) + max * ratio; }
};

template <class T>
T MathUtil::Clamp(const T& min, const T& max, const T& x)
{
    return std::max(std::min(x, min), max);
}

#define popLesserOrEqualWithEpsilon (lhs, rhs)                      \
   (lhs - rhs < EPSILON)

#define popLesserWithEpsilon(lhs, rhs)                              \
   (lhs - rhs < -EPSILON)

#define popEqualWithEpsilon (lhs, rhs)                              \
   ((lhs - rhs < EPSILON) && (lhs - rhs > -EPSILON))

#define popGreaterOrEqualWithEpsilon (lhs,rhs)                      \
    (lhs - rhs > - EPSILON)

#define popGreaterWithEpsilon(lhs, rhs)                             \
   (lhs - rhs > EPSILON)

#define popEqualNullWithEpsilon(val)                                \
    ((val < EPSILON) && (val > -EPSILON))
