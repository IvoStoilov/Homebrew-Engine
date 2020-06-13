#include <engine/precompile.h>
#include <engine/engine.h>
#include <engine/camera.h>
#include <engine/entitymodel/components/visualcomponent.h>


#include <graphics/renderingengine.h>
 #ifdef POP_IMGUI_ENABLED
//#include <graphics/imgui/base/externimgui.h>
//#include <graphics/imgui/imgui.h>
#endif //POP_IMGUI_ENABLED

#include <system/viewprovider/viewprovider.h>
#include <system/inputmanager.h>
#include <system/profiling/profilemanager.h>
#include <system/commandline/commandlineoptions.h>

#include <thread>


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
    //This needs a flow refactor - bootstrap load?
    //if (g_CommandLineOptions.m_Binarize)
    //{
    //    popInfo(LogEngine, "Starting binarization");
    //    Terrain::BinarizeTerrain(g_CommandLineOptions.GetBinarizeInputFile(0),
    //                             g_CommandLineOptions.GetBinarizeInputFile(1),
    //                             g_CommandLineOptions.m_BinarizeOutputFile);
    //
    //    if (g_CommandLineOptions.m_QuitAfterInit)
    //        return true;
    //}

    ProfileManager::CreateInstance();
    ViewProvider::CreateInstance();
    InputManager::CreateInstance();
    RenderingEngine::CreateInstance();
    
//#ifdef POP_IMGUI_ENABLED
//    g_RenderEngine.GetImGuiRenderer()->RegisterRenderCallbackAndReturnIndex(std::bind(&Engine::ImGui_DisplayFPS, this));
//#endif //POP_IMGUI_ENABLED

    Window& window = g_ViewProvider.GetWindow(g_RenderEngine.GetGameWindowCookie());
    g_InputManager.AttachToWindow(window);

    m_Camera = std::make_unique<Camera>();
    popAssert(m_Camera != nullptr, "Memory Alloc Failed");
    m_Camera->SetPosition(-32.0f, +32.0f, -32.0f);

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
        g_RenderEngine.Update(dt);

        EndFrame();
    } while (!m_HasRequestedQuit);
}

void Engine::Shutdown()
{
    InputManager::CleanInstance();
    RenderingEngine::CleanInstance();
    ProfileManager::CleanInstance();
    ViewProvider::CleanInstance();

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

#ifdef POP_IMGUI_ENABLED
//void Engine::ImGui_DisplayFPS()
//{
//    ImGuiWindowFlags windowFlags = 0;
//    windowFlags |= ImGuiWindowFlags_NoTitleBar;
//    windowFlags |= ImGuiWindowFlags_NoScrollbar;
//    windowFlags |= ImGuiWindowFlags_NoMove;
//    windowFlags |= ImGuiWindowFlags_NoResize;
//    windowFlags |= ImGuiWindowFlags_NoCollapse;
//    
//    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
//    ImGui::SetNextWindowSize(ImVec2(125.f, 60.f));
//    ImGui::SetNextWindowBgAlpha(0.45f);
//
//    if (!ImGui::Begin("Overlay", nullptr, windowFlags))
//    {
//        ImGui::End();
//        return;
//    }
//
//    ImGui::Text("FPS : %.1f", 1.f / m_FrameTime.GetDT().count());
//    ImGui::Text("CPU : %.1f ms.", popDurationCast(Milliseconds, m_FrameTime.GetDT()).count());
//#ifdef POP_DEBUG
//    static const String CONFIGURATION = "Debug";
//#elif defined POP_RELEASE
//    static const String CONFIGURATION = "Release";
//#endif
//    ImGui::Text("%s", CONFIGURATION.c_str());
//    ImGui::End();
//}
#endif//POP_IMGUI_ENABLED