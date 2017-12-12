#include <windows.h>

#include "renderer/d3d11renderer.h"
#include "renderer/d3d11model.h"
#include "renderer/d3d11shader.h"
#include "renderer/textureshader.h"
#include "renderer/d3d11.h"

#include "camera.h"

std::string TEXTURE_PATH = "resource/ubisoft-logo.png";

D3D11Renderer::D3D11Renderer() :
    m_D3D(nullptr),
    m_Model(nullptr),
    m_Shader(nullptr),
    m_TextureShader(nullptr),
    m_Camera(nullptr)
{
}

D3D11Renderer::~D3D11Renderer()
{
}


bool D3D11Renderer::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result = false;
    m_D3D = new D3D11();
    if (!m_D3D)
        return false;

     result = m_D3D->Initialize(screenWidth, screenHeight, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        return false;
    }

    // Create the camera object.
    m_Camera = new Camera();
    if (!m_Camera)
    {
        return false;
    }

    // Set the initial position of the camera.
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

    // Create the model object.
    m_Model = new D3D11Model();
    if (!m_Model)
    {
        return false;
    }

    // Initialize the model object.
    result = m_Model->Initialize(m_D3D->GetDevice(), TEXTURE_PATH);
    if (!result)
    {
//        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    // Create the color shader object.
    m_Shader = new D3D11Shader();
    if (!m_Shader)
    {
        return false;
    }

    // Initialize the color shader object.
    result = m_Shader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
 //       MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
        return false;
    }
    

    m_TextureShader = new TextureShader();
    if (!m_TextureShader)
    {
        return false;
    }

    result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        return false;
    }

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
    
    // Clear the buffers to begin the scene.
    m_D3D->BeginScene(0.3f, 0.6f, 0.8f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Update();

    // Get the world, view, and projection matrices from the camera and d3d objects.
    m_Camera->GetViewMatrix(viewMatrix);
    m_D3D->GetWorldMatrix(worldMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
    m_Model->Render(m_D3D->GetDeviceContext());

    // Render the model using the color shader.
    //m_Shader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
    m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());

    // Present the rendered scene to the screen.
    m_D3D->EndScene();

    return true;
}