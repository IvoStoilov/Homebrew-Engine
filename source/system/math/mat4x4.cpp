#include <system/precompile.h>
#include <system/math/mat4x4.h>

#include <extern/DirectXTK/Inc/SimpleMath.inl>

void mat4x4::Transpose()
{
    using namespace DirectX;
    XMMATRIX M = XMLoadFloat4x4(this);
    XMStoreFloat4x4(this, XMMatrixTranspose(M));
}

void mat4x4::Invert()
{
    using namespace DirectX;
    XMMATRIX M = XMLoadFloat4x4(this);
    XMVECTOR det;
    XMStoreFloat4x4(this, XMMatrixInverse(&det, M));
}
