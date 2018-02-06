#pragma once
#include <stdint.h>

class FPSCounter
{
public:
    FPSCounter();
    ~FPSCounter();

    void Initialize();
    void Update();
    inline int32_t GetFPS() const { return m_FPS; }
    inline unsigned long GetFrameNumber() const { return m_FrameNumber; }

private:
    int32_t m_FPS;
    int32_t m_Count;
    unsigned long m_FrameNumber;
    unsigned long m_StartTime;
};