#pragma once
#include <memory>
#include "OpenHardwareMonitorGlobal.h"
#include <map>
#include <string>

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
        virtual float CpuFreq() = 0;            //返回获取到的CPU频率
        virtual float CpuUsage() = 0;           //返回获取的CPU利用率
        virtual const std::map<std::wstring, float>& AllHDDTemperature() = 0;   //返回所有硬盘的温度。map的key是硬盘的名称，value是硬盘的温度
        virtual const std::map<std::wstring, float>& AllCpuTemperature() = 0;   //返回所有CPU（核心）的温度。map的key是CPU的名称，value是硬盘的温度
        virtual const std::map<std::wstring, float>& AllHDDUsage() = 0;         //返回所有硬盘的使用率

        virtual void SetCpuEnable(bool enable) = 0;
        virtual void SetGpuEnable(bool enable) = 0;
        virtual void SetHddEnable(bool enable) = 0;
        virtual void SetMainboardEnable(bool enable) = 0;
    };

    OPENHARDWAREMONITOR_API std::shared_ptr<IOpenHardwareMonitor> CreateInstance();
    OPENHARDWAREMONITOR_API std::wstring GetErrorMessage();
}
