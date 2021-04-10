// OpenHardwareMonitorApi.h

#pragma once
#include <string>
#include "OpenHardwareMonitor/OpenHardwareMonitorApi.h"
#include "UpdateVisitor.h"

using namespace System;
using namespace OpenHardwareMonitor::Hardware;

namespace OpenHardwareMonitorApi {

    public class COpenHardwareMonitor : public IOpenHardwareMonitor
	{
    public:
        COpenHardwareMonitor();

        virtual void GetHardwareInfo() override;
        virtual float CpuTemperature() override;
        virtual float GpuTemperature() override;
        virtual float HDDTemperature() override;
        virtual float MainboardTemperature() override;
        virtual float GpuUsage() override;

    private:
        bool GetHardwareTemperature(IHardware^ hardware, float& temperature);
        bool GetGpuUsage(IHardware^ hardware, float& gpu_usage);

        void ResetAllValues();

    private:
        float m_cpu_temperature{};
        float m_gpu_nvidia_temperature{};
        float m_gpu_ati_temperature{};
        float m_hdd_temperature{};
        float m_main_board_temperature{};
        float m_gpu_nvidia_usage{};
        float m_gpu_ati_usage{};
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

        Computer^ computer;
        UpdateVisitor^ updateVisitor{};

    private:
        static MonitorGlobal^ m_instance{};
    };
}
