#pragma once
#include <Pdh.h>
#include <PdhMsg.h>
#include <string.h>
#include <memory>
#include "TrafficMonitor.h"
#pragma comment(lib,"pdh.lib")

constexpr uint32_t k_cpu_time_counts { 3 };

class CpuUsageHelper {

public:

    CpuUsageHelper(const wchar_t* query_str,
                   PDH_RAW_COUNTER* prev_raw_counter_ptr)
        : m_query_str(query_str),
          m_prev_raw_counter_ptr(prev_raw_counter_ptr)
            { PdhOpenQuery(NULL, 0, &m_query); }

    ~CpuUsageHelper() {
        PdhCloseQuery(m_query); }

    int32_t                GetCpuUsage();

    const PDH_RAW_COUNTER& GetRawCounter();

private:

    const wchar_t*       m_query_str { };

    PDH_RAW_COUNTER*     m_prev_raw_counter_ptr { };

    HQUERY               m_query { };

    HCOUNTER             m_counter { };

    PDH_RAW_COUNTER      m_raw_counter { };

    PDH_FMT_COUNTERVALUE m_counter_value { };

};

class CpuUsage {

public:

    CpuUsage()
        : m_query_str(
            theApp.m_win_version.GetMajorVersion() >= 10 ?
                L"\\Processor Information(_Total)\\% Processor Utility":
                L"\\Processor Information(_Total)\\% Processor Time") { }

    ~CpuUsage() { }

    void		UseCpuTimes(const bool use_cpu_times) {
                    if (m_by_cpu_times != use_cpu_times) {
                        m_by_cpu_times = use_cpu_times;
                        m_had_cpu_util = false;
                    } }

    int32_t		GetCpuUsage() {
                    return m_by_cpu_times ? BySystemTimes() : ByPerfDataHelper(); }

private:

    int32_t		BySystemTimes();

    int32_t		ByPerfDataHelper();

    uint64_t	GetCpuSpan(const FILETIME& prev, const FILETIME& curr) {
                    return GetCpuTime(curr, m_cpu_spans[4])-
                           GetCpuTime(prev, m_cpu_spans[3]); }

    uint64_t&   GetCpuTime(const FILETIME& time, uint64_t& cache) {
                    cache = time.dwHighDateTime;
                    cache <<= 32;
                    cache |= time.dwLowDateTime;
                    return cache; }

private:

    /* Perf-data-helper: */
    PDH_RAW_COUNTER m_prev_raw_counter { };

    /* System-times: prev(idle, kernel, user) curr(...) */
    FILETIME        m_cpu_times[6] { };

    uint64_t        m_cpu_spans[5] { };

    const wchar_t*  m_query_str { };

    bool            m_by_cpu_times { },
                    m_had_cpu_util { };

};
