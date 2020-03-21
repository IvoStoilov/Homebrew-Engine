#pragma once
#include <system/viewprovider/windowcookie.h>
#include <system/singleton/singleton.h>

#ifdef POP_PLATFORM_WINDOWS
#include <windows.h>
#endif //POP_PLATFORM_WINDOWS

#define g_ViewProvider ViewProvider::GetInstance()

class ViewProvider
{
    POP_DECLARE_SINGLETON(ViewProvider)

public:
    struct WindowResolution
    {
        WindowResolution() = default;
        explicit WindowResolution(s32 w, s32 h) : m_Width(w), m_Height(h) {}
        
        s32 m_Width = 0;
        s32 m_Height = 0;
    };

public:
    ViewProvider();
    ~ViewProvider();

    bool Initialize();
    void Shutdown();
    void Update();

    inline const WindowResolution& GetWindowResolution() const { return m_WindowResolution; }
    inline u32 GetWindowWidth() const { return m_WindowResolution.m_Width; }
    inline u32 GetWindowHeight() const { return m_WindowResolution.m_Height; }
    inline u32 GetWindowPosX() const { return m_WindowPosX; }
    inline u32 GetWindowPosY() const { return m_WindowPosY; }

    void ActivateFullscreen();
    void DeactivateFullscreen();

    WindowCookie CreateChildWindow();
    void ClearChildWindow(WindowCookie& cookie);
#ifdef POP_PLATFORM_WINDOWS
    HWND GetWindowHandle() { return m_Win64_HWND; }
    HWND GetChildWindow(const WindowCookie& windowCookie) { return m_ChildWindows[windowCookie.m_Index]; }
    HINSTANCE GetHInstnace() { return m_Win64_hInstnace; }
private:
    HWND m_Win64_HWND;
    Array<HWND> m_ChildWindows;
    HINSTANCE m_Win64_hInstnace;
#else //POP_PLATFORM_WINDOWS
    #error Platform Not Supported!
#endif
protected:
    WindowResolution GetMonitorResolution();
    bool InitializeInternal();
    void ShutdownInternal();

    void SetDefaultPositionAndResolution();

    
protected:
    String m_ApplicationName;
    
    u32 m_WindowPosX = 0;
    u32 m_WindowPosY = 0;

    WindowResolution m_WindowResolution;
};