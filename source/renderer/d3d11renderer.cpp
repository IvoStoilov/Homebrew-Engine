#include <windows.h>

#include "system/error.h"

#include "entitymodel/entity.h"

#include "renderer/d3d11renderer.h"
#include "renderer/d3d11model.h"
#include "renderer/d3d11shader.h"
#include "renderer/textureshader.h"
#include "renderer/lightshader.h"
#include "renderer/d3d11.h"

#include "camera.h"
/*
"resource/ubisoft-logo.png";
"resource/ink-splatter-texture.png";
"resource/metal_texture.jpg";
*/
std::string TEXTURE_PATH = "resource/metal_texture.jpg";
std::string MODEL_PATH = "resource/geometry/cube.bgd";

D3DXVECTOR4 DIFFUSE_COLOR(1.f, 1.f, 1.f, 1.f);
D3DXVECTOR4 LIGHT_DIRECTION(-.3f, -.3f, 1.f, 0.f);


D3D11Renderer::D3D11Renderer() :
    m_D3D(nullptr),
    m_Model(nullptr),
    m_Shader(nullptr),
    m_TextureShader(nullptr),
    m_Camera(nullptr),
    m_CubeEntity(nullptr),
    m_Angle(0)
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
    
    m_Camera = new Camera();
    popAssert(m_Camera != nullptr, "Memory Alloc Failed");

    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
    m_CubeEntity = new Entity();
    
    // Create the model object.
    m_Model = new D3D11Model();
    popAssert(m_Model != nullptr, "Memory Alloc Failed");
    popAssert(m_Model->Initialize(m_D3D->GetDevice(), MODEL_PATH, TEXTURE_PATH), "Model Init Failed");
   
    m_Shader = new D3D11Shader();
    popAssert(m_Shader != nullptr, "Memory Alloc Failed");
    popAssert(m_Shader->Initialize(m_D3D->GetDevice(), hwnd), "Shader Init Failed");
    
    m_TextureShader = new TextureShader();
    popAssert(m_TextureShader != nullptr, "Memory Alloc Failed");
    popAssert(m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd), "Shader Init Failed");

    m_LightShader = new LightShader();
    popAssert(m_LightShader != nullptr, "Memory Alloc Failed");
    popAssert(m_LightShader->Initialize(m_D3D->GetDevice(), hwnd), "Shader Init Failed");

    return true;
}


void D3D11Renderer::Shutdown()
{
    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
    }

    if (m_Shader)
    {
        m_Shader->Shutdown();
        delete m_Shader;
    }

    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
    }

    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
    }

    if (m_Camera)
    {
        delete m_Camera;
    }

    if (m_D3D)
    {
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = nullptr;
    }
}


bool D3D11Renderer::Frame()
{
    return Render();
}


bool D3D11Renderer::Render()
{
    D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
    mat4x4 globalMatrix;
    // Clear the buffers to begin the scene.
    m_D3D->BeginScene(0.3f, 0.6f, 0.8f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Update();

    m_CubeEntity->Rotate(m_Angle);
    m_Angle += 1.f;
    m_Angle = m_Angle % 360;

    // Get the world, view, and projection matrices from the camera and d3d objects.
    m_Camera->GetViewMatrix(viewMatrix);

    globalMatrix = m_CubeEntity->GetGlobalMatrix();
    worldMatrix = globalMatrix.ToD3DXMATRIX();

    m_D3D->GetProjectionMatrix(projectionMatrix);

    // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
    m_Model->Render(m_D3D->GetDeviceContext());

    // Render the model using the color shader.
    //m_Shader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
    //m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
    m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), DIFFUSE_COLOR, LIGHT_DIRECTION);

    // Present the rendered scene to the screen.
    m_D3D->EndScene();

    return true;
}