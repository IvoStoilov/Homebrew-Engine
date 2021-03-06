#pragma once
#include <engine/input/inputhandler.h>
#include <engine/entitymodel/entity.h>

#include <system/fpscounter.h>
#include <system/cpuinfo.h>
#include <system/clock.h>
#include <system/singleton/singleton.h>

class Camera;
class Engine
{
    POP_DECLARE_SINGLETON(Engine);

    Engine();
    ~Engine();
public:
    bool Initialize();
    void Shutdown();

    inline bool          HasRequestedQuit() const { return m_HasRequestedQuit; }
    inline const Camera* GetCamera()        const { return m_Camera.get(); }
    inline int32_t       GetCPUUsage()      const { return m_CPUInfo.GetCpuPercentage(); }
    inline int32_t       GetFPS()           const { return m_FPSCounter.GetFPS(); }
    inline unsigned long GetFrameNumber()   const { return m_FPSCounter.GetFrameNumber(); }


    void Update();
    void GetCameraViewMatrix(mat4x4& outMatrix);

private:
    void EndFrame();
#ifdef POP_IMGUI_ENABLED
 //   void ImGui_DisplayFPS();
#endif //POP_IMGUI_ENABLED
private:
    UniquePtr<Camera> m_Camera;
    Array<Entity*> m_Entities;
    
    InputHandler m_InputHandler;

    FrameTime m_FrameTime;
    FPSCounter m_FPSCounter;
    CPUInfo m_CPUInfo;
    
    bool m_HasRequestedQuit = false;
};

#define g_Engine Engine::GetInstance()