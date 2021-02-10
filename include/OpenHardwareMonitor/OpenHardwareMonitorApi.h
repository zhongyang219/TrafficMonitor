#pragma once
#include <memory>
#include <string>
#include "OpenHardwareMonitorGlobal.h"

namespace OpenHardwareMonitorApi
{
    class OPENHARDWAREMONITOR_API IOpenHardwareMonitor
    {
    public:
        virtual float GetCpuTemperature() = 0;
        virtual float GetGpuTemperature() = 0;
    };

    OPENHARDWAREMONITOR_API std::shared_ptr<IOpenHardwareMonitor> CreateInstance();
}
