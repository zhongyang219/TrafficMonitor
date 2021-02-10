/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2011-2012 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
namespace OpenHardwareMonitor.Hardware.HDD {

  [StructLayout(LayoutKind.Sequential, Pack = 1)]
  internal struct DriveThresholdValue {
    public byte Identifier;
    public byte Threshold;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 10)]
    public byte[] Unknown;
  }

}
