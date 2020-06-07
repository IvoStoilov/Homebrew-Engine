#pragma once
#include <graphics/common/gfxprimitives/gfxbindableresource.h>

class GfxTexture : public GfxBindableResource
{
public:
    GfxTexture(ComPtr<ID3D11Device>& device, const String& texturePath);
    virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const override;
protected:
    ComPtr<ID3D11ShaderResourceView> m_Texture;
};