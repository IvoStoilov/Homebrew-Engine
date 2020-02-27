#pragma once

class MathUtil
{
public:
    static constexpr f32 EPSILON = 0.001f;
    static constexpr f32 PI      = 3.14159265f;
    static constexpr f32 _2_PI   = 6.28318530f;
    static constexpr u32 INF     = 1 << 22;

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
   (lhs - rhs < MathUtil::EPSILON)

#define popLesserWithEpsilon(lhs, rhs)                              \
   (lhs - rhs < -MathUtil::EPSILON)

#define popEqualWithEpsilon (lhs, rhs)                              \
   ((lhs - rhs < MathUtil::EPSILON) && (lhs - rhs > -MathUtil::EPSILON))

#define popGreaterOrEqualWithEpsilon (lhs,rhs)                      \
    (lhs - rhs > - MathUtil::EPSILON)

#define popGreaterWithEpsilon(lhs, rhs)                             \
   (lhs - rhs > MathUtil::EPSILON)

#define popEqualNullWithEpsilon(val)                                \
    ((val < MathUtil::EPSILON) && (val > -MathUtil::EPSILON))
