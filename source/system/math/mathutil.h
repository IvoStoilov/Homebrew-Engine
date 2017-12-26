#pragma once
#include <stdint.h>

#define EPSILON  0.001f
#define INF      1<<22;
#define PI       3.14159265f
#define _2_PI    6.28318530f

class MathUtil
{
public:
   static uint32_t Clamp(const uint32_t min, const uint32_t max, uint32_t x);
   static float Clamp(const float min, const float max, const float x);
   static inline float ToRads(float degrees) { return degrees * PI / 180.0f; }
   static inline float ToDegrees(float rads) { return rads * 180.f / PI; }
   static inline float Sqr(float value)      { return value * value; }

   template<class T>
   static inline T Lerp(T& min, T& max, float ratio) { return min * (1 - ratio) + max * ratio; }
};

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
