#include "stdafx.h"
#include "CPUUsage.h"
#include "Common.h"
#include "TrafficMonitor.h"
#include <powerbase.h>
#include <sysinfoapi.h>

CPdhQuery::CPdhQuery(LPCTSTR _fullCounterPath)
    : fullCounterPath(_fullCounterPath)
{
    Initialize();
}

CPdhQuery::~CPdhQuery()
{
    //关闭查询
    PdhCloseQuery(query);
}

bool CPdhQuery::Initialize()
{
    if (isInitialized)
        return true;

    PDH_STATUS status;
    //打开查询
    status = PdhOpenQuery(NULL, NULL, &query);
    if (status != ERROR_SUCCESS)
        return false;

    //添加计数器
    status = PdhAddEnglishCounter(query, fullCounterPath.GetString(), NULL, &counter);
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

bool CPdhQuery::QueryValue(double& value)
{
    if (!isInitialized)
        return false;

    //更新数据
    PdhCollectQueryData(query);
    PDH_FMT_COUNTERVALUE pdhValue;
    DWORD dwValue;
    PDH_STATUS status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, &dwValue, &pdhValue);
    if (status != ERROR_SUCCESS)
    {
        return false;
    }
    value = pdhValue.doubleValue;
    return true;
}


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
CCPUUsage::CCPUUsage()
    : CPdhQuery(theApp.m_win_version.GetMajorVersion() >= 10 ? _T("\\Processor Information(_Total)\\% Processor Utility") : _T("\\Processor Information(_Total)\\% Processor Time"))
{
}

void CCPUUsage::SetUseCPUTimes(bool use_get_system_times)
{
    if (m_use_get_system_times != use_get_system_times)
    {
        m_use_get_system_times = use_get_system_times;
    }
}

int CCPUUsage::GetCpuUsage()
{
    int cpu_usage{};
    if (m_use_get_system_times)
    {
        cpu_usage = GetCpuUsageByGetSystemTimes();
    }
    else
    {
        //如果通过pdh获取CPU利用率失败，采用GetSystemTimes获取
        if (!GetCPUUsageByPdh(cpu_usage))
        {
            cpu_usage = GetCpuUsageByGetSystemTimes();
            //写入日志
            static bool write_log = false;
            if (!write_log)
            {
                CString str_log = CCommon::LoadTextFormat(IDS_GET_CPU_USAGE_BY_PDH_FAILED_LOG, { fullCounterPath });
                CCommon::WriteLog(str_log, theApp.m_log_path.c_str());
                write_log = true;
            }
        }
    }
    return cpu_usage;
}

int CCPUUsage::GetCpuUsageByGetSystemTimes()
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

bool CCPUUsage::GetCPUUsageByPdh(int& cpu_usage)
{
    double value{};
    if (QueryValue(value))
    {
        cpu_usage = static_cast<int>(value);
        if (cpu_usage > 100)
            cpu_usage = 100;
        return true;
    }
    return false;
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

bool CCpuFreq::GetCpuFreq(float& freq)
{
    double value{};
    if (QueryValue(value))
    {
        freq = value / 100 * max_cpu_freq;
        return true;
    }
    return false;
}
