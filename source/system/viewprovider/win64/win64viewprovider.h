#pragma once
#ifdef POP_PLATFORM_WINDOWS

#include <system/viewprovider/viewprovider.h>

#include <windows.h>

class Win64WindowHandle : public WindowHandle
{
public:
    virtual void ReleaseHandle() override;
    inline HWND& GetWin64WindowHandle() { return m_HWND; }

protected:
    HWND m_HWND = nullptr;
};

class Win64ViewProvider : public ViewProvider
{
public:
    virtual bool InitializeInternal() override;
    virtual void ShutdownInternal() override;
    virtual void Update() override;

    HWND& GetWin64WindowHandle() { return static_cast<Win64WindowHandle*>(m_WindowHandle)->GetWin64WindowHandle(); }
    HINSTANCE& GetWin64hInstance() { return m_hInstance; }
    
    virtual void ActivateFullscreen() override;
    virtual void DeactivateFullscreen() override;

protected:
    virtual WindowResolution GetMonitorResolution() override;
    
protected:
    HINSTANCE m_hInstance;
    
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif //POP_PLATFORM_WINDOWS