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
#include "renderer/font/text.h"

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
    

    m_2DModel = new Model2D();
    popAssert(m_2DModel->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, path, 170, 100), "2d model failed initing");

    m_TextureShader = new TextureShader();
    popAssert(m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd), "asd");


    m_TerrainShader = new D3D11Shader();
    popAssert(m_TerrainShader->Initialize(m_D3D->GetDevice(), hwnd), "Terrain shader failed initing");

    m_Terrain = new Terrain();
    popAssert(m_Terrain->Initialize(m_D3D->GetDevice()), "Terrain mesh failed initing");

    m_Text = new Text();
    popAssert(m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), screenWidth, screenHeight, 3, 10, 590), "Text Init failed");

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

    if (m_Text)
    {
        m_Text->Shutdown();
        delete m_Text;
        m_Text = nullptr;
    }

    if (m_2DModel)
    {
        m_2DModel->Shutdown();
        delete m_2DModel;
    }

    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
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
    

    // TODO (istoilov) Integrate code into graphics node on next pass
    //Render 2D models
    { 
        m_D3D->TurnDepthTestOff();

        char fpsText[40];
        char cpuText[40];
        char frameTimeText[40];
        std::sprintf(fpsText, "FPS : %d", g_Engine->GetFPS());
        std::sprintf(cpuText, "CPU: %d%%", g_Engine->GetCPUUsage());
        std::sprintf(frameTimeText, "Engine: %.1f ms", g_Engine->GetFrameTime());

        //TODO (istoilov) Implement smart line system in the Text Class, change hardcoded Sentances with dynamic arrays
        m_Text->SetText(fpsText, 0);
        m_Text->SetText(cpuText, 1);
        m_Text->SetText(frameTimeText, 2);

        D3DXMATRIX orthoMatrix;
        D3DXMATRIX worldMatrix;
        D3DXMATRIX id;

        D3DXMatrixIdentity(&id);
        D3DXMatrixIdentity(&worldMatrix);

        worldMatrix._43 += 5.f;

        m_D3D->GetOrthoMatrix(orthoMatrix);

        {
            m_D3D->TurnAlphaBlendingOn();
            
            m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);

            m_D3D->TurnAlphaBlendingOff();
        }
        // Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
        //m_2DModel->Render(m_D3D->GetDeviceContext(), 10, 10);
        //m_TextureShader->Render(m_D3D->GetDeviceContext(), m_2DModel->GetIndexCount(), worldMatrix, id, orthoMatrix, m_2DModel->GetTexture());
    }
    m_D3D->TurnDepthTestOn();
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