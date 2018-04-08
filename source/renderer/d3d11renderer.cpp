#include <windows.h>

#include "system/error.h"

#include "entitymodel/entity.h"
#include "entitymodel/components/visualcomponent.h"

#include "renderer/graphicsnode.h"
#include "renderer/d3d11shader.h"
#include "renderer/d3d11renderer.h"
#include "renderer/d3d11.h"

#include "renderer/model2d.h"
#include "renderer/textureshader.h"
#include "renderer/textrendering/text.h"

#include "renderer/isubrenderer.h"
#include "renderer/textrendering/textrenderer.h"

#include "renderer/terrain/terrain.h"

#include "camera.h"
#include "engine.h"

std::string path = "../../resource/ink-splatter-texture.png";

D3D11Renderer* D3D11Renderer::s_Instance = nullptr;

bool D3D11Renderer::Initialize(HWND hwnd, uint32_t screenWidth, uint32_t screenHeight)
{
    m_D3D = new D3D11();
    popAssert(m_D3D != nullptr, "Memory Alloc Failed");
    popAssert(m_D3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR), "D3D Init Failed");

    ISubRenderer* textRenderer = new TextRenderer(screenWidth, screenHeight);
    m_SubRenderers.push_back(textRenderer);

    for (ISubRenderer* subRenderer : m_SubRenderers)
    {
        popAssert(subRenderer->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext()), "subRenderer failed init");
    }

    m_TextureShader = new TextureShader();
    popAssert(m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd), "asd");

    m_TerrainShader = new D3D11Shader();
    popAssert(m_TerrainShader->Initialize(m_D3D->GetDevice(), hwnd), "Terrain shader failed initing");

    m_Terrain = new Terrain();
    popAssert(m_Terrain->Initialize(m_D3D->GetDevice()), "Terrain mesh failed initing");

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
    }

    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
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
    mat4x4 viewMatrix;
    g_Engine->GetCameraViewMatrix(viewMatrix);

    m_ViewMatrix = viewMatrix.ToD3DXMATRIX();

    return true;
}

bool D3D11Renderer::Render()
{
    mat4x4 globalMatrix;
    // Clear the buffers to begin the scene.
    m_D3D->BeginScene(0.f, 0.f, 0.f, 0.f);

    //Terrain Grid Render pass
    {
        D3DXMATRIX worldMatrix;
        D3DXMatrixIdentity(&worldMatrix);
        m_Terrain->Render(m_D3D->GetDeviceContext());
        m_TerrainShader->Render(m_D3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, m_ViewMatrix, m_ProjectionMatrix);
    }

    m_D3D->GetProjectionMatrix(m_ProjectionMatrix);
    for (GraphicsNode* node : m_Nodes)
    {
        node->SetProjectionMatrix(mat4x4(m_ProjectionMatrix)); // TODO (istoilov) fix nasty hack;
        node->SetViewMatrix(m_ViewMatrix);
        node->Render(m_D3D->GetDeviceContext());
    }
    
    for (ISubRenderer* subRenderer : m_SubRenderers)
        subRenderer->Render(m_D3D);

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

void D3D11Renderer::CreateInstance(HWND hwnd, uint32_t screenWidth, uint32_t screenHeight)
{
    if (s_Instance == nullptr)
    {
        s_Instance = new D3D11Renderer();
        s_Instance->Initialize(hwnd, screenWidth, screenHeight);
    }
}