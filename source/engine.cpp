#include "engine.h"
#include "camera.h"

#include "system/inputmanager.h"
#include "system/error.h"
#include "renderer/d3d11renderer.h"
#include "entitymodel/components/visualcomponent.h"

#include <thread>
#include <chrono>
/*
Texture
"../../resource/ubisoft-logo.png"
"../../resource/ink-splatter-texture.png"
"../../resource/metal_texture.jpg"

Model
"../../resource/geometry/cube.bgd"
*/

Engine* Engine::s_Instance = nullptr;

bool Engine::Initialize(HINSTANCE hInstance, HWND hwnd, uint32_t width, uint32_t height, int32_t windowPosX, int32_t windowPosY)
{
    InputManager::CreateInstance(hInstance, hwnd, width, height, windowPosX, windowPosY);

    D3D11Renderer::CreateInstance(hwnd, width, height);

    m_Camera = new Camera();
    popAssert(m_Camera != nullptr, "Memory Alloc Failed");
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

    {
        VisualComponent* visComp = nullptr;
        m_Entities.push_back(new Entity());
        visComp = new VisualComponent();
        visComp->SetModelPath(std::string("../../resource/geometry/cube.bgd"));
        visComp->SetTexturePath(std::string("../../resource/metal_texture.jpg"));
        m_Entities[0]->AddComponent(visComp);

        m_Entities.push_back(new Entity());
        m_Entities[1]->SetGlobalPosition(vec4(2.f, 2.f, 2.f, 1.f));
        visComp = new VisualComponent();
        visComp->SetModelPath(std::string("../../resource/geometry/cube.bgd"));
        visComp->SetTexturePath(std::string("../../resource/ink-splatter-texture.png"));
        m_Entities[1]->AddComponent(visComp);

        for (Entity* entity : m_Entities)
        {
            g_RenderEngine->RegisterDrawable(entity->GetComponentByType<VisualComponent>());
        }
    }

    return true;
}

//----Main Update loop----
void Engine::Update()
{
    m_WorldClock.Update();
    float dt = GetFrameTime();

    g_InputManager->Update();

    // Check if the user pressed escape and wants to exit the application.
    m_HasRequestedQuit = g_InputManager->IsEscapePressed();
    
    m_FPSCounter.Update();
    m_CPUInfo.Update();

    for (Entity* entity : m_Entities)
    {
        entity->Update(dt);
    }

    m_Camera->Update();

    
    g_RenderEngine->PreFrame();
    g_RenderEngine->Frame();

    float delta = m_WorldClock.GetDeltaTime();
    while (delta < 16.6666)
    {
        delta = m_WorldClock.GetDeltaTime();
        //Sleep granularity of very rough and leads to artificial FPS drop from 60 to 30
        //Going with a crappy solution for a while cycle. 
        //Not sure what is the implementation of the VSYNC and what are the tradeoffs.
        //std::this_thread::sleep_for(std::chrono::milliseconds(17 - delta));
    }
}

void Engine::Shutdown()
{
    InputManager::CleanInstance();

    D3D11Renderer::CleanInstance();

    if (m_Camera)
    {
        delete m_Camera;
    }

    for (Entity* entity : m_Entities)
    {
        if (entity)
        {
            entity->OnDestroy();
            delete entity;
        }
    }
    m_Entities.clear();

    m_CPUInfo.Shutdown();
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