/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2010 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Runtime.InteropServices;

namespace OpenHardwareMonitor.Hardware {

  [StructLayout(LayoutKind.Sequential, Pack = 1)]
  internal struct IOControlCode {
    private uint code;

    public IOControlCode(uint deviceType, uint function, Access access) :
      this(deviceType, function, Method.Buffered, access) { }

    public IOControlCode(uint deviceType, uint function, Method method, 
      Access access) 
    {
      code = (deviceType << 16) | 
        ((uint)access << 14) | (function << 2) | (uint)method;
    }

    public enum Method : uint {
      Buffered = 0,
      InDirect = 1,
      OutDirect = 2,
      Neither = 3
    }

    public enum Access : uint {
      Any = 0,
      Read = 1,
      Write = 2
    }
  }    
}
