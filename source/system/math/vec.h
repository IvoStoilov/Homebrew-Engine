#pragma once
/*
#include <directxmath.h>

template <class VecType>
class VecBase
{
public:
    VecBase();
    VecBase(const VecBase<VecType>& other);

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
protected:
    VecType m_Vector;
};

using Vec4f = VecBase<DirectX::XMFLOAT4>;
using Vec3f = VecBase<DirectX::XMFLOAT3>;
using Vec2f = VecBase<DirectX::XMFLOAT2>;
*/