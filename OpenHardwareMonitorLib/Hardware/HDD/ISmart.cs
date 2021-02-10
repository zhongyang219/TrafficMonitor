/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2011-2012 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

using System;
using System.Collections.Generic;


namespace OpenHardwareMonitor.Hardware.HDD {

  internal interface ISmart {

    IntPtr OpenDrive(int driveNumber);

    bool EnableSmart(IntPtr handle, int driveNumber);

    DriveAttributeValue[] ReadSmartData(IntPtr handle, int driveNumber);

    DriveThresholdValue[] ReadSmartThresholds(IntPtr handle, int driveNumber);

    bool ReadNameAndFirmwareRevision(IntPtr handle, int driveNumber,
      out string name, out string firmwareRevision); 

    void CloseHandle(IntPtr handle);

    IntPtr InvalidHandle { get; }

    string[] GetLogicalDrives(int driveIndex);
  }
}
