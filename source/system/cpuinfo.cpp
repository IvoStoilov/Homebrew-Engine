#include <system/precompile.h>
#include "system/cpuinfo.h"

CPUInfo::CPUInfo()
{
    Initialize();
}

CPUInfo::~CPUInfo()
{
}

void CPUInfo::Initialize()
{
    m_CanReadCpu = true;

    PDH_STATUS status = PdhOpenQuery(nullptr, 0, &m_QueryHandle);
    if (status != ERROR_SUCCESS)
        m_CanReadCpu = false;

    status = PdhAddCounter(m_QueryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_CounterHandle);
    if (status != ERROR_SUCCESS)
        m_CanReadCpu = false;

    m_LastSampleTime = GetTickCount();
    m_CpuUsage = 0;
}

void CPUInfo::Shutdown()
{
    if (m_CanReadCpu)
        PdhCloseQuery(m_QueryHandle);
}

void CPUInfo::Update()
{
    PDH_FMT_COUNTERVALUE value;
    if (m_CanReadCpu)
    {
        if ((m_LastSampleTime + 1000) < GetTickCount())
        {
            m_LastSampleTime = GetTickCount();
            PdhCollectQueryData(m_QueryHandle);
            PdhGetFormattedCounterValue(m_CounterHandle, PDH_FMT_LONG, NULL, &value);
            m_CpuUsage = value.longValue;
        }
    }
}

int CPUInfo::GetCpuPercentage() const
{
    int usage;
    if (m_CanReadCpu)
    {
        usage = (int)m_CpuUsage;
    }
    else
    {
        usage = 0;
    }

    return usage;
}