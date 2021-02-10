/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2012-2020 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace OpenHardwareMonitor.Hardware {
  public static class OperatingSystem {

    static OperatingSystem() {
      int p = (int)Environment.OSVersion.Platform;
      IsUnix = (p == 4) || (p == 6) || (p == 128);

      Is64BitOperatingSystem = GetIs64BitOperatingSystem();
    }

    public static bool IsUnix { get; }

    public static bool Is64BitOperatingSystem { get; }

    private static bool GetIs64BitOperatingSystem() {
      if (IntPtr.Size == 8)
        return true;

      try {
        bool wow64Process;
        bool result = IsWow64Process(
          Process.GetCurrentProcess().Handle, out wow64Process);

        return result && wow64Process;
      } catch (EntryPointNotFoundException) {
        return false;
      }
    }

    [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool IsWow64Process(IntPtr hProcess,
      out bool wow64Process);
  }
}
