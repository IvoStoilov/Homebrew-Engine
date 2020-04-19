#pragma once
#include "graphics/common/texture.h"
#include <d3d11.h>

class RenderTexture : public Texture
{
    using super = Texture;
public:
    bool Initialize(ID3D11Device* device, u32 width, u32 height);
    void Shutdown();

    void SetRenderTarget(ID3D11DeviceContext* device);
    void BeginFrame(ID3D11DeviceContext* deviceContext, const vec4& clearColor);
    void ClearRenderTarget(ID3D11DeviceContext* deviceContext);

private:
    void InitDepthStencilView(ID3D11Device* device, u32 width, u32 height);
private:
    //https://stackoverflow.com/questions/49836725/directx-without-release
    ID3D11Texture2D* m_RenderTargetTexture;
    ID3D11Texture2D* m_DepthStencilBuffer;
    ID3D11RenderTargetView* m_RenderTargetView;
    ID3D11DepthStencilView* m_DepthStencilView;
};