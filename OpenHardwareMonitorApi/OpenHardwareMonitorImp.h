// OpenHardwareMonitorApi.h

#pragma once
#include <string>
#include "OpenHardwareMonitor/OpenHardwareMonitorApi.h"
#include "UpdateVisitor.h"
#include <map>

using namespace System;
using namespace LibreHardwareMonitor::Hardware;

namespace OpenHardwareMonitorApi {

    public class COpenHardwareMonitor : public IOpenHardwareMonitor
	{
    public:
        COpenHardwareMonitor();
        virtual ~COpenHardwareMonitor();

        virtual void GetHardwareInfo() override;
        virtual float CpuTemperature() override;
        virtual float GpuTemperature() override;
        virtual float HDDTemperature() override;
        virtual float MainboardTemperature() override;
        virtual float GpuUsage() override;
        virtual float CpuFreq() override;
        virtual float CpuUsage() override;
        virtual const std::map<std::wstring, float>& AllHDDTemperature() override;
        virtual const std::map<std::wstring, float>& AllCpuTemperature() override;
        virtual const std::map<std::wstring, float>& AllHDDUsage() override;

        virtual void SetCpuEnable(bool enable) override;
        virtual void SetGpuEnable(bool enable) override;
        virtual void SetHddEnable(bool enable) override;
        virtual void SetMainboardEnable(bool enable) override;

    private:
        bool GetHardwareTemperature(IHardware^ hardware, float& temperature);
        bool GetCpuTemperature(IHardware^ hardware, float& temperature);
        bool GetGpuUsage(IHardware^ hardware, float& gpu_usage);
        bool GetHddUsage(IHardware^ hardware, float& hdd_usage);
        bool GetCPUFreq(IHardware^ hardware, float& freq);
        bool GetCpuUsage(IHardware^ hardware, float& cpu_usage);
        void ResetAllValues();
        //向map中插入一个数值，如果key已经存在，则自动对新插入的key重命名
        static void InsertValueToMap(std::map<std::wstring, float>& value_map, const std::wstring& key, float value);

    private:
        float m_cpu_temperature{};
        float m_gpu_nvidia_temperature{};
        float m_gpu_ati_temperature{};
        float m_gpu_intel_temperature{};
        float m_hdd_temperature{};
        float m_main_board_temperature{};
        float m_gpu_nvidia_usage{};
        float m_gpu_ati_usage{};
        float m_gpu_intel_usage{};
        float m_cpu_freq{};
        float m_cpu_usage{};
        std::map<std::wstring, float> m_all_hdd_temperature;
        std::map<std::wstring, float> m_all_cpu_temperature;
        std::map<std::wstring, float> m_all_cpu_clock;
        std::map<std::wstring, float> m_all_hdd_usage;
    };

    //一个单实例类
    //由于COpenHardwareMonitor是非托管类，不能将托管类的对象作为成员变量，此类用于保存托管类的对象
    public ref class MonitorGlobal
    {
    public:
        MonitorGlobal();
        ~MonitorGlobal();
        static MonitorGlobal^ Instance()
        {
            if (m_instance == nullptr)
            {
                m_instance = gcnew MonitorGlobal();
            }
            return m_instance;
        }

        void Init();
        void UnInit();

        Computer^ computer;
        UpdateVisitor^ updateVisitor{};

    private:
        static MonitorGlobal^ m_instance{};
    };
}
