#pragma once
#include <Pdh.h>
#include <PdhMsg.h>
#include "PdhQuery.h"

class CPdhCPUUsage : public CPdhQuery
{
public:
    CPdhCPUUsage();

    ~CPdhCPUUsage()
    {
    }

    bool GetCPUUsage(int& cpu_usage);

};

//////////////////////////////////////////////////////////////////////////////////
class CCPUUsage
{
public:
    CCPUUsage();

    ~CCPUUsage()
    {}

    void SetUseCPUTimes(bool use_get_system_times);		//设置获取CPU利用率的方式，是通过GetSystemTimes还是Pdh
    int GetCpuUsage();

private:
    int GetCpuUsageByGetSystemTimes();

private:
    bool m_use_get_system_times{ true };		//是否使用GetSysTime这个API来获取CPU利用率

    FILETIME m_preidleTime{};
    FILETIME m_prekernelTime{};
    FILETIME m_preuserTime{};

    CPdhCPUUsage m_pdh_cup_usage_query;
};
