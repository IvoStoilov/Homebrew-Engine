#pragma comment(lib, "winmm.lib")

#include "system/fpscounter.h"

#include <windows.h>
#include <mmsystem.h>

FPSCounter::FPSCounter()
{
    Initialize();
}

FPSCounter::~FPSCounter()
{
}

void FPSCounter::Initialize()
{
    m_FPS = 0;
    m_Count = 0;
    m_StartTime = timeGetTime();
}

void FPSCounter::Update()
{
    m_Count++;
    m_FrameNumber++;
    if (timeGetTime() >= (m_StartTime + 1000))
    {
        m_FPS = m_Count;
        m_Count = 0;

        m_StartTime = timeGetTime();
    }
}
