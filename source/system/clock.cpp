#include <system/precompile.h>
#include <system/clock.h>

void FrameTime::BeginFrame()
{
    m_FrameEnd = FrameTime::GetNow();
    m_DT = popDurationCast(Seconds, m_FrameEnd - m_FrameBegin);
    m_FrameBegin = m_FrameEnd;
}

Milliseconds FrameTime::GetCurrentFrameElapsedTime() const
{
    TimePoint now = FrameTime::GetNow();
    return popDurationCast(Milliseconds, now - m_FrameBegin);
}

Clock::Clock()
{
    Initialize();
}

Clock::~Clock()
{
}

bool Clock::Initialize()
{
    QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency);
    if (m_Frequency == 0)
        return false;

    m_TicksPerMs = (float)(m_Frequency / 1000.f);
    QueryPerformanceCounter((LARGE_INTEGER*)&m_StartTime);

    return true;
}

void Clock::Update()
{
    int64_t currentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

    float timeDifference = (float)(currentTime - m_StartTime);
    m_FrameTime = timeDifference / m_TicksPerMs;

    m_StartTime = currentTime;
}

float Clock::GetDeltaTime() const
{
    int64_t currentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
    return ((float)(currentTime - m_StartTime) / m_TicksPerMs);
}

