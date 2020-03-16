#pragma once
#ifdef POP_PROFILE_ENABLED
struct ProfileInfo
{
    explicit ProfileInfo(const String& name, s64 start, s64 end, u32 threadID) :
        m_Name(name),
        m_Start(start),
        m_End(end),
        m_ThreadID(threadID)
    {}

    const String m_Name = "";
    const s64 m_Start = 0;
    const s64 m_End = 0;
    const u32 m_ThreadID = 0u;
};

class ProfileManager
{
public:
    static ProfileManager& GetInstance() { return *s_Instance; }
    static void CreateInstance();
    static void CleanInstance();

    inline bool IsSessionActive() const { return m_IsSessionActive; }
    void BeginSession();
    void EndSession();

    void Register(const ProfileInfo& info);
    
protected:
    void DumpInfoToJSON();
    void WriteHeader();
    void WriteProfile();
    void WriteFooter();
protected:
    ProfileManager() {}
    static ProfileManager* s_Instance;

    bool m_IsSessionActive = false;
    std::ofstream m_OutStream;
    Array<ProfileInfo> m_ProfileInfosDuringSession;
};

#define g_ProfileManager ProfileManager::GetInstance()

struct ProfilingObject
{
    const char* m_Name;
    std::chrono::microseconds m_Start;
    std::chrono::microseconds m_End;

    ProfilingObject(const char* name) :
        m_Name(name)
    {
        m_Start = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
        m_End = m_Start;
    }
    ~ProfilingObject()
    {
        if (g_ProfileManager.IsSessionActive())
        {
            m_End = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
            u32 threadID = static_cast<u32>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            ProfileInfo result(m_Name, m_Start.count(), m_End.count(), threadID);
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



