#include "precompile.h"
#include "renderer/common/rendertexture.h"

#include "system/error.h"

bool RenderTexture::Initialize(ID3D11Device* device, u32 textureWidth, u32 textureHeight)
{
    HRESULT result;

    D3D11_TEXTURE2D_DESC textureDesc;
    memset(&textureDesc, 0, sizeof(textureDesc));
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    result = device->CreateTexture2D(&textureDesc, NULL, &m_RenderTargetTexture);
    popAssert(!FAILED(result), "Failed To create Render Texture");
    
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    result = device->CreateRenderTargetView(m_RenderTargetTexture, &renderTargetViewDesc, &m_RenderTargetView);
    popAssert(!FAILED(result), "Failed To create Render Target view");
    
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    result = device->CreateShaderResourceView(m_RenderTargetTexture, &shaderResourceViewDesc, &m_Texture);
    popAssert(!FAILED(result), "Failed To create Shader resource view");

    return true;
}

void RenderTexture::Shutdown()
{
    super::Shutdown();

    if (m_RenderTargetTexture)
    {
        m_RenderTargetTexture->Release();
        m_RenderTargetTexture = nullptr;
    }

    if (m_RenderTargetView)
    {
        m_RenderTargetView->Release();
        m_RenderTargetView = nullptr;
    }
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
    deviceContext->OMSetRenderTargets(1, &m_RenderTargetView, depthStencilView);

    return;
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView,
    f32 red, f32 green, f32 blue, f32 alpha)
{
    float color[4] = {red, green, blue, alpha};

    deviceContext->ClearRenderTargetView(m_RenderTargetView, color);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}