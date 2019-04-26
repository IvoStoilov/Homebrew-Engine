#include "precompile.h"
#include "win64/win64viewprovider.h"
#include "system/inputmanager.h"
#include "system/commandline/commandlineoptions.h"
#include "core/engine.h"

const uint32_t WINDOWED_SCREEN_WIDTH = 1024;
const uint32_t WINDOWED_SCREEN_HEIGHT = 720;

Win64_ViewProvider::Win64_ViewProvider()
{
}

Win64_ViewProvider::~Win64_ViewProvider()
{
}

bool Win64_ViewProvider::Initialize()
{
    int32_t screenWidth, screenHeight;

    // Initialize the width and height of the screen to zero before sending the variables into the function.
    screenWidth = 0;
    screenHeight = 0;

    // Initialize the windows api.
    InitializeWindows(screenWidth, screenHeight);

    Engine::CreateInstance(m_hInstance, m_HWND, screenWidth, screenHeight, m_WindowPosX, m_WindowPosY);

    return true;
}

void Win64_ViewProvider::Shutdown()
{
    Engine::CleanInstnace();
    ShutdownWindows();
}

void Win64_ViewProvider::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    if (g_CommandLineOptions->m_QuitAfterInit)
        return;

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

        if (g_Engine->HasRequestedQuit())
            return;

        g_Engine->Update();
    }
}

void Win64_ViewProvider::InitializeWindows(int& screenWidth, int& screenHeight)
{
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;

    // Get the instance of this application.
    m_hInstance = GetModuleHandle(NULL);

    // Give the application a name.
    m_ApplicationName = "Homebrew Engine";

    // Setup the windows class with default settings.
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
    wc.lpszClassName = m_ApplicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    // Determine the resolution of the clients desktop screen.
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Setup the screen settings depending on whether it is running in full screen or in windowed mode.
    if (g_CommandLineOptions->m_Fullscreen)
    {
        // If full screen set the screen to maximum size of the users desktop and 32bit.
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change the display settings to full screen.
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        // Set the position of the window to the top left corner.
        m_WindowPosX = m_WindowPosY = 0;
    }
    else
    {
        // If windowed then set it to 800x600 resolution.
        screenWidth = WINDOWED_SCREEN_WIDTH;
        screenHeight = WINDOWED_SCREEN_HEIGHT;

        // Place the window in the middle of the screen.
        m_WindowPosX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        m_WindowPosY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // Create the window with the screen settings and get the handle to it.
    m_HWND = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName, m_ApplicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        m_WindowPosX, m_WindowPosY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(m_HWND, SW_SHOW);
    SetForegroundWindow(m_HWND);
    SetFocus(m_HWND);

    // Hide the mouse cursor.
    ShowCursor(g_CommandLineOptions->m_ShowCursor);
}

void Win64_ViewProvider::ShutdownWindows()
{
    // Show the mouse cursor.
    ShowCursor(true);

    // Fix the display settings if leaving full screen mode.
    if (g_CommandLineOptions->m_Fullscreen)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    // Remove the window.
    DestroyWindow(m_HWND);
    m_HWND = NULL;

    // Remove the application instance.
    UnregisterClass(m_ApplicationName, m_hInstance);
    m_hInstance = NULL;
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
        DefWindowProc(hwnd, umessage, wparam, lparam);
    }
    }

}