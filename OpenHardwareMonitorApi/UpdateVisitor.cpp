#include "stdafx.h"
#include "UpdateVisitor.h"

namespace OpenHardwareMonitorApi
{
    void UpdateVisitor::VisitComputer(IComputer ^ computer)
    {
        computer->Traverse(this);
    }

    void UpdateVisitor::VisitHardware(IHardware ^ hardware)
    {
        hardware->Update();
        for each (IHardware^ subHardware in hardware->SubHardware)
        {
            subHardware->Accept(this);
        }
    }

    void UpdateVisitor::VisitSensor(ISensor ^ sensor)
    {
    }

    void UpdateVisitor::VisitParameter(IParameter ^ parameter)
    {
    }

}
