// 这是主 DLL 文件。

#include "stdafx.h"

#include "OpenHardwareMonitorImp.h"
#include <vector>

namespace OpenHardwareMonitorApi
{
    //将CRL的String类型转换成C++的std::wstring类型
    static std::wstring ClrStringToStdWstring(System::String^ str)
    {
        const wchar_t* chars = reinterpret_cast<const wchar_t*>((System::Runtime::InteropServices::Marshal::StringToHGlobalUni(str)).ToPointer());
        return std::wstring(chars);
    }


    std::shared_ptr<IOpenHardwareMonitor> CreateInstance()
    {
        MonitorGlobal::Instance()->Init();
        return std::make_shared<COpenHardwareMonitor>();
    }

    float COpenHardwareMonitor::CpuTemperature()
    {
        return m_cpu_temperature;
    }

    float COpenHardwareMonitor::GpuTemperature()
    {
        if (m_gpu_nvidia_temperature != 0)
            return m_gpu_nvidia_temperature;
        else
            return m_gpu_ati_temperature;
    }

    float COpenHardwareMonitor::HDDTemperature()
    {
        return m_hdd_temperature;
    }

    float COpenHardwareMonitor::MainboardTemperature()
    {
        return m_main_board_temperature;
    }

    float COpenHardwareMonitor::GpuUsage()
    {
        if (m_gpu_nvidia_usage >= 0)
            return m_gpu_nvidia_usage;
        else
            return m_gpu_ati_usage;
    }

    bool COpenHardwareMonitor::GetHardwareTemperature(IHardware^ hardware, float& temperature)
    {
        temperature = -1;
        std::vector<float> all_temperature;
        for (int i = 0; i < hardware->Sensors->Length; i++)
        {
            //找到温度传感器
            if (hardware->Sensors[i]->SensorType == SensorType::Temperature)
            {
                float cur_temperture = Convert::ToDouble(hardware->Sensors[i]->Value);
                all_temperature.push_back(cur_temperture);
            }
        }
        if (!all_temperature.empty())
        {
            //如果有多个温度传感器，则取平均值
            float sum{};
            for (auto i : all_temperature)
                sum += i;
            temperature = sum / all_temperature.size();
            return true;
       }
        //如果没有找到温度传感器，则在SubHardware中寻找
        for (int i = 0; i < hardware->SubHardware->Length; i++)
        {
            if (GetHardwareTemperature(hardware->SubHardware[i], temperature))
                return true;
        }
        return false;
    }

    bool COpenHardwareMonitor::GetGpuUsage(IHardware ^ hardware, float & gpu_usage)
    {
        for (int i = 0; i < hardware->Sensors->Length; i++)
        {
            //找到负载
            if (hardware->Sensors[i]->SensorType == SensorType::Load)
            {
                if (hardware->Sensors[i]->Name == L"GPU Core")
                {
                    gpu_usage = Convert::ToDouble(hardware->Sensors[i]->Value);
                    return true;
                }
            }

        }
        return false;
    }

    COpenHardwareMonitor::COpenHardwareMonitor()
    {
        ResetAllValues();
    }

    void COpenHardwareMonitor::ResetAllValues()
    {
        m_cpu_temperature = -1;
        m_gpu_nvidia_temperature = -1;
        m_gpu_ati_temperature = -1;
        m_hdd_temperature = -1;
        m_main_board_temperature = -1;
        m_gpu_nvidia_usage = -1;
        m_gpu_ati_usage = -1;
    }

    void COpenHardwareMonitor::GetHardwareInfo()
    {
        ResetAllValues();
        auto computer = MonitorGlobal::Instance()->computer;
        computer->Accept(MonitorGlobal::Instance()->updateVisitor);
        //wchar_t buff[256];
        //swprintf(buff, L"%d\n", computer->Hardware->Length);
        //System::Diagnostics::Debug::WriteLine(gcnew System::String(buff));
        for (int i = 0; i < computer->Hardware->Length; i++)
        {
            //查找硬件类型
            switch (computer->Hardware[i]->HardwareType)
            {
            case HardwareType::CPU:
                if (m_cpu_temperature < 0)
                    GetHardwareTemperature(computer->Hardware[i], m_cpu_temperature);
                break;
            case HardwareType::GpuNvidia:
                if (m_gpu_nvidia_temperature < 0)
                    GetHardwareTemperature(computer->Hardware[i], m_gpu_nvidia_temperature);
                if (m_gpu_nvidia_usage < 0)
                    GetGpuUsage(computer->Hardware[i], m_gpu_nvidia_usage);
                break;
            case HardwareType::GpuAti:
                if (m_gpu_ati_temperature < 0)
                    GetHardwareTemperature(computer->Hardware[i], m_gpu_ati_temperature);
                if (m_gpu_ati_usage < 0)
                    GetGpuUsage(computer->Hardware[i], m_gpu_ati_usage);
                break;
            case HardwareType::HDD:
                if (m_hdd_temperature < 0)
                    GetHardwareTemperature(computer->Hardware[i], m_hdd_temperature);
                break;
            case HardwareType::Mainboard:
                if (m_main_board_temperature < 0)
                    GetHardwareTemperature(computer->Hardware[i], m_main_board_temperature);
                break;
            default:
                break;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////
    MonitorGlobal::MonitorGlobal()
    {

    }

    MonitorGlobal::~MonitorGlobal()
    {

    }

    void MonitorGlobal::Init()
    {
        updateVisitor = gcnew UpdateVisitor();
        computer = gcnew Computer();
        computer->CPUEnabled = true;
        computer->GPUEnabled = true;
        computer->HDDEnabled = true;
        computer->MainboardEnabled = true;
        computer->Open();
        computer->Accept(updateVisitor);
    }

}
