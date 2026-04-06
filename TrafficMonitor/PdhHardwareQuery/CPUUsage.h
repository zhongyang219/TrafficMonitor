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

    int GetCpuUsage();

private:
    int GetCpuUsageByGetSystemTimes();

private:

    FILETIME m_preidleTime{};
    FILETIME m_prekernelTime{};
    FILETIME m_preuserTime{};

    CPdhCPUUsage m_pdh_cup_usage_query;
};
