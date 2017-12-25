#pragma once
#include <windows.h>
#include "renderer/d3d11renderer.h"
#include "win64/win64input.h"

//*******************
//*** DEPRICATED ****
//*******************

class Win64_ViewProvider
{
public:
    Win64_ViewProvider();
    ~Win64_ViewProvider();

    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

    inline void SetFullscreen(const bool value) { m_Fullscreen = value; }
    inline bool GetIsFullscreen() const { return m_Fullscreen; }

private:
    void InitializeWindows(int& screenWidth, int& screenHeight);
    void ShutdownWindows();
    
    bool Frame();

private:
    LPCSTR m_ApplicationName;
    HINSTANCE m_hInstance;
    HWND m_HWND;

    Win64_InputHandler* m_InputHandler;
    D3D11Renderer* m_Renderer;

    bool m_Fullscreen;

    int32_t m_WindowPosX;
    int32_t m_WindowPosY;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


static Win64_ViewProvider* g_ApplicationHandle = 0;