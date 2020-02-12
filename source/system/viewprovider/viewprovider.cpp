#include "system/precompile.h"
#include "system/viewprovider/viewprovider.h"
#include "system/commandline/commandlineoptions.h"

#include "system/error.h"

constexpr u32 WINDOWED_SCREEN_WIDTH = 1024;
constexpr u32 WINDOWED_SCREEN_HEIGHT = 720;

// **************************************
// *** Start Singleton Implementation ***
// **************************************

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
        sm_ViewProviderInstance = new ViewProvider();
        bool success = sm_ViewProviderInstance->Initialize();
        popAssert(success, "ViewProvider::Initialize failed.");
    }
}

void ViewProvider::DestroyInstnace()
{
    if (sm_ViewProviderInstance != nullptr)
    {
        sm_ViewProviderInstance->Shutdown();
        delete sm_ViewProviderInstance;
    }
}

// ************************************
// *** End Singleton Implementation ***
// ************************************

ViewProvider::ViewProvider()
    : m_ApplicationName("Homebrew_Engine")
    , m_WindowPosX(0)
    , m_WindowPosY(0)
    , m_WindowResolution()
{}

ViewProvider::~ViewProvider()
{
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

    if (!InitializeInternal())
        return false;

    return true;
}

void ViewProvider::Shutdown()
{
    ShutdownInternal();
}

void ViewProvider::SetDefaultPositionAndResolution()
{
    m_WindowResolution.m_Width = WINDOWED_SCREEN_WIDTH;
    m_WindowResolution.m_Height = WINDOWED_SCREEN_HEIGHT;

    WindowResolution monitorResolution = GetMonitorResolution();
    m_WindowPosX = (monitorResolution.m_Width - m_WindowResolution.m_Width) / 2;
    m_WindowPosY = (monitorResolution.m_Height - m_WindowResolution.m_Height) / 2;
}
