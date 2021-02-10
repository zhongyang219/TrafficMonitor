/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2010 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;

namespace OpenHardwareMonitor.Hardware {

  public class SensorVisitor : IVisitor {
    private readonly SensorEventHandler handler;

    public SensorVisitor(SensorEventHandler handler) {
      if (handler == null)
        throw new ArgumentNullException("handler");
      this.handler = handler;
    }

    public void VisitComputer(IComputer computer) {
      if (computer == null)
        throw new ArgumentNullException("computer");
      computer.Traverse(this);
    }

    public void VisitHardware(IHardware hardware) {
      if (hardware == null)
        throw new ArgumentNullException("hardware");
      hardware.Traverse(this);
    }

    public void VisitSensor(ISensor sensor) {
      handler(sensor);
    }

    public void VisitParameter(IParameter parameter) { }
  }
}
