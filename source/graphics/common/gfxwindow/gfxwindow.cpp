#include <graphics/precompile.h>
#include <graphics/common/gfxwindow/gfxwindow.h>

#include <system/viewprovider/viewprovider.h>

void GfxWindow::Initialize(ID3D11Device* device, const GfxWindowData& windowData)
{
    m_Data = windowData;
    InitWindow(windowData);
    InitSwapChain(device, windowData);
    InitRenderTargetView(device);
    InitDepthStencilView(device, windowData);
}

void GfxWindow::SetRenderTargetView(ID3D11DeviceContext* context)
{
    context->OMSetRenderTargets(1u, &m_RenderTargetView, m_DepthStencilView);

    constexpr f32 BLACK_COLOR[4] = { 0.f, 0.f, 0.f, 0.f };
    context->ClearRenderTargetView(m_RenderTargetView, BLACK_COLOR);
}

void GfxWindow::UnsetRenderTargetView(ID3D11DeviceContext* context)
{
    constexpr ID3D11RenderTargetView* NULL_RTV = nullptr;
    constexpr ID3D11DepthStencilView* NULL_DSV = nullptr;
    context->OMSetRenderTargets(1u, &NULL_RTV, NULL_DSV);
}

void GfxWindow::PresentSwapChain()
{
    m_SwapChain->Present(0, 0);
}

void GfxWindow::Shutdown()
{
    if (m_SwapChain)
    {
        m_SwapChain->SetFullscreenState(false, NULL);
    }

    if (m_RenderTargetView)
    {
        m_RenderTargetView->Release();
        m_RenderTargetView = nullptr;
    }

    if (m_DepthStencilView)
    {
        m_DepthStencilView->Release();
        m_DepthStencilView = nullptr;
    }

    if (m_DepthStencilBuffer)
    {
        m_DepthStencilBuffer->Release();
        m_DepthStencilBuffer = nullptr;
    }

    if (m_SwapChain)
    {
        m_SwapChain->Release();
        m_SwapChain = nullptr;
    }
}

void GfxWindow::InitWindow(const GfxWindowData& windowData)
{
    m_WindowCookie = g_ViewProvider.CreateChildWindow(windowData.m_Width, windowData.m_Height, windowData.m_Name, windowData.m_HasSysMenu, windowData.m_HasBorder);
}

void GfxWindow::InitSwapChain(ID3D11Device* device, const GfxWindowData& windowData)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    constexpr bool NO_STEREO = false;
    constexpr u32 NO_FLAGS = 0u;
    swapChainDesc.Width = windowData.m_Width;
    swapChainDesc.Height = windowData.m_Height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = NO_STEREO;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = NO_FLAGS;

    IDXGIFactory2* DXGIFactory = GetFactoryFromDevice(device);
    popAssert(DXGIFactory, "DXGIFactory is nullptr. Check if the device is created from such a factory");

    HWND hwnd = g_ViewProvider.GetChildWindow(m_WindowCookie);
    constexpr DXGI_SWAP_CHAIN_FULLSCREEN_DESC* NULL_FULLSCREEN_DESC = nullptr;
    constexpr IDXGIOutput* NO_RESTRICT_TO_OUTPUT = nullptr;
    HRESULT result = DXGIFactory->CreateSwapChainForHwnd(device, hwnd, &swapChainDesc, NULL_FULLSCREEN_DESC, NO_RESTRICT_TO_OUTPUT, &m_SwapChain);
    popAssert(!FAILED(result), "Could not create window: {0} with error {1:x}", windowData.m_Name, result);
}

void GfxWindow::InitRenderTargetView(ID3D11Device* device)
{
    ID3D11Texture2D* backBufferPtr = nullptr;
    HRESULT result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    popAssert(!FAILED(result), "Failed to get ptr to back buffer of window");

    if (!backBufferPtr)
        return;

    result = device->CreateRenderTargetView(backBufferPtr, NULL, &m_RenderTargetView);
    popAssert(!FAILED(result), "Failed to create window RTV with result {}", result);

    backBufferPtr->Release();
}

void GfxWindow::InitDepthStencilView(ID3D11Device* device, const GfxWindowData& windowData)
{
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    constexpr u32 NO_FLAGS = 0u;
    depthBufferDesc.Width =  windowData.m_Width;
    depthBufferDesc.Height = windowData.m_Height;
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
    popAssert(!FAILED(result), "Failed to create window Depth buffer for {}", windowData.m_Name);

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    result = device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView);
    popAssert(!FAILED(result), "Failed to create window Depth Stencil View for {}", windowData.m_Name);
}

IDXGIFactory2* GfxWindow::GetFactoryFromDevice(ID3D11Device* device)
{
    IDXGIDevice2* DXGIDevice = nullptr;
    if (device->QueryInterface(IID_PPV_ARGS(&DXGIDevice)) != S_OK)
        return nullptr;

    
    IDXGIAdapter* DXGIAdapter = nullptr;
    if (DXGIDevice->GetParent(IID_PPV_ARGS(&DXGIAdapter)) != S_OK)
        return nullptr;

    IDXGIFactory2* factory = nullptr;
    if (DXGIAdapter->GetParent(IID_PPV_ARGS(&factory)) != S_OK)
        return nullptr;
    
    return factory;
}
