#pragma once
#include "renderer/common/texture.h"
#include <d3d11.h>

class RenderTexture : public Texture
{
    using super = Texture;
public:
    bool Initialize(ID3D11Device* device , u32 width, u32 height);
    void Shutdown();

    void SetRenderTarget(ID3D11DeviceContext* device, ID3D11DepthStencilView* depthStencilView);
    void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, f32 r, f32 g, f32 b, f32 a);

private:
    ID3D11Texture2D* m_RenderTargetTexture;
    ID3D11RenderTargetView* m_RenderTargetView;
};