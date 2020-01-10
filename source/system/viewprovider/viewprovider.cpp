#include <precompile.h>
#include <system/viewprovider/viewprovider.h>
#include <system/commandline/commandlineoptions.h>

#include <system/error.h>

#ifdef POP_PLATFORM_WINDOWS
#include <system/viewprovider/win64/win64viewprovider.h>
#endif //POP_PLATFORM_WINDOWS

constexpr u32 WINDOWED_SCREEN_WIDTH = 1024;
constexpr u32 WINDOWED_SCREEN_HEIGHT = 720;

ViewProvider* ViewProvider::sm_ViewProviderInstance = nullptr;

ViewProvider& ViewProvider::GetInstance()
{
    popAssert(sm_ViewProviderInstance, "ViewProvider::CreateInstance() not called.");
    return *sm_ViewProviderInstance;
}

void ViewProvider::CreateInstance()
{
    if (!sm_ViewProviderInstance)
    {
#ifdef POP_PLATFORM_WINDOWS
        sm_ViewProviderInstance = new Win64ViewProvider();
#endif //POP_PLATFORM_WINDOWS
        bool success = sm_ViewProviderInstance->Initialize();
        popAssert(success, "ViewProvider::Initialize failed.");
    }
}
    
void ViewProvider::CleanInstnace()
{
    if (sm_ViewProviderInstance != nullptr)
    {
        sm_ViewProviderInstance->Shutdown();
        delete sm_ViewProviderInstance;
    }
}

ViewProvider::ViewProvider()
    : m_ApplicationName("Homebrew_Engine")
    , m_WindowHandle(nullptr)
    , m_WindowPosX(0)
    , m_WindowPosY(0)
    , m_WindowResolution()
{}

ViewProvider::~ViewProvider()
{
    if (m_WindowHandle)
    {
        delete m_WindowHandle;
    }
}

bool ViewProvider::Initialize()
{
    m_ApplicationName = "Homebrew Engine";

    if (g_CommandLineOptions->m_Fullscreen)
    {
        ActivateFullscreen();
    }
    else
    {
        SetDefaultPositionAndResolution();
    }

    InitializeWindowHandle();

    if (!InitializeInternal())
        return false;

    return true;
}

void ViewProvider::Shutdown()
{
    ShutdownInternal();
}

void ViewProvider::InitializeWindowHandle()
{
#ifdef POP_PLATFORM_WINDOWS
    m_WindowHandle = new Win64WindowHandle();
#else
    #error Platform Not Supported!
#endif
}

void ViewProvider::SetDefaultPositionAndResolution()
{
    m_WindowResolution.m_Width = WINDOWED_SCREEN_WIDTH;
    m_WindowResolution.m_Height = WINDOWED_SCREEN_HEIGHT;

    WindowResolution monitorResolution = GetMonitorResolution();
    m_WindowPosX = (monitorResolution.m_Width - m_WindowResolution.m_Width) / 2;
    m_WindowPosY = (monitorResolution.m_Height - m_WindowResolution.m_Height) / 2;
}
