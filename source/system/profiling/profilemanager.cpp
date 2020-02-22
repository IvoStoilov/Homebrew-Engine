#include <system/precompile.h>
#include "system/profiling/profilemanager.h"

std::string PROFILE_TEXTFILE_PATH = "Homebrew_ProfileInfo_dump.txt";

ProfileManager* ProfileManager::s_Instance = nullptr;

void ProfileInfo::UpdateInfo(uint64_t time)
{
    if (time < m_MinTime || m_CallCount == 0)
        m_MinTime = time;

    if (time > m_MaxTime)
        m_MaxTime = time;

    m_AvgTime = (m_AvgTime * m_CallCount + time) / (m_CallCount + 1);
    m_CallCount++;
}

void ProfileManager::CreateInstance()
{
    if (s_Instance == nullptr)
        s_Instance = new ProfileManager();
}

void ProfileManager::CleanInstance()
{
    if (s_Instance != nullptr)
    {
        delete s_Instance;
    }

}
bool ProfileManager::UpdateOrRegister(std::string name, long long time /*= 0ull*/)
{
    auto it = m_Repo.find(name);
    if (it == m_Repo.end())
    {
        ProfileInfo info;
        info.UpdateInfo(time);
        m_Repo[name] = info;
        return false;
    }
    it->second.UpdateInfo(time);
    return true;
}

void ProfileManager::GetInfo(const char* name) const
{
    GetInfo(std::string(name));
}

void ProfileManager::GetInfo(std::string& name) const
{
    auto it = m_Repo.find(name);
    if (it == m_Repo.end())
        printf("No data in Repository.\n");
    else
    {
        printf("%s\n", it->first.c_str());
        it->second.Print();
    }
}

void ProfileManager::DumpInfoToFile() const
{
    std::ofstream writer;
    writer.open(PROFILE_TEXTFILE_PATH, std::ofstream::out | std::ofstream::trunc);
    auto it = m_Repo.begin();
    for (it; it != m_Repo.end(); ++it)
    {
        writer << it->first << "\n";
        writer << it->second.ToString() << "\n";
    }
}

std::string ProfileInfo::ToString() const
{
    char buffer[256];
    sprintf_s(buffer, "Avarage Time : %.2f ms.\nMax Time     : %.2f ms.\nMin Time     : %.2f ms.\nCall count : %d\n\n",
        m_AvgTime, m_MaxTime, m_MinTime, m_CallCount);
    return std::string(buffer);
}