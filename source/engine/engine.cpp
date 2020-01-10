#include "precompile.h"
#include "engine/engine.h"
#include "engine/camera.h"
#include "engine/entitymodel/components/visualcomponent.h"

#include <system/viewprovider/viewprovider.h>
#include "system/inputmanager.h"
#include "system/profiling/profilemanager.h"
#include "system/commandline/commandlineoptions.h"
#include "system/error.h"

#include "graphics/d3d11renderer.h"

//Temp Hack
#include "graphics/terrain/terrain.h"

#include <thread>
#include <chrono>

Engine* Engine::s_Instance = nullptr;

bool Engine::Initialize()
{
    ProfileManager::CreateInstance();

    //This needs a flow refactor - bootstrap load?
    if (g_CommandLineOptions->m_Binarize)
    {
        Terrain::BinarizeTerrain(g_CommandLineOptions->GetBinarizeInputFile(0),
                                 g_CommandLineOptions->GetBinarizeInputFile(1),
                                 g_CommandLineOptions->m_BinarizeOutputFile);

        if (g_CommandLineOptions->m_QuitAfterInit)
            return true;
    }

    ViewProvider::CreateInstance();
    ViewProvider& viewProvider = ViewProvider::GetInstance();

    InputManager::CreateInstance(viewProvider);
    D3D11Renderer::CreateInstance(*viewProvider.GetWindowHandle(), viewProvider.GetWindowWidth(), viewProvider.GetWindowHeight());

    m_Camera = new Camera();
    popAssert(m_Camera != nullptr, "Memory Alloc Failed");
    m_Camera->SetPosition(-32.0f, +32.0f, -32.0f);

    {
        VisualComponent* visComp = nullptr;
        m_Entities.push_back(new Entity());
        m_Entities[0]->SetGlobalPosition(vec4(0.f, 10.f, 65.f, 1.f));
        visComp = new VisualComponent();
        visComp->SetModelPath(std::string("../../resource/geometry/cube.bgd"));
        visComp->SetTexturePath(std::string("../../resource/metal_texture.jpg"));
        m_Entities[0]->AddComponent(visComp);

        m_Entities.push_back(new Entity());
        m_Entities[1]->SetGlobalPosition(vec4(65.f, 10.f, 0.f, 1.f));
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
    if (g_CommandLineOptions->m_QuitAfterInit)
        return;

    m_WorldClock.Update();
    float dt = GetFrameTimeInS();

    ViewProvider::GetInstance().Update();

    g_InputManager->Update();

    // Check if the user pressed escape and wants to exit the application.
    m_HasRequestedQuit = m_HasRequestedQuit || g_InputManager->IsEscapePressed();

    m_InputHandler.Update();

    m_FPSCounter.Update();
    m_CPUInfo.Update();

    for (Entity* entity : m_Entities)
    {
        entity->Update(dt);
    }

    m_Camera->Update(dt);

    
    g_RenderEngine->PreFrame();
    g_RenderEngine->Frame(dt);

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

    g_ProfileManager.DumpInfoToFile();
    ProfileManager::CreateInstance();

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

void Engine::CreateInstance()
{
    if (s_Instance == nullptr)
    {
        s_Instance = new Engine();
        popAssert(s_Instance->Initialize(), "Engine::Initialize failed.");
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