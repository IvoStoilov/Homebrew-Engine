#pragma once
#include <system/singleton/singleton.h>
#include <system/clock.h>

#ifdef POP_PROFILE_ENABLED
struct ProfileInfo
{
    explicit ProfileInfo(const String& name, TimePoint start, Microseconds duration, u32 threadID) :
        m_Name(name),
        m_Start(start),
        m_Duration(duration),
        m_ThreadID(threadID)
    {}

    const String m_Name = "";
    const TimePoint m_Start;
    const Microseconds m_Duration;
    const u32 m_ThreadID = 0u;
};

class ProfileManager
{
    POP_DECLARE_SINGLETON(ProfileManager);

public:
    inline bool IsSessionActive() const { return m_IsSessionActive; }
    void BeginSession();
    void EndSession();

    void Register(const ProfileInfo& info);
    
protected:
    ProfileManager() = default;

    void DumpInfoToJSON();
    void WriteHeader();
    void WriteProfile();
    void WriteFooter();
protected:
    bool m_IsSessionActive = false;
    std::ofstream m_OutStream;
    Array<ProfileInfo> m_ProfileInfosDuringSession;
};

#define g_ProfileManager ProfileManager::GetInstance()

struct ProfilingObject
{
    const char* m_Name;
    TimePoint m_Start;
    TimePoint m_End;

    ProfilingObject(const char* name) :
        m_Name(name)
    {
        m_Start = FrameTime::GetNow();
        m_End = m_Start;
    }
    ~ProfilingObject()
    {
        if (g_ProfileManager.IsSessionActive())
        {
            m_End = FrameTime::GetNow();
            Microseconds duration = popDurationCast(Microseconds, m_End - m_Start);
            u32 threadID = static_cast<u32>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            ProfileInfo result(m_Name, m_Start, duration, threadID);
            g_ProfileManager.Register(result);
        }
    }
};
#endif //POP_PROFILE_ENABLED

#ifdef POP_PROFILE_ENABLED
#define popProfile(NAME) ProfilingObject pObj##__LINE__(#NAME);
#else
#define popProfile(NAME)
#endif //POP_PROFILE_ENABLED



