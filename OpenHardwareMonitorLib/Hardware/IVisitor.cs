/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2010 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

namespace OpenHardwareMonitor.Hardware {

  public interface IVisitor {
    void VisitComputer(IComputer computer);
    void VisitHardware(IHardware hardware);
    void VisitSensor(ISensor sensor);
    void VisitParameter(IParameter parameter);
  }

}
