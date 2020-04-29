#include <graphics/precompile.h>
#include <graphics/renderingengine.h>
#include <graphics/d3d11.h>
#include <graphics/graphicsnode.h>
#include <graphics/common/colors.h>
#include <graphics/common/rendertexture.h>
#include <graphics/gfxinfoqueue.h>

#include <graphics/isubrenderer.h>
#include <graphics/terrain/terrainrenderer.h>
#include <graphics/debugdisplay/debugdisplayrenderer.h>
#include <graphics/skydome/skydomerenderer.h>
#include <graphics/textrendering/textrenderer.h>
#include <graphics/water/waterrenderer.h>
#include <graphics/textrendering/text.h>
#include <graphics/imgui/imgui.h>

#include <system/viewprovider/viewprovider.h>

std::string path = "../../resource/ink-splatter-texture.png";
constexpr f32 WATER_LEVEL = 3.f;


RenderingEngine::RenderingEngine()
{
}

RenderingEngine::~RenderingEngine()
{
    Shutdown();
}

bool RenderingEngine::Initialize()
{
#ifdef POP_DEBUG
    GfxInfoQueue::CreateInstance();
#endif

    m_D3D = new D3D11();
    popAssert(m_D3D != nullptr, "Memory Alloc Failed");

    m_D3D->InitializeDeviceAndStates();

    GfxWindowData gfxWindowData;
    gfxWindowData.m_WindowData.m_WindowName = "Game Window";
    gfxWindowData.m_WindowData.m_WindowResolution.w = 800u;
    gfxWindowData.m_WindowData.m_WindowResolution.h = 600u;
    gfxWindowData.m_WindowData.m_HasBorder = true;
    gfxWindowData.m_WindowData.m_HasSystemMenu = true;
    gfxWindowData.m_WindowData.m_ShowCursor = true;

    m_GameWindow.Initialize(m_D3D->GetDevice_DEPRECATED(), gfxWindowData);
    Window& window = g_ViewProvider.GetWindow(m_GameWindow.GetWindowCookie());
    constexpr f32 SCREEN_DEPTH = 1000.0f;
    constexpr f32 SCREEN_NEAR = 0.1f;
    m_D3D->InitializeViewPortAndMatrices(window.m_RenderingResolution, SCREEN_DEPTH, SCREEN_NEAR);

    m_ReflectionTexture = std::make_shared<RenderTexture>();
    m_ReflectionTexture->Initialize(m_D3D->GetDevice_DEPRECATED(), window.m_RenderingResolution.w, window.m_RenderingResolution.h);

    m_RefractionTexture = std::make_shared<RenderTexture>();
    m_RefractionTexture->Initialize(m_D3D->GetDevice_DEPRECATED(), window.m_RenderingResolution.w, window.m_RenderingResolution.h);

    m_SubRenderers.resize(SubRendererOrder::COUNT);

    ISubRenderer* skydomeRenderer = new SkydomeRenderer();
    m_SubRenderers[SubRendererOrder::Skydome] = skydomeRenderer;
    
    ISubRenderer* terrainRenderer = new TerrainRenderer();
    m_SubRenderers[SubRendererOrder::Terrain] = terrainRenderer;

    ISubRenderer* waterRenderer = new WaterRenderer(WATER_LEVEL);
    m_SubRenderers[SubRendererOrder::Water] = waterRenderer;

    ISubRenderer* textRenderer = new TextRenderer(window.m_RenderingResolution.w, window.m_RenderingResolution.h);
    m_SubRenderers[SubRendererOrder::Text] = textRenderer;

    ISubRenderer* debugDisplayRenderer = new DebugDisplayRenderer();
    m_SubRenderers[SubRendererOrder::DebugDisplay] = debugDisplayRenderer;
    debugDisplayRenderer->SetIsEnabled(false);

    ISubRenderer* imguiRenderer = new ImGuiRenderer();
    m_SubRenderers[SubRendererOrder::ImGui] = imguiRenderer;
    for (ISubRenderer* subRenderer : m_SubRenderers)
    {
        popAssert(subRenderer->Initialize(m_D3D->GetDevice_DEPRECATED(), m_D3D->GetDeviceContext_DEPRECATED()), "subRenderer failed init");
    }

    return true;
}


void RenderingEngine::Shutdown()
{
    for (GraphicsNode* node : m_Nodes)
    {
        node->Shutdown();
        delete node;
    }

    m_Nodes.clear();

    for (ISubRenderer* subRenderer : m_SubRenderers)
    {
        subRenderer->Shutdown();
        delete subRenderer;
    }

    if (m_D3D)
    {
        delete m_D3D;
        m_D3D = nullptr;
    }

#ifdef POP_DEBUG
    GfxInfoQueue::CleanInstance();
#endif
}

void RenderingEngine::RegisterDrawable(VisualComponent* visComponent)
{
    
}

void RenderingEngine::UnregisterDrawable(VisualComponent* visComponents)
{

}

bool RenderingEngine::Frame(f32 dt)
{
    popProfile(RenderingEngine::Frame);
    m_DT = dt;
    Render();
    return true;
}

void RenderingEngine::UnregisterGfxWindow(GfxWindow* gfxWindow)
{
    auto it = std::remove_if(m_ActiveWindows.begin(), m_ActiveWindows.end(), 
        [&gfxWindow](const GfxWindow* current) { return current == gfxWindow; });
}

bool RenderingEngine::PreFrame()
{
    return true;
}

bool RenderingEngine::RenderReflection()
{
    popProfile(RenderingEngine::RenderReflection);

    mat4x4 worldMatrix, reflectionViewMatrix, projectionMatrix;
    
    m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext_DEPRECATED());
    m_ReflectionTexture->BeginFrame(m_D3D->GetDeviceContext_DEPRECATED(), Colors::BLACK);

    m_D3D->GetPerspectiveMatrix(projectionMatrix);
    f32 clipPlane[4] = { 0, 1, 0, -WATER_LEVEL };
    
    for (u8 i = 0; i < SubRendererOrder::Water; i++)
    {
        if (m_SubRenderers[i]->IsEnabled())
        {
            m_SubRenderers[i]->UpdateViewMatrix(m_ViewMatrix);
            m_SubRenderers[i]->UpdateReflectionMatrix(m_ReflectedViewMatrix);
            m_SubRenderers[i]->SetClipPlane(vec4(0, 1.f, 0, -WATER_LEVEL));
            m_SubRenderers[i]->Render(m_D3D);
            m_SubRenderers[i]->SetClipPlane(vec4::Zero);
        }
    }

    m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext_DEPRECATED());
    GetGameWindow().SetRenderTargetView(m_D3D->GetDeviceContext_DEPRECATED());

    return true;
}

bool RenderingEngine::RenderRefractionTexture()
{
    popProfile(RenderingEngine::RenderRefractionTexture);

    m_RefractionTexture->SetRenderTarget(m_D3D->GetDeviceContext_DEPRECATED());
    m_RefractionTexture->BeginFrame(m_D3D->GetDeviceContext_DEPRECATED(), Colors::BLACK);

    ISubRenderer* terrainRenderer = m_SubRenderers[SubRendererOrder::Terrain];
    terrainRenderer->UpdateViewMatrix(m_ViewMatrix);
    terrainRenderer->SetClipPlane(vec4(0, -1.f, 0, WATER_LEVEL));
    terrainRenderer->Render(m_D3D);

    m_RefractionTexture->ClearRenderTarget(m_D3D->GetDeviceContext_DEPRECATED());
    GetGameWindow().SetRenderTargetView(m_D3D->GetDeviceContext_DEPRECATED());
    return true;
}

bool RenderingEngine::Render()
{
    RenderReflection();
    RenderRefractionTexture();

    for (GfxWindow* gfxWindow : m_ActiveWindows)
    {
        gfxWindow->BeginFrame(m_D3D->GetDeviceContext_DEPRECATED(), Colors::BLACK);
    }

    {
        //istoilov : minor optimization on the rendering, because we are already drawing this on the refraction texture.
        ISubRenderer* terrainRenderer = m_SubRenderers[SubRendererOrder::Terrain];
        terrainRenderer->SetClipPlane(vec4(0, 1.f, 0, -WATER_LEVEL));
    }

    for (ISubRenderer* subRenderer : m_SubRenderers)
    {
        if (subRenderer->IsEnabled())
        {
            subRenderer->UpdateViewMatrix(m_ViewMatrix);
            subRenderer->Render(m_D3D);
        }
    }

    //TODO istoilov : Move code to a dedicated solid object sub renderer.
    //mat4x4 projectionMatrix;
    //m_D3D->GetPerspectiveMatrix(projectionMatrix);
    //for (GraphicsNode* node : m_Nodes)
    //{
    //    node->SetProjectionMatrix(mat4x4(projectionMatrix));
    //    node->SetViewMatrix(viewMatrix);
    //    node->Render(m_D3D->GetDeviceContext());
    //}

    // Present the rendered scene to the screen.
    for (GfxWindow* gfxWindow : m_ActiveWindows)
    {
        gfxWindow->EndFrame();
    }

    return true;
}

void RenderingEngine::EnableDebugDisplay(bool shouldEnable)
{ 
    GetDebugDisplayRenderer()->SetIsEnabled(shouldEnable);
}

bool RenderingEngine::IsEnabledDebugDisplay() const
{
    return GetDebugDisplayRenderer()->IsEnabled();
}

DebugDisplayRenderer* RenderingEngine::GetDebugDisplayRenderer() const
{
    return static_cast<DebugDisplayRenderer*>(m_SubRenderers[SubRendererOrder::DebugDisplay]); 
}

ImGuiRenderer* RenderingEngine::GetImGuiRenderer() const
{
    return static_cast<ImGuiRenderer*>(m_SubRenderers[SubRendererOrder::ImGui]);
}
