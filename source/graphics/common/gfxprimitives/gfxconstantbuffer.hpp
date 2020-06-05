#pragma once

template <class T>
GfxConstantBuffer<T>::GfxConstantBuffer(ComPtr<ID3D11Device>& device, const T& data, u32 slot /*= 0u*/)
    : m_Slot(slot)
{
    constexpr u32 NO_FLAGS = 0u;
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(data);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = NO_FLAGS;
    bufferDesc.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pSysMem = &data;
    popGfxVerify(device->CreateBuffer(&bufferDesc, subResourceData, &m_ConstantBuffer));
}

template <class T>
GfxConstantBuffer<T>::GfxConstantBuffer(ComPtr<ID3D11Device>& device, u32 slot /*= 0*/)
    : m_Slot(slot)
{
    constexpr u32 NO_FLAGS = 0u;
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(T);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = NO_FLAGS;
    bufferDesc.StructureByteStride = 0u;

    constexpr D3D11_SUBRESOURCE_DATA* NO_DATA = nullptr;
    popGfxVerify(device->CreateBuffer(&bufferDesc, NO_DATA, &m_ConstantBuffer));
}

template <class T>
void GfxConstantBuffer<T>::Update(ComPtr<ID3D11DeviceContext>& deviceContext, const T& data)
{
    constexpr u32 NO_FLAGS = 0u;

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    popGfxVerify(deviceContext->Map(m_ConstantBuffer > Get(), 0u, D3D11_MAP_WRITE_DISCARD, NO_FLAGS, &mappedResource));
    memcpy(mappedResource.pData, &data, sizeof(data));
    deviceContext->Unmap(m_ConstantBuffer.Get(), 0u);
}

template <class T>
void GfxVertexConstantBuffer<T>::Bind(ComPtr<ID3D11DeviceContext>& deviceContext)
{
    popGfxVerify(deviceContext->VSSetConstantBuffers(m_Slot, 1u, m_ConstantBuffer.GetAddressOf()));
}

template <class T>
void GfxPixelConstantBuffer<T>::Bind(ComPtr<ID3D11DeviceContext>& deviceContext)
{
    popGfxVerify(deviceContext->PSSetConstantBuffers(m_Slot, 1u, m_ConstantBuffer.GetAddressOf()));
}