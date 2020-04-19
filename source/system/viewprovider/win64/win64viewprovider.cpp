#include <system/precompile.h>
#include <system/viewprovider/viewprovider.h>
#include <system/commandline/commandlineoptions.h>

#ifdef POP_PLATFORM_WINDOWS

// Forward declare message handler from imgui_impl_win32.cpp
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
        return true;

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

WindowCookie ViewProvider::ConstructWindow(const WindowData& windowData)
{
    Window& window = m_Windows.emplace_back();
    window.m_Data = windowData;
    
    DWORD style = 0l;
    style |= WS_CLIPSIBLINGS;
    style |= WS_CLIPCHILDREN;
    style |= WS_POPUP;
    style |= WS_CAPTION;
    style |= (windowData.m_HasBorder) ? WS_BORDER : 0l;
    style |= (windowData.m_HasSystemMenu) ? WS_SYSMENU : 0l;

    constexpr HWND NULL_PARENT = nullptr;
    constexpr HMENU NULL_MENU = nullptr;
    constexpr LPVOID NULL_PARAM = nullptr;
    window.m_WindowHandle = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName.c_str(), windowData.m_WindowName.c_str(),
        style, windowData.m_PosX, windowData.m_PosY, windowData.m_WindowResolution.w, windowData.m_WindowResolution.h,
        NULL_PARENT, NULL_MENU, m_Win64_hInstnace, NULL_PARAM);

    popAssert(window.m_WindowHandle, "Child Window was not created. Error : {}", GetLastError());

    RECT clientRect;
    GetClientRect(window.m_WindowHandle, &clientRect);
    window.m_RenderingResolution.w = clientRect.right;
    window.m_RenderingResolution.h = clientRect.bottom;
    
    ShowWindow(window.m_WindowHandle, SW_SHOW);
    SetForegroundWindow(window.m_WindowHandle);
    SetFocus(window.m_WindowHandle);
 
    return WindowCookie(static_cast<u32>(m_Windows.size() - 1));
}

void ViewProvider::InitializeInternal()
{
    static constexpr LPCSTR NULLPTR_MODULENAME = nullptr;
    m_Win64_hInstnace = GetModuleHandle(NULLPTR_MODULENAME);

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
    wc.lpszClassName = m_ApplicationName.c_str();
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);
}

void ViewProvider::ShutdownInternal()
{
    for (Window& window : m_Windows)
    {
        DestroyWindow(window.m_WindowHandle);
        window.m_WindowHandle = nullptr;
    }
    m_Windows.clear();

    UnregisterClass(m_ApplicationName.c_str(), m_Win64_hInstnace);
    m_Win64_hInstnace = nullptr;
}

void ViewProvider::Update()
{
    popProfile(ViewProvider::Update);
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

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

Resolution ViewProvider::GetMonitorResolution()
{
    return Resolution{ GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
}                                         

#endif //POP_PLATFORM_WINDOWS