#pragma once
#include <d3d11.h>
#include <extern/DirectXTK/Inc/SimpleMath.h>

struct vec2 : public DirectX::SimpleMath::Vector2
{
    using super = DirectX::SimpleMath::Vector2;

    vec2() noexcept                                : super() {}
    constexpr explicit vec2(f32 ix)                : super(ix) {}
    constexpr vec2(f32 ix, f32 iy)                 : super(ix, iy) {}
    explicit vec2(_In_reads_(2) const f32* pArray) : super(pArray) {}
    explicit vec2(const DirectX::XMVECTORF32& F)   : super(F) {}
    vec2(DirectX::XMVECTOR V)                      : super(V) {}
    vec2(const DirectX::XMFLOAT2& V)               : super(V) {}

    vec2(const vec2&) = default;
    vec2& operator=(const vec2&) = default;

    vec2(vec2&&) = default;
    vec2& operator=(vec2&&) = default;
};

struct vec3 : public DirectX::SimpleMath::Vector3
{
    using super = DirectX::SimpleMath::Vector3;

    vec3() noexcept                                  : super() {}
    constexpr explicit vec3(f32 ix)                  : super(ix) {}
    constexpr vec3(f32 ix, f32 iy, f32 iz)           : super(ix, iy, iz) {}
    explicit vec3(_In_reads_(3) const f32* pArray)   : super(pArray) {}
    explicit vec3(const DirectX::XMVECTORF32& F)     : super(F) {}
    vec3(DirectX::FXMVECTOR V)                       : super(V) {}
    vec3(const DirectX::XMFLOAT3& V)                 : super(V) {}

    vec3(const vec3&) = default;
    vec3& operator=(const vec3&) = default;

    vec3(vec3&&) = default;
    vec3& operator=(vec3&&) = default;
};

struct vec4 : public DirectX::SimpleMath::Vector4
{
    using super = DirectX::SimpleMath::Vector4;

    vec4() noexcept                                  : super() {}
    constexpr explicit vec4(f32 ix)                  : super(ix) {}
    constexpr vec4(f32 ix, f32 iy, f32 iz, f32 iw)   : super(ix, iy, iz, iw) {}
    explicit vec4(_In_reads_(4) const float* pArray) : super(pArray) {}
    explicit vec4(const DirectX::XMVECTORF32& F)     : super(F) {}
    vec4(DirectX::FXMVECTOR V)                       : super(V) {}
    vec4(const DirectX::XMFLOAT4& V)                 : super(V) {}
    
    vec4(const vec4&) = default;
    vec4& operator=(const vec4&) = default;

    vec4(vec4&&) = default;
    vec4& operator=(vec4&&) = default;

    vec4 Cross(const vec4& b) const;
    void Cross(const vec4& b, vec4& result) const;

    const f32* GetBuffer() const { return &x; }
};

using quat = DirectX::SimpleMath::Quaternion;
