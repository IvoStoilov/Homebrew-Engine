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
WindowCookie ViewProvider::CreateChildWindow(u32 width, u32 height, const String& windowName, bool sysMenu, bool border)
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

    DWORD style = 0l;

    style |= WS_CLIPSIBLINGS;
    style |= WS_CLIPCHILDREN;
    style |= WS_POPUP;
    style |= WS_CAPTION;
    style |= (border) ? WS_BORDER : 0l;
    style |= (sysMenu) ? WS_SYSMENU : 0l;

    // Create the window with the screen settings and get the handle to it.
    HWND childWindowHWND = CreateWindowEx(WS_EX_APPWINDOW, "Child Window", windowName.c_str(),
       style, m_WindowPosX, m_WindowPosY, width, height, NULL, NULL, m_Win64_hInstnace, NULL);

    popAssert(childWindowHWND, "Child Window was not created. Error : {}", GetLastError());

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
