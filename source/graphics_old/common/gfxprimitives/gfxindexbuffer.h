#pragma once
#include <graphics/common/gfxprimitives/gfxbindableresource.h>

class GfxIndexBuffer : public GfxBindableResource
{
public:
    virtual ~GfxIndexBuffer() = default;
    virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const override;

    u32 GetSize() const { return m_Size; }
    void InitializeBuffer(ComPtr<ID3D11Device>& device, const Array<u32>& indexArray);
private:
    u32 m_Size = 0;
    ComPtr<ID3D11Buffer> m_IndexBuffer;
};