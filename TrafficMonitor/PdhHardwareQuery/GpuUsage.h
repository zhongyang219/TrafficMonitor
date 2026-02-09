#pragma once
#include "PdhQuery.h"

// GPU利用率监控类
class CPdhGPUUsage : public CPdhQuery
{
public:
    CPdhGPUUsage();
    ~CPdhGPUUsage();

    // 获取GPU利用率（0-100）
    // 返回值: true=成功, false=失败
    // 注意：首次调用通常返回false（PDH需要预热），建议间隔200ms以上再次调用
    bool GetGpuUsage(/*out*/ int& usage);

};
