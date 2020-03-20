#include <system/precompile.h>
#include <system/viewprovider/viewprovider.h>
#include <system/commandline/commandlineoptions.h>

constexpr u32 WINDOWED_SCREEN_WIDTH = 1024;
constexpr u32 WINDOWED_SCREEN_HEIGHT = 720;

ViewProvider::ViewProvider()
    : m_ApplicationName("Homebrew_Engine")
    , m_WindowPosX(0)
    , m_WindowPosY(0)
    , m_WindowResolution()
{
    Initialize();
}

ViewProvider::~ViewProvider()
{
    Shutdown();
}

bool ViewProvider::Initialize()
{
    m_ApplicationName = "Homebrew Engine";

    if (g_CommandLineOptions.m_Fullscreen)
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
