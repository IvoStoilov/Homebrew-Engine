#include <graphics/precompile.h>
#include <graphics/d3d11.h>

#include <system/viewprovider/window.h>
#include <system/math/mathutil.h>

void D3D11::InitializeDeviceAndStates()
{
    InitDeviceAndContext();
    InitDepthStencilState();
    InitDepthDisabledStencilState();
    InitRasterizerState();
    InitAlphaBlendingStates();
}

void D3D11::InitializeViewPortAndMatrices(const Resolution& renderingResolution, float screenDepth, float screenNear)
{
    u32 numerator, denominator;

    InitGraphicsCardProperties(renderingResolution.w, renderingResolution.h, numerator, denominator);
    InitViewPort(renderingResolution.w, renderingResolution.h);
    InitMatrices(renderingResolution.w, renderingResolution.h, screenNear, screenDepth);
}

void D3D11::InitGraphicsCardProperties(u32 screenWidth, u32 screenHeight, u32& outNumerator, u32& outDenominator)
{
    HRESULT result;

    // Create a DirectX graphics interface factory.
    IDXGIFactory* factory = nullptr;
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    popAssert(!FAILED(result),"CreateDXGIFactory failed");

    // Use the factory to create an adapter for the primary graphics interface (video card).
    IDXGIAdapter* adapter = nullptr;
    result = factory->EnumAdapters(0, &adapter);
    popAssert(!FAILED(result), "EnumAdapters failed");

    // Enumerate the primary adapter output (monitor).
    IDXGIOutput* adapterOutput = nullptr;
    result = adapter->EnumOutputs(0, &adapterOutput);
    popAssert(!FAILED(result), "EnumOutputs failed");

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
    u32 numModes = 0;
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    popAssert(!FAILED(result), "GetDisplayModeList failed");

    // Create a list to hold all the possible display modes for this monitor/video card combination.
    DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];

    // Now fill the display mode list structures.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    popAssert(!FAILED(result), "GetDisplayModeList failed");

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    for (u32 i = 0; i < numModes; i++)
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
    DXGI_ADAPTER_DESC adapterDesc;
    result = adapter->GetDesc(&adapterDesc);
    popAssert(!FAILED(result), "adapter->GetDesc failed");

    // Store the dedicated video card memory in megabytes.
    m_VideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // Convert the name of the video card to a character array and store it.
    size_t stringLength;
    s32 error = wcstombs_s(&stringLength, m_VideoCardDescription, 128, adapterDesc.Description, 128);
    popAssert(error == 0, "wcstombs_s failed");

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
}

void D3D11::InitDeviceAndContext()
{
    constexpr IDXGIAdapter* DEFAULT_ADAPTER = nullptr;
    constexpr HMODULE NO_SOFTWARE = nullptr;
#ifdef POP_DEBUG
    constexpr u32 enableDebugFlag = D3D11_CREATE_DEVICE_DEBUG;
#elif POP_RELEASE
    constexpr u32 enableDebugFlag = 0;
#endif
    const InplaceArray<D3D_FEATURE_LEVEL, 1> FEATURE_LEVELS = { D3D_FEATURE_LEVEL_11_0 };
    const u32 FEATURE_LEVEL_SIZE = static_cast<u32>(FEATURE_LEVELS.size());
    HRESULT result = D3D11CreateDevice(DEFAULT_ADAPTER, D3D_DRIVER_TYPE_HARDWARE, NO_SOFTWARE, enableDebugFlag, 
        FEATURE_LEVELS.data(), FEATURE_LEVEL_SIZE, D3D11_SDK_VERSION, &m_Device, nullptr, &m_DeviceContext);

    popAssert(!FAILED(result), "Failed to create Device and Context");
}
void D3D11::InitDepthStencilState()
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
    popAssert(!FAILED(result), "Failed to create Depth Stencil State");

    // Set the depth stencil state.
    m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);
}

void D3D11::InitDepthDisabledStencilState()
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
    popAssert(!FAILED(result), "Failed to create Depth Stencil State");
}

void D3D11::InitRasterizerState()
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
    popAssert(!FAILED(result), "Failed CreateRasterizerState");

    m_DeviceContext->RSSetState(m_RasterState);
}

void D3D11::InitAlphaBlendingStates()
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
    popAssert(!FAILED(result), "Failed CreateBlendState");

    blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
    result = m_Device->CreateBlendState(&blendStateDescription, &m_AlphaBlendingStateDisable);
    popAssert(!FAILED(result), "Failed CreateBlendState");
}

void D3D11::InitViewPort(u32 screenWidth, u32 screenHeight)
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

void D3D11::InitMatrices(u32 screenWidth, u32 screenHeight, f32 screenNear, f32 screenDepth)
{
    float fieldOfView, screenAspect;

    // Setup the projection matrix.
    fieldOfView = MathUtil::PI / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    m_PerspectiveMatrix = mat4x4::CreatePerspectiveFieldOfView(fieldOfView, screenAspect, screenNear, screenDepth);
    m_OrthoMatrix = mat4x4::CreateOrthographic(static_cast<f32>(screenWidth), static_cast<f32>(screenHeight), screenNear, screenDepth);
}

void D3D11::Shutdown()
{
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

    if (m_DepthStencilState)
    {
        m_DepthStencilState->Release();
        m_DepthStencilState = nullptr;
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