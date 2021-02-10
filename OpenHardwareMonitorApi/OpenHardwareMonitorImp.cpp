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

    float COpenHardwareMonitor::GetHardwareTemperature(IHardware^ hardware)
    {
        for (int j = 0; j < hardware->Sensors->Length; j++)
        {
            //找到温度传感器
            if (hardware->Sensors[j]->SensorType == SensorType::Temperature)
            {
                float temperature = Convert::ToDouble(hardware->Sensors[j]->Value);
                //System::Diagnostics::Debug::WriteLine("Temperature:");
                //System::Diagnostics::Debug::WriteLine(computer->Hardware[i]->Sensors[j]->Value.ToString());
                return temperature;
            }
        }

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
            case HardwareType::Mainboard:
                m_main_board_temperature = GetHardwareTemperature(computer->Hardware[i]);
                break;
            case HardwareType::CPU:
                m_cpu_temperature = GetHardwareTemperature(computer->Hardware[i]);
                break;
            case HardwareType::GpuNvidia:
                m_gpu_nvidia_temperature = GetHardwareTemperature(computer->Hardware[i]);
                break;
            case HardwareType::GpuAti:
                m_gpu_ati_temperature = GetHardwareTemperature(computer->Hardware[i]);
                break;
            case HardwareType::HDD:
                m_hdd_temperature = GetHardwareTemperature(computer->Hardware[i]);
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
