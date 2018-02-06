#pragma once
#pragma comment(lib, "pdh.lib")
#include <pdh.h>

class CPUInfo
{
public:
    CPUInfo();
    ~CPUInfo();

    void Initialize();
    void Shutdown();
    void Update();
    int GetCpuPercentage() const;

private:
    bool m_CanReadCpu;
    HQUERY m_QueryHandle;
    HCOUNTER m_CounterHandle;
    unsigned long m_LastSampleTime;
    long m_CpuUsage;
};
