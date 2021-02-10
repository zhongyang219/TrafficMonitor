/*
 
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 
  Copyright (C) 2011-2015 Michael Möller <mmoeller@openhardwaremonitor.org>
	Copyright (C) 2010 Paul Werelds
  Copyright (C) 2011 Roland Reinl <roland-reinl@gmx.de>
	
*/

using System;
using System.Collections.Generic;
using System.Text;
using OpenHardwareMonitor.Collections;

namespace OpenHardwareMonitor.Hardware.HDD {

  [NamePrefix("")]
  internal class GenericHarddisk : AbstractHarddrive {

    private static readonly List<SmartAttribute> smartAttributes =
      new List<SmartAttribute> {
      new SmartAttribute(0x01, SmartNames.ReadErrorRate),
      new SmartAttribute(0x02, SmartNames.ThroughputPerformance),
      new SmartAttribute(0x03, SmartNames.SpinUpTime),
      new SmartAttribute(0x04, SmartNames.StartStopCount, RawToInt),
      new SmartAttribute(0x05, SmartNames.ReallocatedSectorsCount),
      new SmartAttribute(0x06, SmartNames.ReadChannelMargin),
      new SmartAttribute(0x07, SmartNames.SeekErrorRate),
      new SmartAttribute(0x08, SmartNames.SeekTimePerformance),
      new SmartAttribute(0x09, SmartNames.PowerOnHours, RawToInt),
      new SmartAttribute(0x0A, SmartNames.SpinRetryCount),
      new SmartAttribute(0x0B, SmartNames.RecalibrationRetries),
      new SmartAttribute(0x0C, SmartNames.PowerCycleCount, RawToInt),
      new SmartAttribute(0x0D, SmartNames.SoftReadErrorRate),
      new SmartAttribute(0xAA, SmartNames.Unknown), 
      new SmartAttribute(0xAB, SmartNames.Unknown), 
      new SmartAttribute(0xAC, SmartNames.Unknown),
      new SmartAttribute(0xB7, SmartNames.SataDownshiftErrorCount, RawToInt),
      new SmartAttribute(0xB8, SmartNames.EndToEndError),
      new SmartAttribute(0xB9, SmartNames.HeadStability),
      new SmartAttribute(0xBA, SmartNames.InducedOpVibrationDetection),
      new SmartAttribute(0xBB, SmartNames.ReportedUncorrectableErrors, RawToInt),
      new SmartAttribute(0xBC, SmartNames.CommandTimeout, RawToInt),
      new SmartAttribute(0xBD, SmartNames.HighFlyWrites),
      new SmartAttribute(0xBF, SmartNames.GSenseErrorRate),
      new SmartAttribute(0xC0, SmartNames.EmergencyRetractCycleCount),
      new SmartAttribute(0xC1, SmartNames.LoadCycleCount),
      new SmartAttribute(0xC3, SmartNames.HardwareEccRecovered),
      new SmartAttribute(0xC4, SmartNames.ReallocationEventCount),
      new SmartAttribute(0xC5, SmartNames.CurrentPendingSectorCount),
      new SmartAttribute(0xC6, SmartNames.UncorrectableSectorCount),
      new SmartAttribute(0xC7, SmartNames.UltraDmaCrcErrorCount),
      new SmartAttribute(0xC8, SmartNames.WriteErrorRate),
      new SmartAttribute(0xCA, SmartNames.DataAddressMarkErrors),
      new SmartAttribute(0xCB, SmartNames.RunOutCancel),
      new SmartAttribute(0xCC, SmartNames.SoftEccCorrection),
      new SmartAttribute(0xCD, SmartNames.ThermalAsperityRate),
      new SmartAttribute(0xCE, SmartNames.FlyingHeight),
      new SmartAttribute(0xCF, SmartNames.SpinHighCurrent),
      new SmartAttribute(0xD0, SmartNames.SpinBuzz),
      new SmartAttribute(0xD1, SmartNames.OfflineSeekPerformance),
      new SmartAttribute(0xD3, SmartNames.VibrationDuringWrite),
      new SmartAttribute(0xD4, SmartNames.ShockDuringWrite),
      new SmartAttribute(0xDC, SmartNames.DiskShift),
      new SmartAttribute(0xDD, SmartNames.AlternativeGSenseErrorRate), 
      new SmartAttribute(0xDE, SmartNames.LoadedHours),
      new SmartAttribute(0xDF, SmartNames.LoadUnloadRetryCount),
      new SmartAttribute(0xE0, SmartNames.LoadFriction),
      new SmartAttribute(0xE1, SmartNames.LoadUnloadCycleCount),
      new SmartAttribute(0xE2, SmartNames.LoadInTime),
      new SmartAttribute(0xE3, SmartNames.TorqueAmplificationCount),
      new SmartAttribute(0xE4, SmartNames.PowerOffRetractCycle),
      new SmartAttribute(0xE6, SmartNames.GmrHeadAmplitude),      
      new SmartAttribute(0xE8, SmartNames.EnduranceRemaining),
      new SmartAttribute(0xE9, SmartNames.PowerOnHours),
      new SmartAttribute(0xF0, SmartNames.HeadFlyingHours),
      new SmartAttribute(0xF1, SmartNames.TotalLbasWritten),
      new SmartAttribute(0xF2, SmartNames.TotalLbasRead),
      new SmartAttribute(0xFA, SmartNames.ReadErrorRetryRate),
      new SmartAttribute(0xFE, SmartNames.FreeFallProtection),

      new SmartAttribute(0xC2, SmartNames.Temperature, 
        (byte[] r, byte v, IReadOnlyArray<IParameter> p) 
          => { return r[0] + (p == null ? 0 : p[0].Value); }, 
          SensorType.Temperature, 0, SmartNames.Temperature, false, 
        new[] { new ParameterDescription("Offset [°C]", 
                  "Temperature offset of the thermal sensor.\n" + 
                  "Temperature = Value + Offset.", 0) }),
      new SmartAttribute(0xE7, SmartNames.Temperature, 
        (byte[] r, byte v, IReadOnlyArray<IParameter> p) 
          => { return r[0] + (p == null ? 0 : p[0].Value); }, 
          SensorType.Temperature, 0, SmartNames.Temperature, false, 
        new[] { new ParameterDescription("Offset [°C]", 
                  "Temperature offset of the thermal sensor.\n" + 
                  "Temperature = Value + Offset.", 0) }),
      new SmartAttribute(0xBE, SmartNames.TemperatureDifferenceFrom100, 
        (byte[] r, byte v, IReadOnlyArray<IParameter> p) 
          => { return r[0] + (p == null ? 0 : p[0].Value); }, 
          SensorType.Temperature, 0, "Temperature", false, 
        new[] { new ParameterDescription("Offset [°C]", 
                  "Temperature offset of the thermal sensor.\n" + 
                  "Temperature = Value + Offset.", 0) })
    };

    public GenericHarddisk(ISmart smart, string name, string firmwareRevision, 
      int index, ISettings settings)
      : base(smart, name, firmwareRevision, index, smartAttributes, settings) {}
  }
}
