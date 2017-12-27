#include "engine.h"
#include "camera.h"

#include "system/inputmanager.h"
#include "system/error.h"
#include "renderer/d3d11renderer.h"

Engine* Engine::s_Instance = nullptr;

bool Engine::Initialize(HINSTANCE hInstance, HWND hwnd, uint32_t width, uint32_t height, int32_t windowPosX, int32_t windowPosY)
{
    InputManager::CreateInstance(hInstance, hwnd, width, height, windowPosX, windowPosY);

    m_Camera = new Camera();
    popAssert(m_Camera != nullptr, "Memory Alloc Failed");
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

    m_Entities.push_back(new Entity());
    m_Entities.push_back(new Entity());
    m_Entities[1]->SetGlobalPosition(vec4(2.f, 2.f, 2.f, 1.f));

    m_Renderer = new D3D11Renderer();
    popAssert(m_Renderer->Initialize(width, height, hwnd), "Renderer Init failed");

    for (Entity* entity : m_Entities)
    {
        m_Renderer->RegisterEntity(entity);
    }

    return true;
}


void Engine::Update()
{
    float dt = 0.003333;

    g_InputManager->Update();

    // Check if the user pressed escape and wants to exit the application.
    m_HasRequestedQuit = g_InputManager->IsEscapePressed();
    
    for (Entity* entity : m_Entities)
    {
        entity->Update(dt);
    }

    m_Camera->Update();

    // Render Engine
    m_Renderer->PreFrame();
    m_Renderer->Frame();
}

void Engine::Shutdown()
{
    InputManager::CleanInstance();

    if (m_Renderer)
    {
        m_Renderer->Shutdown();
        delete m_Renderer;
        m_Renderer = nullptr;
    }

    if (m_Camera)
    {
        delete m_Camera;
    }

    for (Entity* entity : m_Entities)
    {
        if (entity)
            delete entity;
    }
    m_Entities.clear();
}

void Engine::GetCameraViewMatrix(mat4x4& outMatrix) 
{
    m_Camera->GetViewMatrix(outMatrix); 
}

void Engine::CreateInstance(HINSTANCE hInstance, HWND hwnd, uint32_t width, uint32_t height, int32_t windowPosX, int32_t windowPosY)
{
    if (s_Instance == nullptr)
    {
        s_Instance = new Engine();
        popAssert(s_Instance->Initialize(hInstance, hwnd, width, height, windowPosX, windowPosY), "Engine::Initialize failed.");
    }
}

void Engine::CleanInstnace()
{
    if (s_Instance != nullptr)
    {
        s_Instance->Shutdown();
        delete s_Instance;
    }
}

Engine* Engine::GetInstance()
{
    popAssert(s_Instance != nullptr, "Engine Instance is not created.");
    return s_Instance;
}

Engine::Engine() :
    m_HasRequestedQuit(false)
{}

Engine::~Engine()
{}