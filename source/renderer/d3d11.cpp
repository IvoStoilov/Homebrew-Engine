#include "renderer/d3d11.h"

D3D11::D3D11() :
    m_SwapChain(nullptr),
    m_Device(nullptr),
    m_DeviceContext(nullptr),
    m_RenderTargetView(nullptr),
    m_DepthStencilBuffer(nullptr),
    m_DepthStencilState(nullptr),
    m_DepthDisabledStencilState(nullptr),
    m_DepthStencilView(nullptr),
    m_RasterState(nullptr),
    m_AlphaBlendingStateEnable(nullptr),
    m_AlphaBlendingStateDisable(nullptr)
{
}

D3D11::~D3D11()
{
}

bool D3D11::Initialize(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd, bool vsync, bool fullscreen, float screenDepth, float screenNear)
{
    uint32_t numerator, denominator;
    
    m_vsync_enabled = vsync;

    if (!InitGraphicsCardProperties(screenWidth, screenHeight, numerator, denominator))
        return false;

    if (!InitDeviceAndSwapchain(screenWidth, screenHeight, hwnd, fullscreen, numerator, denominator))
        return false;

    if (!InitDepthStencilView(screenWidth, screenHeight))
        return false;
   
    if (!InitDepthStencilState())
        return false;

    if (!InitDepthDisabledStencilState())
        return false;

    if (!InitRasterizerState())
        return false;

    if (!InitAlphaBlendingStates())
        return false;

    InitViewPort(screenWidth, screenHeight);

    InitMatrices(screenWidth, screenHeight, screenNear, screenDepth);

    return true;
}

bool D3D11::InitGraphicsCardProperties(uint32_t screenWidth, uint32_t screenHeight, uint32_t& outNumerator, uint32_t& outDenominator)
{
    HRESULT result;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;

    DXGI_MODE_DESC* displayModeList;

    DXGI_ADAPTER_DESC adapterDesc;

    uint32_t numModes = 0;
    int error;

    // Create a DirectX graphics interface factory.
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result))
    {
        return false;
    }

    // Use the factory to create an adapter for the primary graphics interface (video card).
    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result))
    {
        return false;
    }

    // Enumerate the primary adapter output (monitor).
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result))
    {
        return false;
    }

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result))
    {
        return false;
    }

    // Create a list to hold all the possible display modes for this monitor/video card combination.
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList)
    {
        return false;
    }

    // Now fill the display mode list structures.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result))
    {
        return false;
    }

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    for (uint32_t i = 0; i < numModes; i++)
    {
        if (displayModeList[i].Width == (unsigned int)screenWidth)
        {
            if (displayModeList[i].Height == (unsigned int)screenHeight)
            {
                outNumerator = displayModeList[i].RefreshRate.Numerator;
                outDenominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }
    // Get the adapter (video card) description.
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result))
    {
        return false;
    }

    // Store the dedicated video card memory in megabytes.
    m_VideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // Convert the name of the video card to a character array and store it.
    size_t stringLength;
    error = wcstombs_s(&stringLength, m_VideoCardDescription, 128, adapterDesc.Description, 128);
    if (error != 0)
    {
        return false;
    }

    // Release the display mode list.
    delete[] displayModeList;
    displayModeList = nullptr;

    // Release the adapter output.
    adapterOutput->Release();
    adapterOutput = nullptr;

    // Release the adapter.
    adapter->Release();
    adapter = nullptr;

    // Release the factory.
    factory->Release();
    factory = nullptr;

    return true;
}

bool D3D11::InitDeviceAndSwapchain(uint32_t screenWidth, uint32_t screenHeight, HWND hwnd, bool fullscreen, uint32_t numerator, uint32_t denominator)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;

    // Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

    // Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Set the refresh rate of the back buffer.
    if (m_vsync_enabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd;

    // Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Set to full screen or windowed mode.
    if (fullscreen)
    {
        swapChainDesc.Windowed = false;
    }
    else
    {
        swapChainDesc.Windowed = true;
    }

    // Set the scan line ordering and scaling to unspecified.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard the back buffer contents after presenting.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // Don't set the advanced flags.
    swapChainDesc.Flags = 0;

    // Set the feature level to DirectX 11.
    featureLevel = D3D_FEATURE_LEVEL_11_0;

    // Create the swap chain, Direct3D device, and Direct3D device context.
#ifdef POP_DEBUG
    constexpr uint32_t enableDebugFlag = D3D11_CREATE_DEVICE_DEBUG;
#elif POP_RELEASE
    constexpr uint32_t enableDebugFlag = 0;
#endif
    HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, enableDebugFlag, &featureLevel, 1,
        D3D11_SDK_VERSION, &swapChainDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool D3D11::InitDepthStencilView(uint32_t screenWidth, uint32_t screenHeight)
{
    HRESULT result;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

    // Get the pointer to the back buffer.
    result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result))
    {
        return false;
    }

    // Create the render target view with the back buffer pointer.
    result = m_Device->CreateRenderTargetView(backBufferPtr, NULL, &m_RenderTargetView);
    if (FAILED(result))
    {
        return false;
    }

    // Release pointer to the back buffer as we no longer need it.
    backBufferPtr->Release();
    backBufferPtr = 0;

    // Initialize the description of the depth buffer.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    result = m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_DepthStencilBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Initailze the depth stencil view.
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    result = m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView);
    if (FAILED(result))
    {
        return false;
    }

    m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

    return true;
}

bool D3D11::InitDepthStencilState()
{
    HRESULT result;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

    // Initialize the description of the stencil state.
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = false;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state.
    result = m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);
    if (FAILED(result))
    {
        return false;
    }

    // Set the depth stencil state.
    m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

    return true;
}

bool D3D11::InitDepthDisabledStencilState()
{
    HRESULT result;
    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

    // Clear the second depth stencil state before setting the parameters.
    ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

    // Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
    // that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
    depthDisabledStencilDesc.DepthEnable = false;
    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDisabledStencilDesc.StencilEnable = true;
    depthDisabledStencilDesc.StencilReadMask = 0xFF;
    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the state using the device.
    result = m_Device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_DepthDisabledStencilState);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool D3D11::InitRasterizerState()
{
    HRESULT result;
    D3D11_RASTERIZER_DESC rasterDesc;

    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state from the description we just filled out.
    result = m_Device->CreateRasterizerState(&rasterDesc, &m_RasterState);
    if (FAILED(result))
    {
        return false;
    }

    // Now set the rasterizer state.
    m_DeviceContext->RSSetState(m_RasterState);
    return true;
}

bool D3D11::InitAlphaBlendingStates()
{
    D3D11_BLEND_DESC blendStateDescription;
    HRESULT result;
    ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

    blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
    blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    result = m_Device->CreateBlendState(&blendStateDescription, &m_AlphaBlendingStateEnable);
    if (FAILED(result))
    {
        return false;
    }

    blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
    result = m_Device->CreateBlendState(&blendStateDescription, &m_AlphaBlendingStateDisable);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void D3D11::InitViewPort(uint32_t screenWidth, uint32_t screenHeight)
{
    D3D11_VIEWPORT viewport;

    // Setup the viewport for rendering.
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    m_DeviceContext->RSSetViewports(1, &viewport);
}

void D3D11::InitMatrices(uint32_t screenWidth, uint32_t screenHeight, float screenNear, float screenDepth)
{
    float fieldOfView, screenAspect;

    // Setup the projection matrix.
    fieldOfView = (float)D3DX_PI / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    // Create the projection matrix for 3D rendering.
    D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

    // Initialize the world matrix to the identity matrix.
    D3DXMatrixIdentity(&m_WorldMatrix);

    // Create an orthographic projection matrix for 2D rendering.
    D3DXMatrixOrthoLH(&m_OrthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);
}

void D3D11::Shutdown()
{
    // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
    if (m_SwapChain)
    {
        m_SwapChain->SetFullscreenState(false, NULL);
    }

    if (m_AlphaBlendingStateEnable)
    {
        m_AlphaBlendingStateEnable->Release();
        m_AlphaBlendingStateEnable = nullptr;
    }

    if (m_AlphaBlendingStateDisable)
    {
        m_AlphaBlendingStateDisable->Release();
        m_AlphaBlendingStateDisable = nullptr;
    }

    if (m_DepthDisabledStencilState)
    {
        m_DepthDisabledStencilState->Release();
        m_DepthDisabledStencilState = nullptr;
    }

    if (m_RasterState)
    {
        m_RasterState->Release();
        m_RasterState = nullptr;
    }

    if (m_DepthStencilView)
    {
        m_DepthStencilView->Release();
        m_DepthStencilView = nullptr;
    }

    if (m_DepthStencilState)
    {
        m_DepthStencilState->Release();
        m_DepthStencilState = nullptr;
    }

    if (m_DepthStencilBuffer)
    {
        m_DepthStencilBuffer->Release();
        m_DepthStencilBuffer = nullptr;
    }

    if (m_RenderTargetView)
    {
        m_RenderTargetView->Release();
        m_RenderTargetView = nullptr;
    }

    if (m_DeviceContext)
    {
        m_DeviceContext->Release();
        m_DeviceContext = nullptr;
    }

    if (m_Device)
    {
        m_Device->Release();
        m_Device = nullptr;
    }

    if (m_SwapChain)
    {
        m_SwapChain->Release();
        m_SwapChain = nullptr;
    }

    return;
}

void D3D11::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];

    // Setup the color to clear the buffer to.
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // Clear the back buffer.
    m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, color);

    // Clear the depth buffer.
    m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D11::EndScene()
{
    // Present the back buffer to the screen since rendering is complete.
    if (m_vsync_enabled)
    {
        // Lock to screen refresh rate.
        m_SwapChain->Present(1, 0);
    }
    else
    {
        // Present as fast as possible.
        m_SwapChain->Present(0, 0);
    }
}

void D3D11::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, m_VideoCardDescription);
    memory = m_VideoCardMemory;
    return;
}

void D3D11::TurnDepthTestOn()
{
    m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);
    return;
}

void D3D11::TurnDepthTestOff()
{
    m_DeviceContext->OMSetDepthStencilState(m_DepthDisabledStencilState, 1);
    return;
}

void D3D11::TurnAlphaBlendingOn()
{
    float blendFactor[4];

    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    // Turn on the alpha blending.
    m_DeviceContext->OMSetBlendState(m_AlphaBlendingStateEnable, blendFactor, 0xffffffff);
}

void D3D11::TurnAlphaBlendingOff()
{
    float blendFactor[4];

    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    // Turn off the alpha blending.
    m_DeviceContext->OMSetBlendState(m_AlphaBlendingStateDisable, blendFactor, 0xffffffff);
}
