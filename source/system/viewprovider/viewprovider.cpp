#include <system/precompile.h>
#include <system/viewprovider/viewprovider.h>
#include <system/commandline/commandlineoptions.h>

constexpr u32 WINDOWED_SCREEN_WIDTH = 1024;
constexpr u32 WINDOWED_SCREEN_HEIGHT = 720;

ViewProvider::ViewProvider()
    : m_ApplicationName("Homebrew_Engine")
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
    InitializeInternal();
    return true;
}

void ViewProvider::Shutdown()
{
    ShutdownInternal();
}

void ViewProvider::ClearChildWindow(WindowCookie& cookie)
{
    if (cookie.m_Index == WindowCookie::INVALID_INDEX)
        return;

    m_Windows.erase(m_Windows.begin() + cookie.m_Index);
    cookie.m_Index = WindowCookie::INVALID_INDEX;
}