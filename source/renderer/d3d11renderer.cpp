#include "precompile.h"
//TODO istoilov: remove the include of #include <d3dx10math.h> in d3d11renderer
#include "renderer/d3d11.h"
#include "renderer/d3d11renderer.h"

#include "renderer/graphicsnode.h"

#include "renderer/isubrenderer.h"
#include "renderer/terrain/terrainrenderer.h"
#include "renderer/debugdisplay/debugdisplayrenderer.h"
#include "renderer/skydome/skydomerenderer.h"
#include "renderer/textrendering/textrenderer.h"
#include "renderer/water/waterrenderer.h"

#include "renderer/textrendering/text.h"

#include "entitymodel/entity.h"
#include "entitymodel/components/visualcomponent.h"

#include "core/camera.h"
#include "core/engine.h"

#include "system/error.h"

#include <windows.h>

std::string path = "../../resource/ink-splatter-texture.png";
constexpr f32 WATER_LEVEL = 3.f;

D3D11Renderer* D3D11Renderer::s_Instance = nullptr;

bool D3D11Renderer::Initialize(HWND hwnd, u32 screenWidth, u32 screenHeight)
{
    m_D3D = new D3D11();
    popAssert(m_D3D != nullptr, "Memory Alloc Failed");
    popAssert(m_D3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR), "D3D Init Failed");

    m_ReflectionTexture = std::make_shared<RenderTexture>();
    m_ReflectionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);

    m_SubRenderers.resize(SubRendererOrder::COUNT);

    ISubRenderer* skydomeRenderer = new SkydomeRenderer();
    m_SubRenderers[SubRendererOrder::Skydome] = skydomeRenderer;
    
    ISubRenderer* terrainRenderer = new TerrainRenderer();
    m_SubRenderers[SubRendererOrder::Terrain] = terrainRenderer;

    ISubRenderer* waterRenderer = new WaterRenderer(WATER_LEVEL);
    m_SubRenderers[SubRendererOrder::Water] = waterRenderer;

    ISubRenderer* textRenderer = new TextRenderer(screenWidth, screenHeight);
    m_SubRenderers[SubRendererOrder::Text] = textRenderer;

    ISubRenderer* debugDisplayRenderer = new DebugDisplayRenderer();
    m_SubRenderers[SubRendererOrder::DebugDisplay] = debugDisplayRenderer;
    debugDisplayRenderer->SetIsEnabled(false);

    for (ISubRenderer* subRenderer : m_SubRenderers)
    {
        popAssert(subRenderer->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext()), "subRenderer failed init");
    }

    return true;
}


void D3D11Renderer::Shutdown()
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
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = nullptr;
    }
}

void D3D11Renderer::RegisterDrawable(VisualComponent* visComponent)
{
    if (visComponent)
    {
        GraphicsNode* node = new GraphicsNode(visComponent);
        node->Initialize(m_D3D->GetDevice());
        if (!visComponent->GetIs2D())
            m_Nodes.push_back(node);
    }
}

void D3D11Renderer::UnregisterDrawable(VisualComponent* visComponents)
{

}

bool D3D11Renderer::Frame()
{
    return Render();
}

bool D3D11Renderer::PreFrame()
{
    return true;
}

bool D3D11Renderer::RenderReflection()
{
    D3DXMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;
    
    m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());
    m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

    mat4x4 reflectedViewMatrix = g_Engine->GetCamera()->ComputeReflectionMatrix(WATER_LEVEL);
    m_D3D->GetProjectionMatrix(projectionMatrix);
    f32 clipPlane[4] = { 0, 1, 0, -WATER_LEVEL };
    //istoilov : we render everything before the water
    for (u8 i = 0; i < SubRendererOrder::Water; i++)
    {
        if (m_SubRenderers[i]->IsEnabled())
        {
            m_SubRenderers[i]->UpdateViewMatrix(reflectedViewMatrix);
            m_SubRenderers[i]->UpdateReflectionMatrix(reflectedViewMatrix);
            m_SubRenderers[i]->SetClipPlane(vec4(0, 1, 0, -WATER_LEVEL));
            m_SubRenderers[i]->Render(m_D3D);
        }
    }

    m_D3D->SetBackBufferRenderTarget();

    return true;
}

bool D3D11Renderer::Render()
{
    RenderReflection();

    m_D3D->BeginScene(0.f, 0.f, 0.f, 0.f);

    mat4x4 viewMatrix;
    g_Engine->GetCameraViewMatrix(viewMatrix);

    for (ISubRenderer* subRenderer : m_SubRenderers)
    {
        if (subRenderer->IsEnabled())
        {
            subRenderer->UpdateViewMatrix(viewMatrix);
            subRenderer->SetClipPlane(vec4::Zero);
            subRenderer->Render(m_D3D);
        }
    }

    //TODO istoilov : Move code to a dedicated solid object sub renderer.
    D3DXMATRIX projectionMatrix;
    m_D3D->GetProjectionMatrix(projectionMatrix);
    for (GraphicsNode* node : m_Nodes)
    {
        node->SetProjectionMatrix(mat4x4(projectionMatrix));
        node->SetViewMatrix(viewMatrix);
        node->Render(m_D3D->GetDeviceContext());
    }

    // Present the rendered scene to the screen.
    m_D3D->EndScene();

    return true;
}

D3D11Renderer::D3D11Renderer() :
    m_D3D(nullptr)
{
}

D3D11Renderer::~D3D11Renderer()
{
}

D3D11Renderer* D3D11Renderer::GetInstance()
{
    popAssert(s_Instance, "InputManager Create Instance Not called");
    return s_Instance;
}

void D3D11Renderer::CleanInstance()
{
    if (s_Instance != nullptr)
    {
        s_Instance->Shutdown();
        delete s_Instance;
    }
}

void D3D11Renderer::CreateInstance(HWND hwnd, u32 screenWidth, u32 screenHeight)
{
    if (s_Instance == nullptr)
    {
        s_Instance = new D3D11Renderer();
        s_Instance->Initialize(hwnd, screenWidth, screenHeight);
    }
}

void D3D11Renderer::EnableDebugDisplay(bool shouldEnable)
{ 
    GetDebugDisplayRenderer()->SetIsEnabled(shouldEnable);
}

bool D3D11Renderer::IsEnabledDebugDisplay() const
{
    return GetDebugDisplayRenderer()->IsEnabled();
}

DebugDisplayRenderer* D3D11Renderer::GetDebugDisplayRenderer() const
{
    return static_cast<DebugDisplayRenderer*>(m_SubRenderers[SubRendererOrder::DebugDisplay]); 
}