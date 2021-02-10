/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2011-2012 Michael Möller <mmoeller@openhardwaremonitor.org>
	
*/

namespace OpenHardwareMonitor.Hardware.HDD {
  using System.Collections.Generic;
  using OpenHardwareMonitor.Collections;

  [NamePrefix("PLEXTOR")]
  internal class SSDPlextor : AbstractHarddrive {

    private static readonly IEnumerable<SmartAttribute> smartAttributes =
      new List<SmartAttribute> {
      new SmartAttribute(0x09, SmartNames.PowerOnHours, RawToInt),
      new SmartAttribute(0x0C, SmartNames.PowerCycleCount, RawToInt),
      new SmartAttribute(0xF1, SmartNames.HostWrites, RawToGb, SensorType.Data, 
        0, SmartNames.HostWrites),
      new SmartAttribute(0xF2, SmartNames.HostReads, RawToGb, SensorType.Data, 
        1, SmartNames.HostReads),
    };

    public SSDPlextor(ISmart smart, string name, string firmwareRevision, 
      int index, ISettings settings)
      : base(smart, name, firmwareRevision, index, smartAttributes, settings) {}

    private static float RawToGb(byte[] rawvalue, byte value,
      IReadOnlyArray<IParameter> parameters) 
    {
      return RawToInt(rawvalue, value, parameters) / 32;
    }
  }
}
