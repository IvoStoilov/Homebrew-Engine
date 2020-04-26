#pragma once
#include <graphics/common/gfxprimitives/gfxobject.h>

class GfxVertexBuffer : public GfxObject
{
public:
    template<class T>
    void InitializeBuffer(ComPtr<ID3D11Device>& device, const Array<T>& vertexData);

    virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const override;
private:
    u32 m_ElementSize = 0u;
    ComPtr<ID3D11Buffer> m_VertexBuffer;
    D3D_PRIMITIVE_TOPOLOGY m_DrawTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

template<class T>
void GfxVertexBuffer::InitializeBuffer(ComPtr<ID3D11Device>& device, const Array<T>& vertexData)
{
    m_ElementSize = sizeof(T);
    const u32 sizeOfArray = static_cast<u32>(m_Vertices.size());

    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = m_ElementSize * sizeOfArray;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertexData.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    popGfxVerify(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer));
}

