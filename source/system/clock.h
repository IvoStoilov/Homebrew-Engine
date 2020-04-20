#pragma once

using Nanoseconds  = std::chrono::nanoseconds;
using Microseconds = std::chrono::duration<s64, std::micro>;
using Milliseconds = std::chrono::duration<f32, std::milli>;
using Seconds      = std::chrono::duration<f32>;

using SteadyClock = std::chrono::steady_clock;
using TimePoint   = std::chrono::time_point<SteadyClock>;

#define popDurationCast(durationType, value) std::chrono::duration_cast<durationType>((value))

class FrameTime
{
public:
    static TimePoint GetNow() { return SteadyClock::now(); }
    void BeginFrame();
    
    Seconds GetDT() const { return m_DT; }
    Milliseconds GetCurrentFrameElapsedTime() const;
    
private:
    TimePoint m_FrameBegin;
    TimePoint m_FrameEnd;
    Seconds m_DT;
};

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