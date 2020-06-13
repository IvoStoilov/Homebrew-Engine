#pragma once
#include <graphics/common/gfxprimitives/gfxbindableresource.h>

class GfxTexture : public GfxBindableResource
{
public:
    enum class ShaderTarget
    {
        Vertex,
        Pixel
    };
    GfxTexture(ComPtr<ID3D11Device>& device, const String& texturePath, ShaderTarget shaderTarget);
    virtual void Bind(ComPtr<ID3D11DeviceContext>& deviceContext) const override;
protected:
    ShaderTarget m_ShaderTarget = ShaderTarget::Vertex;
    ComPtr<ID3D11ShaderResourceView> m_Texture;
};