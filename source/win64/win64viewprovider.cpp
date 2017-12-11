#include "win64/win64viewprovider.h"

Win64_ViewProvider::Win64_ViewProvider() :
    m_Fullscreen(false),
    m_InputHandler(nullptr),
    m_Renderer(nullptr)

{
}

Win64_ViewProvider::~Win64_ViewProvider()
{
    delete m_Renderer;
    delete m_InputHandler;
}

bool Win64_ViewProvider::Initialize()
{
    int screenWidth, screenHeight;
    bool result;

    // Initialize the width and height of the screen to zero before sending the variables into the function.
    screenWidth = 0;
    screenHeight = 0;

    // Initialize the windows api.
    InitializeWindows(screenWidth, screenHeight);

    // Create the input object.  This object will be used to handle reading the keyboard input from the user.
    m_InputHandler = new Win64_InputHandler();
    if (!m_InputHandler)
    {
        return false;
    }

    // Initialize the input object.
    m_InputHandler->Initialize();

    // Create the graphics object.  This object will handle rendering all the graphics for this application.
    m_Renderer = new D3D11Renderer();
    if (!m_Renderer)
    {
        return false;
    }

    // Initialize the graphics object.
    result = m_Renderer->Initialize(screenWidth, screenHeight, m_HWND);
    if (!result)
    {
        return false;
    }

    return true;
}

void Win64_ViewProvider::Shutdown()
{
    // Release the graphics object.
    if (m_Renderer)
    {
        m_Renderer->Shutdown();
        delete m_Renderer;
        m_Renderer = nullptr;
    }

    // Release the input object.
    if (m_InputHandler)
    {
        delete m_InputHandler;
        m_InputHandler = nullptr;
    }

    // Shutdown the window.
    ShutdownWindows();

    return;
}

void Win64_ViewProvider::Run()
{
    MSG msg;
    bool done, result;


    // Initialize the message structure.
    ZeroMemory(&msg, sizeof(MSG));

    // Loop until there is a quit message from the window or the user.
    done = false;
    while (!done)
    {
        // Handle the windows messages.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // If windows signals to end the application then exit out.
        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            // Otherwise do the frame processing.
            result = Frame();
            if (!result)
            {
                done = true;
            }
        }

    }

    return;
}

bool Win64_ViewProvider::Frame()
{
    bool result = false;;

    // Check if the user pressed escape and wants to exit the application.
    if (m_InputHandler->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    if (m_InputHandler->IsKeyDown(VK_F1))
    {
        m_Fullscreen = !m_Fullscreen;
    }

    // Do the frame processing for the graphics object.
    result = m_Renderer->Frame();
    if (!result)
    {
        return false;
    }

    return true;
}

LRESULT CALLBACK Win64_ViewProvider::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        // Check if a key has been pressed on the keyboard.
    case WM_KEYDOWN:
    {
        // If a key is pressed send it to the input object so it can record that state.
        m_InputHandler->KeyDown((unsigned int)wparam);
        return 0;
    }

    // Check if a key has been released on the keyboard.
    case WM_KEYUP:
    {
        // If a key is released then send it to the input object so it can unset the state for that key.
        m_InputHandler->KeyUp((unsigned int)wparam);
        return 0;
    }

    // Any other messages send to the default message handler as our application won't make use of them.
    default:
    {
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
    }
}

void Win64_ViewProvider::InitializeWindows(int& screenWidth, int& screenHeight)
{
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;


    // Get an external pointer to this object.
    g_ApplicationHandle = this;

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
    if (m_Fullscreen)
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
        posX = posY = 0;
    }
    else
    {
        // If windowed then set it to 800x600 resolution.
        screenWidth = 800;
        screenHeight = 600;

        // Place the window in the middle of the screen.
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // Create the window with the screen settings and get the handle to it.
    m_HWND = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName, m_ApplicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(m_HWND, SW_SHOW);
    SetForegroundWindow(m_HWND);
    SetFocus(m_HWND);

    // Hide the mouse cursor.
    ShowCursor(false);

    return;
}

void Win64_ViewProvider::ShutdownWindows()
{
    // Show the mouse cursor.
    ShowCursor(true);

    // Fix the display settings if leaving full screen mode.
    if (m_Fullscreen)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    // Remove the window.
    DestroyWindow(m_HWND);
    m_HWND = NULL;

    // Remove the application instance.
    UnregisterClass(m_ApplicationName, m_hInstance);
    m_hInstance = NULL;

    // Release the pointer to this class.
    g_ApplicationHandle = NULL;

    return;
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
        return g_ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
    }
    }
}