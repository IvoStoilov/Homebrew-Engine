#pragma once
#include <stdint.h>

struct D3DXVECTOR2;
class vec2
{
public:
    vec2();
    vec2(const vec2& value);
    vec2(float x, float y);

    void operator=  (const vec2& rhs);
    void operator+= (const vec2& rhs);
    void operator-= (const vec2& rhs);
    void operator*= (const float rhs);
    void operator/= (const float rhs);
    
    vec2 operator+ (const vec2& rhs) const;
    vec2 operator- (const vec2& rhs) const;
    vec2 operator* (const float rhs) const;
    vec2 operator/ (const float rhs) const;

    float& operator[](const uint32_t i);
    float  operator[](const uint32_t i) const;

    float GetLength()    const;
    float GetLengthSqr() const;

    void Normalize();
    vec2 GetNormalized() const;

    D3DXVECTOR2 ToD3DXVECTOR2() const;

    static float Dot(const vec2& a, const vec2& b);

    union
    {
        float _data[2];
        struct { float x, y; };
        struct { float u, v; };
        struct { float r, g; };
    };

    static const vec2 Zero;
    static const vec2 BaseI;
    static const vec2 BaseJ;
};