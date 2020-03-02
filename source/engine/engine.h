#pragma once
#include <engine/input/inputhandler.h>
#include <engine/entitymodel/entity.h>

#include <system/fpscounter.h>
#include <system/cpuinfo.h>
#include <system/clock.h>

class Camera;
class Engine
{
private:
    static Engine* s_Instance;
    Engine();
    ~Engine();

public:
    static Engine* GetInstance();
    static void CreateInstance();
    static void CleanInstnace();

public:
    bool Initialize();
    void Shutdown();

    inline bool          HasRequestedQuit() const { return m_HasRequestedQuit; }
    inline const Camera* GetCamera()        const { return m_Camera; }
    inline float         GetFrameTimeInS()  const { return m_WorldClock.GetFrameTimeInS(); }
    inline float         GetFrameTimeInMS() const { return m_WorldClock.GetFrameTimeInMs(); }
    inline int32_t       GetCPUUsage()      const { return m_CPUInfo.GetCpuPercentage(); }
    inline int32_t       GetFPS()           const { return m_FPSCounter.GetFPS(); }
    inline unsigned long GetFrameNumber()   const { return m_FPSCounter.GetFrameNumber(); }


    void Update();
    void GetCameraViewMatrix(mat4x4& outMatrix);


private:
    Camera* m_Camera;
    std::vector<Entity*> m_Entities;
    
    InputHandler m_InputHandler;

    FPSCounter m_FPSCounter;
    CPUInfo m_CPUInfo;
    Clock m_WorldClock;

    bool m_HasRequestedQuit;
};

#define g_Engine Engine::GetInstance()