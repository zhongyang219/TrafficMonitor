#pragma once
#include "PdhQuery.h"

class CPdhDiskUsage : public CPdhQuery
{
public:
    CPdhDiskUsage();
    ~CPdhDiskUsage();

    // 获取指定物理磁盘的利用率（0-100）
    // diskIndex: 0=第一个物理磁盘, 1=第二个...
    // 返回值: true=成功, false=失败（首次调用通常失败，需间隔200ms+重试）
    bool GetDiskUsage(int diskIndex, /*out*/ int& usage);

    // 获取物理磁盘名称列表
    const std::vector<CString>& GetDiskNames() const { return m_diskNames; }

    //根据名称查找一个硬盘的序号
    int FindDiskIndex(const std::wstring diskName);

    // 检查磁盘监控是否可用
    bool IsAvailable() const { return m_isAvailable; }

private:
    // 从计数器实例提取磁盘名称
    void ExtractDiskNames();

    // 处理空闲时间
    int CalculateUtilization(double idleTime) const;

private:
    bool m_isAvailable{ false };
    std::vector<CString> m_diskNames;  // 磁盘名称列表，按PDH返回顺序
};