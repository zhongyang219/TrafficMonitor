#pragma once
#include <Pdh.h>
#include <PdhMsg.h>

class CPdhQuery
{
public:
    CPdhQuery(LPCTSTR _fullCounterPath);
    virtual ~CPdhQuery();

protected:
    bool Initialize();
    bool QueryValue(double& value);

protected:
    HQUERY query = nullptr;
    HCOUNTER counter = nullptr;
    bool isInitialized = false;
    CString fullCounterPath;
};

class CCPUUsage : public CPdhQuery
{
public:
    CCPUUsage();

    ~CCPUUsage()
    {}

    void SetUseCPUTimes(bool use_get_system_times);		//设置获取CPU利用率的方式，是通过GetSystemTimes还是Pdh
    int GetCpuUsage();

private:
    int GetCpuUsageByGetSystemTimes();
    bool GetCPUUsageByPdh(int& cpu_usage);

private:
    bool m_use_get_system_times{ true };		//是否使用GetSysTime这个API来获取CPU利用率

    FILETIME m_preidleTime{};
    FILETIME m_prekernelTime{};
    FILETIME m_preuserTime{};

};

class CCpuFreq : public CPdhQuery
{
public:
    CCpuFreq();

    // 调用此函数获取CPU频率。
    // 注意：第二次调用此函数开始才能获取到值，两次调用的时间不应过短，最好大于200毫秒
    bool GetCpuFreq(float& freq);

private:
    float max_cpu_freq = 0;
};
