#pragma once

#include "OpenHardwareMonitor/OpenHardwareMonitorGlobal.h"

using namespace HARDWAREMONITOR_HARDWARE_NAMESPACE;
namespace OpenHardwareMonitorApi
{
    public ref class UpdateVisitor : IVisitor
    {
    public:
        virtual void VisitComputer(IComputer^ computer);
        virtual void VisitHardware(IHardware^ hardware);
        virtual void VisitSensor(ISensor^ sensor);
        virtual void VisitParameter(IParameter^ parameter);
    };

}

