#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxvertexbuffer.h>

void GfxVertexBuffer::Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const
{
    const u32 offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_ElementSize, &offset);
}
