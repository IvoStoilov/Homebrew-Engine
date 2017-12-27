#pragma once
#include <vector>
#include <stdint.h>
#include <windows.h>

#include "entitymodel/entity.h"

class D3D11Renderer;
class Camera;
class Engine
{
private:
    static Engine* s_Instance;
    Engine();
    ~Engine();

public:
    static void CreateInstance(HINSTANCE hInstance, HWND hwnd, uint32_t width, uint32_t height, int32_t windowPosX, int32_t windowPosY);
    static Engine* GetInstance();
    static void CleanInstnace();

public:
    bool Initialize(HINSTANCE hInstance, HWND hwnd, uint32_t width, uint32_t height, int32_t windowPosX, int32_t windowPosY);
    void Shutdown();

    inline bool HasRequestedQuit() const { return m_HasRequestedQuit; }
    void GetCameraViewMatrix(mat4x4& outMatrix);

    void Update();

private:
    D3D11Renderer* m_Renderer;
    Camera* m_Camera;
    std::vector<Entity*> m_Entities;

    bool m_HasRequestedQuit;
};

#define g_Engine Engine::GetInstance()