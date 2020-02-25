#include <system/precompile.h>
#include <system/math/vec.h>

#include <extern/DirectXTK/Inc/SimpleMath.inl>


vec4 vec4::Cross(const vec4& b) const
{
    vec3 aVec3(x, y, z);
    vec3 bVec3(b.x, b.y, b.z);
    vec3 resultVec3 = aVec3.Cross(bVec3);
    return vec4(resultVec3.x, resultVec3.y, resultVec3.z, 0.f);
}

void vec4::Cross(const vec4& b, vec4& result) const
{   
    vec3 aVec3(x, y, z);
    vec3 bVec3(b.x, b.y, b.z);
    vec3 resultVec3 = aVec3.Cross(bVec3);
    result.x = resultVec3.x;
    result.y = resultVec3.y;
    result.z = resultVec3.z;
    result.w = 0.f;
}
