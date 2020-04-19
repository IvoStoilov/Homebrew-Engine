#pragma once
#include <system/viewprovider/windowcookie.h>
#include <system/viewprovider/window.h>
#include <system/singleton/singleton.h>

#ifdef POP_PLATFORM_WINDOWS
#include <windows.h>
#endif //POP_PLATFORM_WINDOWS

#define g_ViewProvider ViewProvider::GetInstance()

class ViewProvider
{
    POP_DECLARE_SINGLETON(ViewProvider)

public:
    void Update();

    WindowCookie ConstructWindow(const WindowData& windowData);
    inline Window& GetWindow(const WindowCookie& windowCookie) { return m_Windows[windowCookie.m_Index]; }
    void ClearChildWindow(WindowCookie& cookie);

    static Resolution GetMonitorResolution();
private:
    ViewProvider();
    ~ViewProvider();

    bool Initialize();
    void Shutdown();

    
    void InitializeInternal();
    void ShutdownInternal();

private:
    String m_ApplicationName;
    Array<Window> m_Windows;

//==============================================
//=============== Win64 ========================
//==============================================
#ifdef POP_PLATFORM_WINDOWS
public:
    HINSTANCE GetHInstnace() { return m_Win64_hInstnace; }
private:
    
    HINSTANCE m_Win64_hInstnace = nullptr;
#else //POP_PLATFORM_WINDOWS
    #error Platform Not Supported!
#endif
};