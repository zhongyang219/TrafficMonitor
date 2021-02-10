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

    float COpenHardwareMonitor::GetCpuTemperature()
    {
        auto computer = MonitorGlobal::Instance()->computer;
        computer->Accept(MonitorGlobal::Instance()->updateVisitor);
        float temperature{};
        //wchar_t buff[256];
        //swprintf(buff, L"%d\n", computer->Hardware->Length);
        //System::Diagnostics::Debug::WriteLine(gcnew System::String(buff));
        for (int i = 0; i < computer->Hardware->Length; i++)
        {
            //查找硬件类型为CPU
            if (computer->Hardware[i]->HardwareType == HardwareType::CPU)
            {
                for (int j = 0; j < computer->Hardware[i]->Sensors->Length; j++)
                {
                    //找到温度传感器
                    if (computer->Hardware[i]->Sensors[j]->SensorType == SensorType::Temperature)
                    {
                        temperature = Convert::ToDouble(computer->Hardware[i]->Sensors[j]->Value);
                        //System::Diagnostics::Debug::WriteLine("Temperature:");
                        //System::Diagnostics::Debug::WriteLine(computer->Hardware[i]->Sensors[j]->Value.ToString());
                    }
                }
            }
        }
        return temperature;
    }

    float COpenHardwareMonitor::GetGpuTemperature()
    {
        return 0;
    }


    ////////////////////////////////////////////////////////////////////////////////////
    MonitorGlobal::MonitorGlobal()
    {
        updateVisitor = gcnew UpdateVisitor();
        computer = gcnew Computer();
        computer->CPUEnabled = true;
        computer->Open();
        computer->Accept(updateVisitor);

    }

    MonitorGlobal::~MonitorGlobal()
    {

    }

}
