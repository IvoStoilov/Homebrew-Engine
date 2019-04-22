#pragma once
#include <windows.h>
#include <stdint.h>

class Clock
{
public:
    Clock();
    ~Clock();

    bool Initialize();
    void Update();

    inline float GetFrameTimeInMs() const { return m_FrameTime; }
    inline float GetFrameTimeInS () const { return m_FrameTime / 1000.f; }
    inline int64_t GetFrameStartTime() const { return m_StartTime; }
    float GetDeltaTime() const;

private:
    int64_t m_Frequency;
    float m_TicksPerMs;
    int64_t m_StartTime;
    float m_FrameTime;
};