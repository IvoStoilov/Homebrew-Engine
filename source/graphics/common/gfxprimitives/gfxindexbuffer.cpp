#include <graphics/precompile.h>
#include <graphics/common/gfxprimitives/gfxindexbuffer.h>

void GfxIndexBuffer::InitializeBuffer(ComPtr<ID3D11Device>& device, const Array<u32>& indexArray)
{
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = static_cast<u32>(sizeof(u32) * indexArray.size());
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indexArray.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    popGfxVerify(device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer));
}

void GfxIndexBuffer::Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const
{
    deviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}
