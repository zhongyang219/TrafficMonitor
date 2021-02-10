/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2009-2011 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;

namespace OpenHardwareMonitor.Hardware {
  internal static class HexStringArray {

    public static byte Read(string s, ushort address) {
      string[] lines = s.Split(new[] { '\r', '\n' }, 
        StringSplitOptions.RemoveEmptyEntries);

      foreach (string line in lines) {
        string[] array = line.Split(new[] { ' ', '\t' }, 
          StringSplitOptions.RemoveEmptyEntries);
        if (array.Length == 0)
          continue; 
        if (Convert.ToInt32(array[0], 16) == (address & 0xFFF0)) 
          return Convert.ToByte(array[(address & 0x0F) + 1], 16);
      }

      throw new ArgumentException();
    }
  }
}
