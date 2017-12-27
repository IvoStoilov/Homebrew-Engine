#pragma once
#include <windows.h>
#include <stdint.h>

class Win64_ViewProvider
{
public:
    Win64_ViewProvider();
    ~Win64_ViewProvider();

    bool Initialize();
    void Shutdown();
    void Run();

    inline void SetFullscreen(const bool value) { m_Fullscreen = value; }
    inline bool GetIsFullscreen() const { return m_Fullscreen; }

private:
    void InitializeWindows(int& screenWidth, int& screenHeight);
    void ShutdownWindows();

private:
    LPCSTR m_ApplicationName;
    HINSTANCE m_hInstance;
    HWND m_HWND;

    bool m_Fullscreen;

    int32_t m_WindowPosX;
    int32_t m_WindowPosY;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
