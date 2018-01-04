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

class D3D11Renderer
{
public:
    D3D11Renderer();
    ~D3D11Renderer();

    bool Initialize(int, int, HWND);
    void Shutdown();

    //TODO (istoilov) : refactor after implementing the component system
    void RegisterEntity(Entity* entity);
    void UnregisterEntity(Entity* entity);
    
    void Register2DEntity(Entity* entity);

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

    D3DXMATRIX m_ViewMatrix;
    D3DXMATRIX m_ProjectionMatrix;
    D3DXMATRIX m_WorldMatrix;
};