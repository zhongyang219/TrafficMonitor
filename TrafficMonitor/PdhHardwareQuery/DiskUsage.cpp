#include "stdafx.h"
#include "DiskUsage.h"
#include <algorithm>
#include <cmath>

CPdhDiskUsage::CPdhDiskUsage()
    : CPdhQuery(_T("\\PhysicalDisk(*)\\% Idle Time"))
{
    m_isAvailable = Initialize();
    if (m_isAvailable)
    {
        // 预热：触发PDH内部初始化（首次QueryValues会填充实例列表）
        std::vector<CounterValueItem> dummy;
        QueryValues(dummy);
        ExtractDiskNames();
    }
}

CPdhDiskUsage::~CPdhDiskUsage()
{
}

void CPdhDiskUsage::ExtractDiskNames()
{
    m_diskNames.clear();
    std::vector<CounterValueItem> values;

    if (!QueryValues(values))
        return;

    for (const auto& item : values)
    {
        CString name(item.name.c_str());
        m_diskNames.push_back(name);
    }
}

int CPdhDiskUsage::CalculateUtilization(double idleTime) const
{
    // 关键处理：NVMe/RAID等多队列磁盘的空闲时间可能 >100%
    // 例如: 4队列磁盘空闲时间=400% → 实际空闲=100% → 利用率=0%
    if (idleTime > 100.0)
        idleTime = 100.0;

    // 确保范围在0-100
    idleTime = (std::max)(0.0, (std::min)(100.0, idleTime));

    // 利用率 = 100% - 空闲时间
    double utilization = 100.0 - idleTime;
    return static_cast<int>(utilization + 0.5); // 四舍五入
}

bool CPdhDiskUsage::GetDiskUsage(int diskIndex, int& usage)
{
    usage = 0;
    if (!m_isAvailable)
        return false;

    std::vector<CounterValueItem> values;
    if (!QueryValues(values) || values.empty())
        return false;

    if (diskIndex >= 0 && diskIndex < static_cast<int>(values.size()))
    {
        usage = CalculateUtilization(values[diskIndex].value);
        return true;
    }
    return false;

}

int CPdhDiskUsage::FindDiskIndex(const std::wstring diskName)
{
    int disk_index = -1;
    for (int i = 0; i < static_cast<int>(m_diskNames.size()); i++)
    {
        if (diskName == m_diskNames[i].GetString())
        {
            disk_index = i;
            break;
        }
    }
    return disk_index;
}
