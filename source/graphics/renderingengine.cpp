#include <graphics/precompile.h>
#include <graphics/renderingengine.h>
#include <graphics/common/gfxwindow/gfxwindow.h>
#include <graphics/common/colors.h>
#include <graphics/d3d11.h>

#include <graphics/common/mesh.h>
#include <system/viewprovider/viewprovider.h>

RenderingEngine::RenderingEngine()
{
    Initialize();
}

RenderingEngine::~RenderingEngine()
{
    Shutdown();
}

void RenderingEngine::Initialize()
{
#ifdef POP_DEBUG
    GfxInfoQueue::CreateInstance();
#endif

    m_D3D = std::make_unique<D3D11>();
    m_D3D->InitializeDeviceAndStates();

    GfxWindowData gfxWindowData;
    gfxWindowData.m_WindowData.m_WindowName = "Game Window";
    gfxWindowData.m_WindowData.m_WindowResolution.w = 800u;
    gfxWindowData.m_WindowData.m_WindowResolution.h = 600u;
    gfxWindowData.m_WindowData.m_PosX = 1920 / 2 - 400;
    gfxWindowData.m_WindowData.m_PosY = 1080 / 2 - 300;
    gfxWindowData.m_WindowData.m_HasBorder = true;
    gfxWindowData.m_WindowData.m_HasSystemMenu = true;
    gfxWindowData.m_WindowData.m_ShowCursor = true;

    m_GameWindow = std::make_unique<GfxWindow>();
    m_GameWindow->Initialize(m_D3D->GetDevice(), gfxWindowData);
    Window& window = g_ViewProvider.GetWindow(m_GameWindow->GetWindowCookie());
    constexpr f32 SCREEN_DEPTH = 1000.0f;
    constexpr f32 SCREEN_NEAR = 0.1f;
    m_D3D->InitializeViewPortAndMatrices(window.m_RenderingResolution, SCREEN_DEPTH, SCREEN_NEAR);

    {
        Mesh mesh("../../resource/stone/Stone.obj");
    }

}


void RenderingEngine::Update(f32 dt)
{
    popProfile(RenderingEngine::Update);

    m_GameWindow->BeginFrame(m_D3D->GetDeviceContext(), Colors::BLACK);
    m_GameWindow->EndFrame();
}

const WindowCookie& RenderingEngine::GetGameWindowCookie() const
{
    return m_GameWindow->GetWindowCookie();
}

void RenderingEngine::Shutdown()
{
#ifdef POP_DEBUG
    GfxInfoQueue::CleanInstance();
#endif
}
