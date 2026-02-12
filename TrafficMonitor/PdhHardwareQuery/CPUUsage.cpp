#include "stdafx.h"
#include "CPUUsage.h"
#include "Common.h"
#include "TrafficMonitor.h"
#include <powerbase.h>
#include <sysinfoapi.h>


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
CPdhCPUUsage::CPdhCPUUsage()
    : CPdhQuery(theApp.m_win_version.GetMajorVersion() >= 10 ? _T("\\Processor Information(_Total)\\% Processor Utility") : _T("\\Processor Information(_Total)\\% Processor Time"))
{
}

bool CPdhCPUUsage::GetCPUUsage(int& cpu_usage)
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

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
CCPUUsage::CCPUUsage()
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
        if (!m_pdh_cup_usage_query.GetCPUUsage(cpu_usage))
        {
            cpu_usage = GetCpuUsageByGetSystemTimes();
            //写入日志
            //static bool write_log = false;
            //if (!write_log)
            //{
            //    CString str_log = CCommon::LoadTextFormat(IDS_GET_CPU_USAGE_BY_PDH_FAILED_LOG, { fullCounterPath });
            //    CCommon::WriteLog(str_log, theApp.m_log_path.c_str());
            //    write_log = true;
            //}
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
