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
  internal struct DriveAttributeValue {
    public byte Identifier;
    public short StatusFlags;
    public byte AttrValue;
    public byte WorstValue;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
    public byte[] RawValue;
    public byte Reserved;
  }  

}
