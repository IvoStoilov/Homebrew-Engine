#include <graphics/precompile.h>
#include <graphics/common/rendertexture.h>

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

    result = device->CreateRenderTargetView(m_RenderTargetTexture.Get(), &renderTargetViewDesc, &m_RenderTargetView);
    popAssert(!FAILED(result), "Failed To create Render Target view");
    
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    result = device->CreateShaderResourceView(m_RenderTargetTexture.Get(), &shaderResourceViewDesc, &m_Texture);
    popAssert(!FAILED(result), "Failed To create Shader resource view");

    InitDepthStencilView(device, textureWidth, textureHeight);
    return true;
}

void RenderTexture::InitDepthStencilView(ID3D11Device* device, u32 width, u32 height)
{
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    constexpr u32 NO_FLAGS = 0u;
    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = NO_FLAGS;
    depthBufferDesc.MiscFlags = NO_FLAGS;

    HRESULT result = device->CreateTexture2D(&depthBufferDesc, NULL, &m_DepthStencilBuffer);
    popAssert(!FAILED(result), "Failed to create window Depth buffer for Render Texture");

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    result = device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &depthStencilViewDesc, &m_DepthStencilView);
    popAssert(!FAILED(result), "Failed to create window Depth Stencil View for Render Texture");
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext)
{
    deviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());
}

void RenderTexture::BeginFrame(ID3D11DeviceContext* deviceContext, const vec4& clearColor)
{
    f32 color[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };

    deviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), color);
    deviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext)
{
    constexpr ID3D11RenderTargetView* NULL_RTV = nullptr;
    constexpr ID3D11DepthStencilView* NULL_DSV = nullptr;
    deviceContext->OMSetRenderTargets(1, &NULL_RTV, NULL_DSV);
}