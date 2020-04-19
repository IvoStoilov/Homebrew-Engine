#pragma once
#ifdef POP_PLATFORM_WINDOWS
#include <windows.h>
#endif //POP_PLATFORM_WINDOWS

struct Resolution
{
    s32 w = 0;
    s32 h = 0;
};

struct WindowData
{
    String m_WindowName = "New Window";
    Resolution m_WindowResolution;
    u32 m_PosX = 0;
    u32 m_PosY = 0;
    bool m_HasSystemMenu = false;
    bool m_HasBorder = false;
    bool m_ShowCursor = true;
};

struct Window
{
    WindowData m_Data;
    Resolution m_RenderingResolution;
#ifdef POP_PLATFORM_WINDOWS
    HWND m_WindowHandle = nullptr;
#else //POP_PLATFORM_WINDOWS
    #error Platform Not Supported!
#endif
};

