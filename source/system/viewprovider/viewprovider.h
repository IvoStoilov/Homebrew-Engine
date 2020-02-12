#pragma once
#ifdef POP_PLATFORM_WINDOWS
#include <windows.h>
#endif //POP_PLATFORM_WINDOWS

class ViewProvider
{
public:
    struct WindowResolution
    {
        WindowResolution() = default;
        explicit WindowResolution(s32 w, s32 h) : m_Width(w), m_Height(h) {}
        
        s32 m_Width = 0;
        s32 m_Height = 0;
    };

public:
    static ViewProvider& GetInstance();
    static void CreateInstance();
    static void DestroyInstnace();

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

#ifdef POP_PLATFORM_WINDOWS
    HWND GetWindowHandle() { return m_Win64_HWND; }
    HINSTANCE GetHInstnace() { return m_Win64_hInstnace; }
private:
    HWND m_Win64_HWND;
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

private:
    static ViewProvider* sm_ViewProviderInstance;
};