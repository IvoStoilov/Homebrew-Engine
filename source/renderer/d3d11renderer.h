#pragma once
#include <vector>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3D11;
class GraphicsNode;
class Model2D;
class TextureShader;
class VisualComponent;
class Text;
class D3D11Renderer
{
private:
    static D3D11Renderer* s_Instance;
    D3D11Renderer();
    ~D3D11Renderer();

    bool Initialize(HWND hwnd, uint32_t screenWidth, uint32_t screenHeight);
    void Shutdown();

public:
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
    //std::vector<GraphicsNode*> m_2DNodes;

    //temp hack
    Model2D* m_2DModel;
    TextureShader* m_TextureShader;
    Text* m_Text;

    D3DXMATRIX m_ViewMatrix;
    D3DXMATRIX m_ProjectionMatrix;
    D3DXMATRIX m_WorldMatrix;
};

#define g_RenderEngine D3D11Renderer::GetInstance()