#pragma once
#include <memory>
#include "OpenHardwareMonitorGlobal.h"

namespace OpenHardwareMonitorApi
{
    class IOpenHardwareMonitor
    {
    public:
        virtual void GetHardwareInfo() = 0;     //获取一次硬件信息
        virtual float CpuTemperature() = 0;     //返回获取到的CPU温度
        virtual float GpuTemperature() = 0;     //返回获取到的GPU温度
        virtual float HDDTemperature() = 0;     //返回获取到的硬盘温度
        virtual float MainboardTemperature() = 0;   //返回获取到的主板温度
        virtual float GpuUsage() = 0;           //返回获取到的GPU利用率
    };

    OPENHARDWAREMONITOR_API std::shared_ptr<IOpenHardwareMonitor> CreateInstance();
}
