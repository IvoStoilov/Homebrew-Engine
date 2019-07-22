#pragma once
#include <windows.h>

class Win64_ViewProvider
{
public:
    Win64_ViewProvider();
    ~Win64_ViewProvider();

    bool Initialize();
    void Shutdown();
    void Run();

private:
    void InitializeWindows(int& screenWidth, int& screenHeight);
    void ShutdownWindows();

private:
    LPCWSTR m_ApplicationName;
    HINSTANCE m_hInstance;
    HWND m_HWND;

    u32 m_WindowPosX;
    u32 m_WindowPosY;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
