#pragma once
#include "renderer/common/rendertexture.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3D11;
class GraphicsNode;
class VisualComponent;
class ISubRenderer;
class DebugDisplayRenderer;
class RenderTexture;
class D3D11Renderer
{
private:
    enum SubRendererOrder
    {
        Skydome      = 0,
        Terrain      = 1,
        Water        = 2,
        Text         = 3,
        DebugDisplay = 4,

        COUNT,
        Invalid
    };

    static D3D11Renderer* s_Instance;
    D3D11Renderer();
    ~D3D11Renderer();

    bool Initialize(HWND hwnd, u32 screenWidth, u32 screenHeight);
    void Shutdown();

public:
    DebugDisplayRenderer* GetDebugDisplayRenderer() const;

    static D3D11Renderer* GetInstance();
    static void CreateInstance(HWND hwnd, u32 screenWidth, u32 screenHeight);
    static void CleanInstance();

public:
    bool IsEnabledDebugDisplay() const;
    void EnableDebugDisplay(bool shouldEnable);

    void RegisterDrawable(VisualComponent* visComponent);
    void UnregisterDrawable(VisualComponent* visComponent);
    
    bool PreFrame();
    bool Frame();

    inline const SharedPtr<RenderTexture>& GetReflectionTexture() const { return m_ReflectionTexture; }
    inline SharedPtr<RenderTexture>& GetReflectionTexture() { return m_ReflectionTexture; }

private:
    bool Render();
    bool RenderReflection();
private:
    D3D11* m_D3D;

    Array<GraphicsNode*> m_Nodes;
    Array<ISubRenderer*> m_SubRenderers;
    SharedPtr<RenderTexture> m_ReflectionTexture;
};

#define g_DebugDisplay D3D11Renderer::GetInstance()->GetDebugDisplayRenderer()
#define g_RenderEngine D3D11Renderer::GetInstance()