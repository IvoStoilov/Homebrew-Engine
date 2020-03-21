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

extern LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
WindowCookie ViewProvider::CreateChildWindow()
{
    WNDCLASSEX wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_Win64_hInstnace;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "Child Window";
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    // Create the window with the screen settings and get the handle to it.
    HWND childWindowHWND = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName.c_str(), m_ApplicationName.c_str(),
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_CAPTION | WS_BORDER | WS_SYSMENU,
        m_WindowPosX, m_WindowPosY, m_WindowResolution.m_Width, m_WindowResolution.m_Height, NULL, NULL, m_Win64_hInstnace, NULL);

    RECT clientRect;
    GetClientRect(childWindowHWND, &clientRect);
    //m_WindowResolution.m_Width = clientRect.right;
    //m_WindowResolution.m_Height = clientRect.bottom;
    // Bring the window up on the screen and set it as main focus.
    ShowWindow(childWindowHWND, SW_SHOW);
    SetForegroundWindow(childWindowHWND);
    SetFocus(childWindowHWND);

    // Hide the mouse cursor.
    ShowCursor(true);
    m_ChildWindows.push_back(childWindowHWND);
    return WindowCookie(static_cast<u32>(m_ChildWindows.size() - 1));
}


void ViewProvider::ClearChildWindow(WindowCookie& cookie)
{
    if (cookie.m_Index == WindowCookie::INVALID_INDEX)
        return;

    m_ChildWindows.erase(m_ChildWindows.begin() + cookie.m_Index);
    cookie.m_Index = WindowCookie::INVALID_INDEX;
}

void ViewProvider::SetDefaultPositionAndResolution()
{
    m_WindowResolution.m_Width = WINDOWED_SCREEN_WIDTH;
    m_WindowResolution.m_Height = WINDOWED_SCREEN_HEIGHT;

    WindowResolution monitorResolution = GetMonitorResolution();
    m_WindowPosX = (monitorResolution.m_Width - m_WindowResolution.m_Width) / 2;
    m_WindowPosY = (monitorResolution.m_Height - m_WindowResolution.m_Height) / 2;
}
