#pragma once
#include <graphics/common/texture.h>

class RenderTexture : public GfxTexture
{
    using super = GfxTexture;
public:
    bool Initialize(ID3D11Device* device, u32 width, u32 height);
    
    void SetRenderTarget(ID3D11DeviceContext* device);
    void BeginFrame(ID3D11DeviceContext* deviceContext, const vec4& clearColor);
    void ClearRenderTarget(ID3D11DeviceContext* deviceContext);

private:
    void InitDepthStencilView(ID3D11Device* device, u32 width, u32 height);
private:
    ComPtr<ID3D11Texture2D> m_RenderTargetTexture;
    ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
    ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
    ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
};