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

    inline float GetFrameTime() const { return m_FrameTime; }
    inline int64_t GetFrameStartTime() const { return m_StartTime; }
    float GetDeltaTime() const;

private:
    int64_t m_Frequency;
    float m_TicksPerMs;
    int64_t m_StartTime;
    float m_FrameTime;
};