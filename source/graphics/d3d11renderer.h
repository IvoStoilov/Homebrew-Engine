#pragma once
#include <graphics/common/rendertexture.h>

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
class WindowHandle;
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

    bool Initialize();
    void Shutdown();

public:
    DebugDisplayRenderer* GetDebugDisplayRenderer() const;

    static D3D11Renderer* GetInstance();
    static void CreateInstance();
    static void CleanInstance();

public:
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

    inline void SetViewMatrix(const mat4x4 viewMatrix) { m_ViewMatrix = viewMatrix; }
    inline void SetReflectedViewMatrix(const mat4x4 reflectedViewMatrix) { m_ReflectedViewMatrix = reflectedViewMatrix; }
    inline const f32 GetDT() const { return m_DT; }

private:
    bool Render();
    bool RenderReflection();
    bool RenderRefractionTexture();
private:
    //Do we really need DT in the Render Engine?
    f32 m_DT;

    D3D11* m_D3D;
    mat4x4 m_ViewMatrix;
    mat4x4 m_ReflectedViewMatrix;

    Array<GraphicsNode*> m_Nodes;
    Array<ISubRenderer*> m_SubRenderers;
    SharedPtr<RenderTexture> m_ReflectionTexture;
    SharedPtr<RenderTexture> m_RefractionTexture;
};

#define g_DebugDisplay D3D11Renderer::GetInstance()->GetDebugDisplayRenderer()
#define g_RenderEngine D3D11Renderer::GetInstance()