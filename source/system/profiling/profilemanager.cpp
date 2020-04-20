#include <system/precompile.h>
#include <system/profiling/profilemanager.h>

#ifdef POP_PROFILE_ENABLED

#include <system/string/stringhelper.h>

void ProfileManager::DumpInfoToJSON()
{
    String dateTime = StringHelper::FormatTime(std::chrono::system_clock::now());
    String fileName = StringHelper::Format("Profile_%s.json", dateTime.c_str());
    m_OutStream.open(fileName);
    popAssert(!m_OutStream.fail(), "Failed to open {}", fileName);
   
    WriteHeader();
    WriteProfile();
    WriteFooter();
    m_OutStream.close();
}

void ProfileManager::WriteHeader()
{
    m_OutStream << "{\"otherData\": {},\"traceEvents\":[";
    m_OutStream.flush();
}

void ProfileManager::WriteProfile()
{
    bool firstEntry = true;
    for (const ProfileInfo& info : m_ProfileInfosDuringSession)
    {
        if (!firstEntry)
        {
            m_OutStream << ",";
        }
        firstEntry = false;

        String name = info.m_Name;
        std::replace(name.begin(), name.end(), '"', '\'');

        Nanoseconds tsNS = info.m_Start.time_since_epoch();
        Microseconds tsUs = popDurationCast(Microseconds, tsNS);

        m_OutStream << "{";
        m_OutStream << "\"cat\":\"function\",";
        m_OutStream << "\"dur\":" << info.m_Duration.count() << ',';
        m_OutStream << "\"name\":\"" << name << "\",";
        m_OutStream << "\"ph\":\"X\",";
        m_OutStream << "\"pid\":0,";
        m_OutStream << "\"tid\":" << info.m_ThreadID << ",";
        m_OutStream << "\"ts\":" << tsUs.count();
        m_OutStream << "}";
    }

    m_OutStream.flush();
}

void ProfileManager::WriteFooter()
{
    m_OutStream << "]}";
    m_OutStream.flush();
}

void ProfileManager::BeginSession()
{
    m_ProfileInfosDuringSession.clear();
    m_IsSessionActive = true;
}

void ProfileManager::EndSession()
{
    m_IsSessionActive = false;
    DumpInfoToJSON();
}

void ProfileManager::Register(const ProfileInfo& info)
{
    popError(m_IsSessionActive, LogSystem, "Trying to register Profile info, during no active session");
    if (m_IsSessionActive)
    {
        m_ProfileInfosDuringSession.push_back(info);
    }
}

#endif //POP_PROFILE_ENABLED