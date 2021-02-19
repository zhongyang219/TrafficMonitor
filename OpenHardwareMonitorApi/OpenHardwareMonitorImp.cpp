// 这是主 DLL 文件。

#include "stdafx.h"

#include "OpenHardwareMonitorImp.h"

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

    bool COpenHardwareMonitor::GetHardwareTemperature(IHardware^ hardware, float& temperature)
    {
        temperature = 0;
        for (int i = 0; i < hardware->Sensors->Length; i++)
        {
            //找到温度传感器
            if (hardware->Sensors[i]->SensorType == SensorType::Temperature)
            {
                temperature = Convert::ToDouble(hardware->Sensors[i]->Value);
                return true;
            }
        }
        //如果没有找到温度传感器，则在SubHardware中寻找
        for (int i = 0; i < hardware->SubHardware->Length; i++)
        {
            if (GetHardwareTemperature(hardware->SubHardware[i], temperature))
                return true;
        }
        return false;
    }

    void COpenHardwareMonitor::GetHardwareInfo()
    {
        m_cpu_temperature = 0;
        m_gpu_nvidia_temperature = 0;
        m_gpu_ati_temperature = 0;
        m_hdd_temperature = 0;
        m_main_board_temperature = 0;

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
                if (m_cpu_temperature == 0)
                    GetHardwareTemperature(computer->Hardware[i], m_cpu_temperature);
                break;
            case HardwareType::GpuNvidia:
                if (m_gpu_nvidia_temperature == 0)
                    GetHardwareTemperature(computer->Hardware[i], m_gpu_nvidia_temperature);
                break;
            case HardwareType::GpuAti:
                if (m_gpu_ati_temperature == 0)
                    GetHardwareTemperature(computer->Hardware[i], m_gpu_ati_temperature);
                break;
            case HardwareType::HDD:
                if (m_hdd_temperature == 0)
                    GetHardwareTemperature(computer->Hardware[i], m_hdd_temperature);
                break;
            case HardwareType::Mainboard:
                if (m_main_board_temperature == 0)
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
        updateVisitor = gcnew UpdateVisitor();
        computer = gcnew Computer();
        computer->CPUEnabled = true;
        computer->GPUEnabled = true;
        computer->HDDEnabled = true;
        computer->MainboardEnabled = true;
        computer->Open();
        computer->Accept(updateVisitor);

    }

    MonitorGlobal::~MonitorGlobal()
    {

    }

}
