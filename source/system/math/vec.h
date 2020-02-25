#pragma once
#include <d3d11.h>
#include <extern/DirectXTK/Inc/SimpleMath.h>

using vec2 = DirectX::SimpleMath::Vector2;
using vec3 = DirectX::SimpleMath::Vector3;

struct vec4 : public DirectX::SimpleMath::Vector4
{
    using super = DirectX::SimpleMath::Vector4;

    vec4() noexcept                                  : super() {}
    constexpr explicit vec4(f32 ix)                  : super(ix) {}
    constexpr vec4(f32 ix, f32 iy, f32 iz, f32 iw)   : super(ix, iy, iz, iw) {}
    explicit vec4(_In_reads_(4) const float* pArray) : super(pArray) {}
    vec4(DirectX::FXMVECTOR V)                       : super(V) {}
    vec4(const DirectX::XMFLOAT4& V)                 : super(V) {}
    explicit vec4(const DirectX::XMVECTORF32& F)     : super(F) {}

    vec4(const vec4&) = default;
    vec4& operator=(const vec4&) = default;

    vec4(vec4&&) = default;
    vec4& operator=(vec4&&) = default;

    vec4 Cross(const vec4& b) const;
    void Cross(const vec4& b, vec4& result) const;
};

using quat = DirectX::SimpleMath::Quaternion;
