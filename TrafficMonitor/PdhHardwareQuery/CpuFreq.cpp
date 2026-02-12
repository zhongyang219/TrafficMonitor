#include "stdafx.h"
#include "CpuFreq.h"
#include <powerbase.h>
#include <sysinfoapi.h>


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

typedef struct _PROCESSOR_POWER_INFORMATION {
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, * PPROCESSOR_POWER_INFORMATION;

CPdhCpuFreq::CPdhCpuFreq()
    : CPdhQuery(_T("\\Processor Information(_Total)\\% Processor Performance"))
{
    //获取max_cpu_freq
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    auto ppInfo = std::vector<PROCESSOR_POWER_INFORMATION>(si.dwNumberOfProcessors);
    auto status = CallNtPowerInformation(POWER_INFORMATION_LEVEL::ProcessorInformation,
        NULL, 0, &ppInfo[0], sizeof(PROCESSOR_POWER_INFORMATION) * ppInfo.size());
    for (size_t i = 0; i < ppInfo.size(); i++)
    {
        max_cpu_freq = max(max_cpu_freq, ppInfo[i].MaxMhz / 1000.f);
    }
}

bool CPdhCpuFreq::GetCpuFreq(float& freq)
{
    double value{};
    if (QueryValue(value))
    {
        freq = value / 100 * max_cpu_freq;
        return true;
    }
    return false;
}
