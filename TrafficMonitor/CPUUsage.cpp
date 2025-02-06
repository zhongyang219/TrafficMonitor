#include "stdafx.h"
#include "CPUUsage.h"
#include "Common.h"
#include "TrafficMonitor.h"
#include <powerbase.h>
#include <sysinfoapi.h>

void CCPUUsage::SetUseCPUTimes(bool use_get_system_times)
{
    if (m_use_get_system_times != use_get_system_times)
    {
        m_use_get_system_times = use_get_system_times;
        m_first_get_CPU_utility = true;
    }
}

int CCPUUsage::GetCPUUsage()
{
    if (m_use_get_system_times)
        return GetCPUUsageByGetSystemTimes();
    else
        return GetCPUUsageByPdh();
}

int CCPUUsage::GetCPUUsageByGetSystemTimes()
{
    int cpu_usage{};
    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);

    __int64 idle = CCommon::CompareFileTime2(m_preidleTime, idleTime);
    __int64 kernel = CCommon::CompareFileTime2(m_prekernelTime, kernelTime);
    __int64 user = CCommon::CompareFileTime2(m_preuserTime, userTime);

    if (kernel + user == 0)
    {
        cpu_usage = 0;
    }
    else
    {
        //（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率
        cpu_usage = static_cast<int>(abs((kernel + user - idle) * 100 / (kernel + user)));
    }
    m_preidleTime = idleTime;
    m_prekernelTime = kernelTime;
    m_preuserTime = userTime;

    return cpu_usage;
}

int CCPUUsage::GetCPUUsageByPdh()
{
    int cpu_usage{};
    HQUERY hQuery;
    HCOUNTER hCounter;
    DWORD counterType;
    PDH_RAW_COUNTER rawData;

    PdhOpenQuery(NULL, 0, &hQuery);//开始查询
    const wchar_t* query_str{};
    if (theApp.m_win_version.GetMajorVersion() >= 10)
        query_str = L"\\Processor Information(_Total)\\% Processor Utility";
    else
        query_str = L"\\Processor Information(_Total)\\% Processor Time";
    PdhAddCounter(hQuery, query_str, NULL, &hCounter);
    PdhCollectQueryData(hQuery);
    PdhGetRawCounterValue(hCounter, &counterType, &rawData);

    if (m_first_get_CPU_utility) {//需要获得两次数据才能计算CPU使用率
        cpu_usage = 0;
        m_first_get_CPU_utility = false;
    }
    else {
        PDH_FMT_COUNTERVALUE fmtValue;
        PdhCalculateCounterFromRawValue(hCounter, PDH_FMT_DOUBLE, &rawData, &m_last_rawData, &fmtValue);//计算使用率
        cpu_usage = fmtValue.doubleValue;//传出数据
        if (cpu_usage > 100)
            cpu_usage = 100;
    }
    m_last_rawData = rawData;//保存上一次数据
    PdhCloseQuery(hQuery);//关闭查询
    return cpu_usage;
}

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

CCpuFreq::CCpuFreq()
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

    //初始化CPU频率查询
    Initialize();
}

CCpuFreq::~CCpuFreq()
{
    PdhCloseQuery(query);
}

bool CCpuFreq::Initialize()
{
    if (isInitialized)
        return true;

    PDH_STATUS status;
    //打开查询
    status = PdhOpenQuery(NULL, NULL, &query);
    if (status != ERROR_SUCCESS)
        return false;

    //添加计数器
    status = PdhAddCounterA(query, LPCSTR("\\Processor Information(_Total)\\% Processor Performance"), NULL, &counter);
    if (status != ERROR_SUCCESS)
    {
        PdhCloseQuery(query);
        query = nullptr;
        return false;
    }

    //初始化计数器
    PdhCollectQueryData(query);
    isInitialized = true;
    return true;
}

bool CCpuFreq::GetCpuFreq(float& freq)
{
    if (!isInitialized && !Initialize())
        return false;

    //从query中获取数据
    PdhCollectQueryData(query);
    PDH_FMT_COUNTERVALUE pdhValue;
    DWORD dwValue;
    PDH_STATUS status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
    if (status != ERROR_SUCCESS)
    {
        return false;
    }
    freq = pdhValue.doubleValue / 100 * max_cpu_freq;
    return true;
}
