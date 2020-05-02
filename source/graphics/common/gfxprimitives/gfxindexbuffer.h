#pragma once
#include <graphics/common/gfxprimitives/gfxbindableresource.h>

class GfxIndexBuffer : public GfxBindableResource
{
public:
    virtual ~GfxIndexBuffer() = default;

    void InitializeBuffer(ComPtr<ID3D11Device>& device, const Array<u32>& indexArray);
    virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const override;
private:
    ComPtr<ID3D11Buffer> m_IndexBuffer;
};