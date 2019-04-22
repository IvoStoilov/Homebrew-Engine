#pragma once
#include <vector>
#include <d3dx10math.h>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3D11;
class GraphicsNode;
class VisualComponent;
class ISubRenderer;
class DebugDisplayRenderer;
class D3D11Renderer
{
private:
    //temporary - should make debug display singleton
    DebugDisplayRenderer* m_DebugDisplayRendererCache;

    static D3D11Renderer* s_Instance;
    D3D11Renderer();
    ~D3D11Renderer();

    bool Initialize(HWND hwnd, uint32_t screenWidth, uint32_t screenHeight);
    void Shutdown();

public:
    DebugDisplayRenderer* GetDebugDisplayRenderer() { return m_DebugDisplayRendererCache; }

    static D3D11Renderer* GetInstance();
    static void CreateInstance(HWND hwnd, uint32_t screenWidth, uint32_t screenHeight);
    static void CleanInstance();

public:
    void RegisterDrawable(VisualComponent* visComponent);
    void UnregisterDrawable(VisualComponent* visComponent);
    
    bool PreFrame();
    bool Frame();

private:
    bool Render();

private:
    D3D11* m_D3D;

    std::vector<GraphicsNode*> m_Nodes;
    std::vector<ISubRenderer*> m_SubRenderers;
  
    D3DXMATRIX m_ViewMatrix;
    D3DXMATRIX m_ProjectionMatrix;
    D3DXMATRIX m_WorldMatrix;
};

#define g_DebugDisplay D3D11Renderer::GetInstance()->GetDebugDisplayRenderer()
#define g_RenderEngine D3D11Renderer::GetInstance()