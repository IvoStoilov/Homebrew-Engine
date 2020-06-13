#pragma once
#include <system/singleton/singleton.h>
#include <system/viewprovider/windowcookie.h>

class GfxWindow;
class D3D11;
class RenderingEngine
{
    POP_DECLARE_SINGLETON(RenderingEngine);
    RenderingEngine();
    ~RenderingEngine();

public:
    void Update(f32 dt);
    GfxWindow& GetGameWindow() { popAssert(m_GameWindow, "No Game Window Created"); return *m_GameWindow.get(); }
    const WindowCookie& GetGameWindowCookie() const;

private:
    void Initialize();
    void Shutdown();

private:
    UniquePtr<D3D11> m_D3D;
    UniquePtr<GfxWindow> m_GameWindow;
};

#define g_RenderEngine RenderingEngine::GetInstance()