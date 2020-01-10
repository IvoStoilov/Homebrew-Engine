#include <precompile.h>
#include <system/viewprovider/win64/win64viewprovider.h>

#ifdef POP_PLATFORM_WINDOWS

#include <system/commandline/commandlineoptions.h>

bool Win64ViewProvider::InitializeInternal()
{
    m_hInstance = GetModuleHandle(NULL);

    // Setup the windows class with default settings.
    WNDCLASSEX wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_ApplicationName.c_str();
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    // Create the window with the screen settings and get the handle to it.
    HWND& hwnd = GetWin64WindowHandle();
    hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName.c_str(), m_ApplicationName.c_str(),
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        m_WindowPosX, m_WindowPosY, m_WindowResolution.m_Width, m_WindowResolution.m_Height, NULL, NULL, m_hInstance, NULL);

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // Hide the mouse cursor.
    ShowCursor(g_CommandLineOptions->m_ShowCursor);
}

void Win64ViewProvider::ShutdownInternal()
{
    // Show the mouse cursor.
    ShowCursor(true);

    if (g_CommandLineOptions->m_Fullscreen)
    {
        DeactivateFullscreen();
    }

    m_WindowHandle->ReleaseHandle();

    // Remove the application instance.
    UnregisterClass(m_ApplicationName.c_str(), m_hInstance);
    m_hInstance = NULL;
}

void Win64ViewProvider::Update()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (true)
    {
        // Handle the windows messages.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // If windows signals to end the application then exit out.
        if (msg.message == WM_QUIT)
            return;
    }
}

void Win64ViewProvider::ActivateFullscreen()
{
    // Determine the resolution of the clients desktop screen.
    m_WindowResolution = GetMonitorResolution();

    DEVMODE dmScreenSettings;
    // If full screen set the screen to maximum size of the users desktop and 32bit.
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth = static_cast<DWORD>(m_WindowResolution.m_Width);
    dmScreenSettings.dmPelsHeight = static_cast<DWORD>(m_WindowResolution.m_Height);
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Change the display settings to full screen.
    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

    // Set the position of the window to the top left corner.
    m_WindowPosX = m_WindowPosY = 0;
}

void Win64ViewProvider::DeactivateFullscreen()
{
    // Fix the display settings if leaving full screen mode.
    ChangeDisplaySettings(NULL, 0);
}

ViewProvider::WindowResolution Win64ViewProvider::GetMonitorResolution()
{
    return ViewProvider::WindowResolution(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}                                         

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
        // Check if the window is being destroyed.
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        // Check if the window is being closed.
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }

        // All other messages pass to the message handler in the system class.
        default:
        {
            return DefWindowProc(hwnd, umessage, wparam, lparam);

        }
    }
}

void Win64WindowHandle::ReleaseHandle()
{
    DestroyWindow(m_HWND);
    m_HWND = NULL;
}

#endif //POP_PLATFORM_WINDOWS