#include <engine/precompile.h>
#include <engine/engine.h>
#include <engine/camera.h>
#include <engine/entitymodel/components/visualcomponent.h>

//Temp Hack
#include <graphics/renderingengine.h>
#include <graphics/terrain/terrain.h>

#include <system/viewprovider/viewprovider.h>
#include <system/inputmanager.h>
#include <system/profiling/profilemanager.h>
#include <system/commandline/commandlineoptions.h>

#include <thread>
#include <chrono>

constexpr f32 WATER_LEVEL = 3.f;

Engine::Engine()
{
    Initialize();
}

Engine::~Engine()
{
    Shutdown();
}

bool Engine::Initialize()
{
    ProfileManager::CreateInstance();

    //This needs a flow refactor - bootstrap load?
    if (g_CommandLineOptions.m_Binarize)
    {
        popInfo(LogEngine, "Starting binarization");
        Terrain::BinarizeTerrain(g_CommandLineOptions.GetBinarizeInputFile(0),
                                 g_CommandLineOptions.GetBinarizeInputFile(1),
                                 g_CommandLineOptions.m_BinarizeOutputFile);

        if (g_CommandLineOptions.m_QuitAfterInit)
            return true;
    }

    ViewProvider::CreateInstance();
    InputManager::CreateInstance();
    RenderingEngine::CreateInstance();
    g_RenderEngine.Initialize();
    GfxWindow& gameWindow = g_RenderEngine.GetGameWindow();
    Window& window = g_ViewProvider.GetWindow(gameWindow.GetWindowCookie());
    g_InputManager.AttachToWindow(window);

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
            g_RenderEngine.RegisterDrawable(entity->GetComponentByType<VisualComponent>());
        }
    }

    return true;
}

//----Main Update loop----
void Engine::Update()
{
    do
    {
        popProfile(Engine::Update);
        if (g_CommandLineOptions.m_QuitAfterInit)
            return;

        m_FrameTime.BeginFrame();
        f32 dt = m_FrameTime.GetDT().count();

        ViewProvider::GetInstance().Update();

        g_InputManager.Update();

        // Check if the user pressed escape and wants to exit the application.
        m_HasRequestedQuit = m_HasRequestedQuit || g_InputManager.IsEscapePressed();

        m_InputHandler.Update();

        m_FPSCounter.Update();
        m_CPUInfo.Update();

        for (Entity* entity : m_Entities)
        {
            entity->Update(dt);
        }

        m_Camera->Update(dt);

        g_RenderEngine.SetViewMatrix(m_Camera->GetViewMatrix());
        g_RenderEngine.SetReflectedViewMatrix(m_Camera->ComputeReflectionMatrix(WATER_LEVEL));
        g_RenderEngine.PreFrame();
        g_RenderEngine.Frame(dt);

        EndFrame();
    } while (!m_HasRequestedQuit);
}

void Engine::Shutdown()
{
    InputManager::CleanInstance();
    RenderingEngine::CleanInstance();
    ProfileManager::CleanInstance();
    ViewProvider::CleanInstance();

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

void Engine::EndFrame()
{
    popProfile(Engine::EndFrame);
    //Sleep granularity is very rough and leads to artificial FPS drop from 60 to 30
    //Going with an artificial Spin Lock instead of
    //std::this_thread::sleep_for(std::chrono::milliseconds(17 - delta));

    Milliseconds elapsedFrameTime = m_FrameTime.GetCurrentFrameElapsedTime();
    while (elapsedFrameTime < Milliseconds(16.6666f))
    {
        elapsedFrameTime = m_FrameTime.GetCurrentFrameElapsedTime();
    }
}