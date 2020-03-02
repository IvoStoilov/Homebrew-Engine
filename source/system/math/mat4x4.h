#pragma once
#include <d3d11.h>
#include <extern/DirectXTK/Inc/SimpleMath.h>
#include <system/math/vec.h>

struct mat4x4 : public DirectX::SimpleMath::Matrix
{
    using super = DirectX::SimpleMath::Matrix;

    mat4x4() : super() {}
    constexpr mat4x4(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
        : super(m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33) 
    {}

    explicit mat4x4(const vec3& r0, const vec3& r1, const vec3& r2)
        : super(r0.x, r0.y, r0.z, 0,
                r1.x, r1.y, r1.z, 0,
                r2.x, r2.y, r2.z, 0,
                0, 0, 0, 1.f) 
    {}

    explicit mat4x4(const vec4& r0, const vec4& r1, const vec4& r2, const vec4& r3)
        : super(r0.x, r0.y, r0.z, r0.w,
                r1.x, r1.y, r1.z, r1.w,
                r2.x, r2.y, r2.z, r2.w,
                r3.x, r3.y, r3.z, r3.w) 
    {}

    mat4x4(const DirectX::XMFLOAT4X4& M) : super(M) {}
    mat4x4(const DirectX::XMFLOAT3X3& M) : super(M) {}
    mat4x4(const DirectX::XMFLOAT4X3& M) : super(M) {}

    explicit mat4x4(_In_reads_(16) const float* pArray) : super(pArray) {}
    mat4x4(DirectX::CXMMATRIX M) : super(M) {}

    mat4x4(const mat4x4&) = default;
    mat4x4& operator=(const mat4x4&) = default;

    mat4x4(mat4x4&&) = default;
    mat4x4& operator=(mat4x4&&) = default;

    inline void SetTranslation(const vec4& v) { _41 = v.x; _42 = v.y; _43 = v.z; };
    inline void SetTranslation(const vec3& v) { _41 = v.x; _42 = v.y; _43 = v.z; };
    inline vec4 GetTranslation() const { return vec4(_41, _42, _43, 1.f); } ;

    void Transpose();
    inline void GetTransposed(mat4x4& result) const { super::Transpose(result); }
    inline mat4x4 GetTransposed() const { return super::Transpose(); }

    void Invert();
    inline void GetInverted(mat4x4& result) const { super::Invert(result); }
    inline mat4x4 GetInverted() const { return super::Invert(); }
};