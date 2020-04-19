#pragma once
#include <graphics/common/rendertexture.h>
#include <graphics/common/gfxwindow/gfxwindow.h>
#include <system/singleton/singleton.h>

class D3D11;
class GraphicsNode;
class VisualComponent;
class ISubRenderer;
class DebugDisplayRenderer;
class RenderTexture;
class WindowHandle;
class RenderingEngine
{
    POP_DECLARE_SINGLETON(RenderingEngine);
    RenderingEngine();
    ~RenderingEngine();

public:
    bool Initialize();

    DebugDisplayRenderer* GetDebugDisplayRenderer() const;

    bool IsEnabledDebugDisplay() const;
    void EnableDebugDisplay(bool shouldEnable);

    void RegisterDrawable(VisualComponent* visComponent);
    void UnregisterDrawable(VisualComponent* visComponent);

    bool PreFrame();
    bool Frame(f32 dt);

    inline const SharedPtr<RenderTexture>& GetReflectionTexture() const { return m_ReflectionTexture; }
    inline SharedPtr<RenderTexture>& GetReflectionTexture() { return m_ReflectionTexture; }

    inline const SharedPtr<RenderTexture>& GetRefractionTexture() const { return m_RefractionTexture; }
    inline SharedPtr<RenderTexture>& GetRefractionTexture() { return m_RefractionTexture; }

    void RegisterGfxWindow(GfxWindow* gfxWindow) { m_ActiveWindows.push_back(gfxWindow); }
    void UnregisterGfxWindow(GfxWindow* gfxWindow);

    inline GfxWindow& GetGameWindow() { return m_GameWindow; }

    inline void SetViewMatrix(const mat4x4 viewMatrix) { m_ViewMatrix = viewMatrix; }
    inline void SetReflectedViewMatrix(const mat4x4 reflectedViewMatrix) { m_ReflectedViewMatrix = reflectedViewMatrix; }
    inline const f32 GetDT() const { return m_DT; }

private:
    void Shutdown();

    bool Render();
    bool RenderReflection();
    bool RenderRefractionTexture();

    enum SubRendererOrder
    {
        Skydome = 0,
        Terrain = 1,
        Water = 2,
        Text = 3,
        DebugDisplay = 4,
        ImGui = 5,

        COUNT,
        Invalid
    };

private:
    //Do we really need DT in the Render Engine?
    f32 m_DT = 0.f;

    D3D11* m_D3D = nullptr;
    mat4x4 m_ViewMatrix;
    mat4x4 m_ReflectedViewMatrix;

    Array<GraphicsNode*> m_Nodes;
    Array<ISubRenderer*> m_SubRenderers;
    Array<GfxWindow*> m_ActiveWindows;
    GfxWindow m_GameWindow;
    SharedPtr<RenderTexture> m_ReflectionTexture;
    SharedPtr<RenderTexture> m_RefractionTexture;
};

#define g_DebugDisplay RenderingEngine::GetInstance().GetDebugDisplayRenderer()
#define g_RenderEngine RenderingEngine::GetInstance()