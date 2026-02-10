#include "stdafx.h"
#include "GpuUsage.h"

///////////////////////////////////////////////////////////////////////////////////////////
// CPdhGPUUsage 实现
///////////////////////////////////////////////////////////////////////////////////////////
CPdhGPUUsage::CPdhGPUUsage()
: CPdhQuery(_T("\\GPU Engine(*)\\Utilization Percentage"))
{
}

CPdhGPUUsage::~CPdhGPUUsage()
{
}

bool CPdhGPUUsage::GetGpuUsage(int& usage)
{
    if (isInitialized)
    {
        std::vector<CounterValueItem> valueItems;
        if (QueryValues(valueItems))
        {
            if (!valueItems.empty())
            {
                //获取所有类型的利用率
                std::map<std::wstring, double> gpu_usage_map;
                for (const auto& item : valueItems)
                {
                    std::wstring item_name = item.name;
                    size_t index = item.name.rfind(L'_');
                    if (index != std::wstring::npos)
                        item_name = item.name.substr(index + 1);
                    gpu_usage_map[item_name] += item.value;
                }
                //查找所有类型中最大的值作为总利用率（同Windows任务管理器的处理）
                double max_value = 0;
                for (const auto& item : gpu_usage_map)
                {
                    if (item.second > max_value)
                        max_value = item.second;
                }

                usage = static_cast<int>(max_value + 0.5);  // 四舍五入
                usage = min(max(usage, 0), 100);        // 限制在0-100范围
                return true;
            }
        }
    }

    return false;
}
