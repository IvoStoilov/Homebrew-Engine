#pragma once
#include <chrono>
#include <stdio.h>
#include <string.h>
#include <map>

class ProfileInfo
{
protected:
    double m_AvgTime;
    double m_MaxTime;
    double m_MinTime;
    uint32_t m_CallCount;
    
    friend class ProfileManager;
    static ProfileInfo Empty() { return ProfileInfo(); }
public:
    ProfileInfo() :
        m_AvgTime(0), m_MaxTime(0),
        m_MinTime(0), m_CallCount(0)
    {}

    void UpdateInfo(uint64_t time);
    void Print() const
    {
        printf("Avarage Time : %.2f ms.\nMax Time     : %.2f ms.\nMin Time     : %.2f ms.\nCall count : %d\n\n", 
            m_AvgTime, m_MaxTime, m_MinTime, m_CallCount);
    }
    std::string ToString() const;
};


class ProfileManager
{
protected:
    std::map<std::string, ProfileInfo> m_Repo;
    static ProfileManager* s_Instance;
    ProfileManager()
    {}

public:
    static ProfileManager& GetInstance() { return *s_Instance; }
    static void CreateInstance();
    static void CleanInstance();

    bool UpdateOrRegister(std::string name, const long long time = 0ull);

    void GetInfo(std::string& name) const;
    void GetInfo(const char* name) const;
    void DumpInfoToFile() const;
};

#define g_ProfileManager ProfileManager::GetInstance()

#define CONCAT(A, B) A##B

#define PROFILE_MANAGER_INIT      																		\
    ProfileManager::Initialize();	

#define PROFILE_MANAGER_UPDATE_OR_REGISTER(NAME, TIME)													\
    g_ProfileManager.UpdateOrRegister(#NAME, TIME);

#define PROFILE_CODEREGION_START(NAME)																	\
    std::chrono::milliseconds ms_start_##NAME = std::chrono::duration_cast<std::chrono::milliseconds>	\
    (std::chrono::system_clock::now().time_since_epoch());

#define PROFILE_CODEREGION_END(NAME)																	\
    std::chrono::milliseconds ms_end_##NAME = std::chrono::duration_cast<std::chrono::milliseconds>		\
    (std::chrono::system_clock::now().time_since_epoch());												\
    uint64_t TIME = (ms_end_##NAME).count() - (ms_start_##NAME).count();							    \
    PROFILE_MANAGER_UPDATE_OR_REGISTER(NAME, TIME)														\
    /*printf("PROFILE_CODEREGION::%s : %lld ms.\n", #NAME, TIME)*/;

struct ProfilingObject
{
    const char* m_Name;
    std::chrono::milliseconds ms_start;
    std::chrono::milliseconds ms_end;

    ProfilingObject(const char* name) :
        m_Name(name)
    {
        ms_start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    }
    ~ProfilingObject()
    {
        ms_end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        const long long time = ms_end.count() - ms_start.count();

        g_ProfileManager.UpdateOrRegister(m_Name, time);

        /*printf("PROFILE_FUNCTION::%s : %lld ms.\n", m_Name, time);*/
    }
};

#define PROFILE_FUNCTION(NAME) ProfilingObject p_obj(#NAME);


