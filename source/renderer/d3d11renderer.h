#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3D11;
class D3D11Model;
class D3D11Shader;
class TextureShader;
class Camera;
class D3D11Renderer
{
public:
    D3D11Renderer();
    ~D3D11Renderer();

    bool Initialize(int, int, HWND);
    void Shutdown();
    bool Frame();

private:
    bool Render();

private:
    D3D11* m_D3D;
    D3D11Model* m_Model;
    D3D11Shader* m_Shader;
    TextureShader* m_TextureShader;
    Camera* m_Camera;
};