#include <windows.h>

#include "system/error.h"

#include "entitymodel/entity.h"

#include "renderer/graphicsnode.h"
#include "renderer/d3d11renderer.h"
#include "renderer/d3d11.h"

#include "camera.h"
#include "engine.h"

D3D11Renderer::D3D11Renderer() :
    m_D3D(nullptr)
{
}

D3D11Renderer::~D3D11Renderer()
{
}


bool D3D11Renderer::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    m_D3D = new D3D11();
    popAssert(m_D3D != nullptr, "Memory Alloc Failed");
    popAssert(m_D3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR), "D3D Init Failed");
    
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

    if (m_D3D)
    {
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = nullptr;
    }
}

void D3D11Renderer::RegisterEntity(Entity* entity)
{
    GraphicsNode* node = new GraphicsNode(entity);
    node->Initialize(m_D3D->GetDevice());
    m_Nodes.push_back(node);
}

void D3D11Renderer::UnregisterEntity(Entity* entity)
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
    m_D3D->BeginScene(0.3f, 0.6f, 0.8f, 1.0f);

    m_D3D->GetProjectionMatrix(m_ProjectionMatrix);
    for (GraphicsNode* node : m_Nodes)
    {
        node->SetProjectionMatrix(mat4x4(m_ProjectionMatrix)); // TODO (istoilov) fix nasty hack;
        node->SetViewMatrix(m_ViewMatrix);
        node->Render(m_D3D->GetDeviceContext());
    }
    
    // Present the rendered scene to the screen.
    m_D3D->EndScene();

    return true;
}