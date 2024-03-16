#include "stdafx.h"
#include "CpuUsage.h"

int32_t
CpuUsage::BySystemTimes()
{
    GetSystemTimes(&m_cpu_times[3], &m_cpu_times[4], &m_cpu_times[5]);
    for (uint32_t prev_index { 0 }; prev_index < k_cpu_time_counts; ++prev_index) {
        const uint32_t curr_index { prev_index + k_cpu_time_counts };
        m_cpu_spans[prev_index] =
            GetCpuSpan(m_cpu_times[prev_index], m_cpu_times[curr_index]);
        m_cpu_times[prev_index] = m_cpu_times[curr_index];
    }
    const uint64_t& total_span { m_cpu_spans[1] + m_cpu_spans[2] };
    return total_span ? static_cast<int32_t>((total_span - m_cpu_spans[0]) * 100 / total_span) : 0;
}

int32_t
CpuUsage::ByPerfDataHelper()
{
    CpuUsageHelper usage_helper{
        m_query_str, m_had_cpu_util ? &m_prev_raw_counter : NULL };
    const auto& prev_counter_cache { usage_helper.GetRawCounter() };
    const int32_t cpu_usage { usage_helper.GetCpuUsage() };
    m_prev_raw_counter = prev_counter_cache;
    if (!m_had_cpu_util) m_had_cpu_util = true;
    return cpu_usage;
}

const PDH_RAW_COUNTER&
CpuUsageHelper::GetRawCounter()
{
    PdhAddCounter(m_query, m_query_str, NULL, &m_counter);
    PdhCollectQueryData(m_query);
    PdhGetRawCounterValue(m_counter, NULL, &m_raw_counter);
    return m_raw_counter;
}

int32_t
CpuUsageHelper::GetCpuUsage()
{
    if (m_prev_raw_counter_ptr) {
        PdhCalculateCounterFromRawValue(
            m_counter, PDH_FMT_DOUBLE, &m_raw_counter,
            m_prev_raw_counter_ptr, &m_counter_value
        );
        return static_cast<int32_t>(m_counter_value.doubleValue);
    } else {
        return 0;
    }
}
